#include "OBabel.h"

#ifdef MOD_HAVE_OPENBABEL
#include <mod/Config.h>
#include <mod/Error.h>

#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/IO.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>
#include <openbabel/op.h>

namespace mod {
namespace lib {
namespace Chem {
namespace {

template<typename Graph>
std::unique_ptr<OpenBabel::OBMol> makeOBMolImpl(const Graph &g,
		std::function<const AtomData &(typename boost::graph_traits<Graph>::vertex_descriptor) > atomData,
		std::function<BondType(typename boost::graph_traits<Graph>::edge_descriptor) > bondData,
		bool withHydrogen, bool ignoreDuplicateBonds) {
	typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
	typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
	std::unique_ptr<OpenBabel::OBMol> mol(new OpenBabel::OBMol());
	mol->BeginModify();

	// add all visible vertices
	for(Vertex v : asRange(vertices(g))) {
		unsigned int vId = get(boost::vertex_index_t(), g, v);
		if(!withHydrogen && isCollapsible(v, g, atomData, bondData)) continue;
		OpenBabel::OBAtom *atom = mol->NewAtom(vId);
		assert(atomData(v).getAtomId() != AtomIds::Invalid);
		atom->SetAtomicNum(atomData(v).getAtomId());
		atom->SetFormalCharge(atomData(v).getCharge());
		atom->SetType(symbolFromAtomId(atomData(v).getAtomId()));
	}

	// add all edges
	for(Edge e : asRange(edges(g))) {
		Vertex vSrc = source(e, g);
		Vertex vTar = target(e, g);
		unsigned int srcId = get(boost::vertex_index_t(), g, vSrc);
		unsigned int tarId = get(boost::vertex_index_t(), g, vTar);
		if(!withHydrogen && isCollapsible(vSrc, g, atomData, bondData)) continue;
		if(!withHydrogen && isCollapsible(vTar, g, atomData, bondData)) continue;
		OpenBabel::OBAtom *src = mol->GetAtomById(srcId), *tar = mol->GetAtomById(tarId);
		assert(src);
		assert(tar);
		int order;
		switch(bondData(e)) {
		case BondType::Invalid:
		case BondType::Single:
			order = 1;
			break;
		case BondType::Aromatic:
			order = 5;
			break;
		case BondType::Double:
			order = 2;
			break;
		case BondType::Triple:
			order = 3;
			break;
		default:
			std::abort();
		}
		OpenBabel::OBBond *bond = mol->GetBond(src, tar);
		if(!ignoreDuplicateBonds && bond) MOD_ABORT;
		if(!bond) {
			bond = mol->NewBond();
			bond->SetBegin(src);
			bond->SetEnd(tar);
			bond->SetBondOrder(order);
			src->AddBond(bond);
			tar->AddBond(bond);
			assert(mol->GetBond(src, tar));
		}
	}
	OpenBabel::OBConversion conv; // because the constructor apparently takes care of lib loading
	OpenBabel::OBOp *op = OpenBabel::OBOp::FindType("gen2D");
	assert(op);
	bool res = op->Do(mol.get());
	assert(res);
	mol->EndModify();
	return mol;
}

} // namespace

std::unique_ptr<OpenBabel::OBMol> copyOBMol(const OpenBabel::OBMol &mol) {
	return std::unique_ptr<OpenBabel::OBMol>(new OpenBabel::OBMol(mol));
}

std::unique_ptr<OpenBabel::OBMol> makeOBMol(const lib::Graph::GraphType &g,
		std::function<const AtomData &(lib::Graph::Vertex) > atomData,
		std::function<BondType(lib::Graph::Edge) > bondData,
		bool withHydrogen) {
	return makeOBMolImpl(g, atomData, bondData, withHydrogen, false);
}

std::unique_ptr<OpenBabel::OBMol> makeOBMol(const lib::Rules::GraphType &g,
		std::function<const AtomData &(lib::Rules::Vertex) > atomData,
		std::function<BondType(lib::Rules::Edge) > bondData) {
	return makeOBMolImpl(g, atomData, bondData, true, true);
}

double getMolarMass(OpenBabel::OBMol &mol) {
	double weight = mol.GetMolWt(false);
	unsigned int intWeight = weight * 1024;
	return static_cast<double> (intWeight) / 1024;
}

double getEnergy(OpenBabel::OBMol &mol) {
	if(getConfig().obabel.verbose.get()) IO::log() << "OBabel energy: '" << mol.GetTitle() << "'"
		<< "\t" << mol.NumAtoms() << " atoms\t" << mol.NumBonds() << " bonds"
		<< std::endl;
	// code originally from GGL

	OpenBabel::OBForceField* pFF = nullptr;

	unsigned int conformers = 25;
	unsigned int geomSteps = 100;

	{ // gen3D stuff
		if(getConfig().obabel.verbose.get()) IO::log() << "\tgen 3D" << std::endl;
		pFF = OpenBabel::OBForceField::FindForceField("MMFF94");
		assert(pFF);
		OpenBabel::OBBuilder builder;
		builder.Build(mol);
		mol.SetDimension(3);

		const bool setupOK = pFF->Setup(mol);
		if(!setupOK) {
			IO::log() << "ERROR: Setup of Open Babel forcefield failed, using 0 as energy." << std::endl;
			return 0;
		}

		// Since we only want a rough geometry, use distance cutoffs for VDW, Electrostatics
		pFF->EnableCutOff(true);
		pFF->SetVDWCutOff(10.0);
		pFF->SetElectrostaticCutOff(20.0);
		pFF->SetUpdateFrequency(10); // update non-bonded distances

		pFF->SteepestDescent(250, 1.0e-4);
		pFF->WeightedRotorSearch(conformers, geomSteps);
		pFF->ConjugateGradients(250, 1.0e-6);
		pFF->UpdateCoordinates(mol);
	}

	{// conformer stuff
		if(getConfig().obabel.verbose.get()) IO::log() << "\tconformer" << std::endl;
		pFF = OpenBabel::OBForceField::FindForceField("MMFF94");
		assert(pFF);
		const bool setupOK = pFF->Setup(mol);
		assert(setupOK /*ensure setup is working*/);
		if(!setupOK) MOD_ABORT;
		pFF->Setup(mol);
		pFF->WeightedRotorSearch(conformers, geomSteps);
		pFF->ConjugateGradients(geomSteps); // final cleanup
		pFF->UpdateCoordinates(mol);
	}

	// calculate energy
	if(getConfig().obabel.verbose.get()) IO::log() << "\tenergy" << std::endl;
	double energy = pFF->Energy(false);

	// calculate appropriate unit scaling
	std::string unit = pFF->GetUnit();
	if(unit == "kcal/mol")
		return energy * 4.184; // convert kcal -> kJ
	else
		return energy;
}

void print2Dsvg(std::ostream &s, OpenBabel::OBMol &mol) {
	OpenBabel::OBConversion conv;
	conv.SetOutFormat("svg");
	conv.AddOption("svgwritechemobject"); // hack to make it not output fixed size svgs, is this really the best way?
	//	conv.AddOption("C"); // do not show terminal carbons
	//	conv.AddOption("i"); // put indices on
	conv.AddOption("h");
	conv.Write(&mol, &s);
	//	conv.SetOutFormat("smi");
	//	conv.Write(&mol, &IO::log());
}

double getOBCoordScaling(const OpenBabel::OBMol &obMol) {
	// this is an adaptation of the code from Open Babel (src/depict/depict.cpp)
	if(obMol.NumBonds() == 0) return 1;
	OpenBabel::OBMol &obMolNoConst = const_cast<OpenBabel::OBMol&> (obMol); // TODO: and Open Babel, please get your shit together!
	double sumLengths = 0;
	for(auto bIter = obMolNoConst.BeginBonds(); bIter != obMolNoConst.EndBonds(); ++bIter) sumLengths += (*bIter)->GetLength();
	return obMol.NumBonds() / sumLengths; // == 1 / (length / count);
}

double getOBAtomX(const OpenBabel::OBMol &obMol, unsigned int id) {
	auto *a = obMol.GetAtomById(id);
	assert(a);
	return a->GetX();
}

double getOBAtomY(const OpenBabel::OBMol &obMol, unsigned int id) {
	auto *a = obMol.GetAtomById(id);
	assert(a);
	return a->GetY();
}

} // namespace Chem
} // namespace lib
} // namespace mod

#endif // MOD_HAVE_OPENBABEL