#ifndef MOD_LIB_CHEM_OBABEL_H
#define	MOD_LIB_CHEM_OBABEL_H

#include <mod/BuildConfig.h>
#include <mod/lib/Graph/Base.h>
#ifndef MOD_HAVE_OPENBABEL
#include <mod/lib/IO/IO.h>
#else
#include <mod/lib/Rule/LabelledRule.h>

#include <openbabel/mol.h>
#include <iosfwd>
#endif

namespace mod {
struct AtomData;
enum class BondType;
namespace lib {
namespace Graph {
class Single;
} // namespace Graph
namespace Chem {

#ifndef MOD_HAVE_OPENBABEL
#define MOD_NO_OPENBABEL_ERROR												\
	IO::log() << "Call to '" << __FUNCTION__ << "' failed." << std::endl;	\
	IO::log() << "Open Babel features are not available. Rebuild with Open Babel enabled." << std::endl;
#else
std::unique_ptr<OpenBabel::OBMol> copyOBMol(const OpenBabel::OBMol &mol);
std::unique_ptr<OpenBabel::OBMol> makeOBMol(const lib::Graph::GraphType &g,
		std::function<const AtomData &(lib::Graph::Vertex) > atomData,
		std::function<BondType(lib::Graph::Edge) > bondData,
		bool withHydrogen);
std::unique_ptr<OpenBabel::OBMol> makeOBMol(const lib::Rule::GraphType &g,
		std::function<const AtomData &(lib::Rule::Vertex) > atomData,
		std::function<BondType(lib::Rule::Edge) > bondData);
double getMolarMass(OpenBabel::OBMol &mol); // will truncate by *1024, cast to unsigned int, /1024
double getEnergy(OpenBabel::OBMol &mol);
void print2Dsvg(std::ostream &s, OpenBabel::OBMol &mol);

double getOBCoordScaling(const OpenBabel::OBMol &obMol);
double getOBAtomX(const OpenBabel::OBMol &obMol, unsigned int id);
double getOBAtomY(const OpenBabel::OBMol &obMol, unsigned int id);
#endif

} // namespace Chem
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_CHEM_OBABEL_H */
