#ifndef MOD_LIB_GRAPH_DEPICTION_H
#define MOD_LIB_GRAPH_DEPICTION_H

#include <mod/BuildConfig.h>
#include <mod/lib/Graph/Base.h>
#include <mod/lib/Graph/Properties/Molecule.h>

#ifdef MOD_HAVE_OPENBABEL
#include <openbabel/mol.h>
#endif

namespace mod {
struct AtomId;
struct Charge;
struct AtomData;
enum class BondType;
template<typename Sig> class Function;
namespace lib {
namespace Graph {

class DepictionData {
	DepictionData(const DepictionData&) = delete;
	DepictionData &operator=(const DepictionData&) = delete;
	DepictionData(DepictionData&&) = delete;
	DepictionData &operator=(DepictionData&&) = delete;
public:
	DepictionData(const GraphType &g, const PropStringType &pString, const PropMolecule &moleculeState);
	AtomId getAtomId(Vertex v) const; // shortcut to moleculeState
	Charge getCharge(Vertex v) const; // shortcut to moleculeState
	std::string getVertexLabelNoCharge(Vertex v) const;
	BondType getBondData(Edge e) const; // shortcut to moleculeState
	std::string getEdgeLabel(Edge e) const;
	const AtomData &operator()(Vertex v) const; // fake data
	BondType operator()(Edge e) const; // fake data
	bool getHasCoordinates() const;
	double getX(Vertex v, bool withHydrogen) const;
	double getY(Vertex v, bool withHydrogen) const;
public: // custom depiction
	void setImage(std::shared_ptr<mod::Function<std::string()> > image);
	std::shared_ptr<mod::Function<std::string()> > getImage() const;
	void setImageCommand(std::string cmd);
	std::string getImageCommand() const;
private:
	const GraphType &g;
	const PropMolecule &moleculeState;
	bool hasMoleculeEncoding;
	std::map<Vertex, AtomData> nonAtomToPhonyAtom;
	std::map<AtomId, std::string> phonyAtomToStringNoCharge;
	std::map<Edge, std::string> nonBondEdges;
#ifdef MOD_HAVE_OPENBABEL
	std::unique_ptr<OpenBabel::OBMol> obMolAll, obMolNoHydrogen;
#endif
	std::shared_ptr<mod::Function<std::string()> > image;
	std::string imageCmd;
};

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_DEPICTION_H */
