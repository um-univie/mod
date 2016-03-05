#include "SBML.h"

#include <mod/BuildConfig.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/DG/Matrix.h>

#ifdef MOD_HAVE_SBML
// fix bug in build of libSBML
// TODO: remove bug fix, when libSBML is fixed
#define LIBSBML_USE_CPP_NAMESPACE
#include <sbml/SBMLDocument.h>
#include <sbml/Model.h>
#include <sbml/Compartment.h>
#endif

#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace Chem {

#define NO_LIBSBML_ERROR													\
	IO::log() << "Call to '" << __FUNCTION__ << "' failed." << std::endl;	\
	IO::log() << "libSBML features are not available. Rebuild with libSBML enabled." << std::endl;	\
	std::exit(1);

DG::Matrix *loadNetworkFromSBML(const std::string& fileName) {
#ifndef MOD_HAVE_SBML
	NO_LIBSBML_ERROR
#else
	unsigned int numReversible = 0;
	using namespace libsbml;
	DG::Matrix *matrix = new DG::Matrix();
	libsbml::SBMLReader reader;
	const SBMLDocument *doc = reader.readSBML(fileName);
	if(doc->getNumErrors() != 0) {
		IO::log() << "SBML document contains errors." << std::endl;
		exit(1);
	}
	const Model *model = doc->getModel();

	const ListOfSpecies *specs = model->getListOfSpecies();
	std::map<std::string, unsigned int> specMap;
	for(unsigned int i = 0; i < specs->size(); i++) {
		const Species *spec = specs->get(i);
		//		IO::log() << "Spec:\t" << spec->getId() << "\t:\t" << spec->getName() << std::endl;
		specMap.insert(std::make_pair(spec->getId(), i));
		unsigned int id = matrix->addRow(spec->getId());
		if(spec->getCompartment() == "C_e")
			matrix->getInputIds().push_back(id);
	}

	const ListOfReactions *reacs = model->getListOfReactions();
	for(unsigned int i = 0; i < reacs->size(); i++) {
		const Reaction *reac = reacs->get(i);
		//		IO::log() << "Reac:\t" << reac->getId() << "\t:\t" << reac->getName() << std::endl;
		unsigned int col = matrix->addCol(reac->getId());
		bool haveFloats = false;
		for(unsigned int j = 0; j < reac->getNumReactants(); j++) {
			std::map<std::string, unsigned int>::const_iterator iter = specMap.find(reac->getReactant(j)->getSpecies());
			if(iter == specMap.end()) {
				IO::log() << "Reaction " << j << ", " << reac->getId() << ": have an unknown "
						<< "reactant, " << reac->getReactant(j)->getId() << std::endl;
				exit(1);
			}
			unsigned int row = iter->second;
			double floatCoef = reac->getReactant(j)->getStoichiometry();
			int coef = floatCoef;
			if(round(floatCoef) != floatCoef) haveFloats = true;
			matrix->addCoef(col, row, -coef);
		}
		for(unsigned int j = 0; j < reac->getNumProducts(); j++) {
			std::map<std::string, unsigned int>::const_iterator iter = specMap.find(reac->getProduct(j)->getSpecies());
			if(iter == specMap.end()) {
				IO::log() << "Reaction " << j << ", " << reac->getId() << ": have an unknown "
						<< "product, " << reac->getReactant(j)->getId() << std::endl;
				exit(1);
			}
			unsigned int row = iter->second;
			double floatCoef = reac->getProduct(j)->getStoichiometry();
			int coef = floatCoef;
			if(round(floatCoef) != floatCoef) haveFloats = true;
			matrix->addCoef(col, row, coef);
		}
		if(haveFloats) {
			IO::log() << "Reaction " << i << ", " << reac->getId() << ": have something not integer as stoichiometry. Skipping." << std::endl;
			matrix->getCols().pop_back();
			matrix->getColNames().pop_back(); // names should be unique, so this is ok
			continue;
		}

		//		unsigned int limit = 4;
		//		if(reac->getNumReactants() >= limit || reac->getNumProducts() >= limit) {
		//			IO::log() << "Reaction " << i << ", " << reac->getId() << ": have many" << std::endl;
		//		}
		if(reac->getReversible()) {
			numReversible++;
			//			IO::log() << "Reversible:\t" << reac->getId() << std::endl;
			std::string colName = boost::lexical_cast<std::string > (reac->getId()) + "_reverse";
			unsigned int revCol = matrix->addCol(colName);
			typedef std::pair<unsigned int, int> P;
			for(const P &coef : matrix->getCols()[col].getCoefs()) {
				matrix->addCoef(revCol, coef.first, -coef.second);
			}
		}
	}

	IO::log() << "SBML\t#species\t" << matrix->getRowNames().size() << std::endl;
	IO::log() << "SBML\t#external\t" << matrix->getInputIds().size() << std::endl;
	IO::log() << "SBML\t#reactions\t" << matrix->getCols().size() << std::endl;
	IO::log() << "SBML\t#reversible\t" << numReversible << std::endl;

	delete doc;
	return matrix;
#endif
}

} // namespace Chem
} // namespace lib
} // namespace mod