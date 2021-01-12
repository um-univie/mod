#ifndef MOD_LIB_CHEM_OBABEL_HPP
#define MOD_LIB_CHEM_OBABEL_HPP

#include <mod/BuildConfig.hpp>
#include <mod/lib/Graph/GraphDecl.hpp>
#ifndef MOD_HAVE_OPENBABEL
#include <mod/lib/IO/IO.hpp>
#else
#include <mod/lib/Rules/LabelledRule.hpp>

#include <iosfwd>
#endif

namespace mod {
struct AtomData;
enum class BondType;
} // namespace mod
namespace mod::lib::IO::Graph::Write {
enum class EdgeFake3DType;
} // namespace mod::lib::IO::Graph::Write
namespace mod::lib::Graph {
struct PropStereo;
struct Single;
} // namespace mod::lib::Graph
namespace mod::lib::Rules {
struct PropStereoCore;
} // namespace mod::lib::Rules
namespace mod::lib::Chem {

#ifndef MOD_HAVE_OPENBABEL
#define MOD_NO_OPENBABEL_ERROR            \
	std::cout << "Call to '" << __FUNCTION__ << "' failed." << std::endl; \
	std::cout << "Open Babel features are not available. Rebuild with Open Babel enabled." << std::endl;
#else

struct OBMolHandle {
	struct Pimpl;
public:
	OBMolHandle();
	OBMolHandle(OBMolHandle&&);
	OBMolHandle &operator=(OBMolHandle&&);
	OBMolHandle(std::unique_ptr<Pimpl> p);
	~OBMolHandle();
	explicit operator bool() const;
	void setCoordinates(const std::vector<double> &x, const std::vector<double> &y);
public:
	double getMolarMass() const; // will truncate by *1024, cast to unsigned int, /1024
	double getEnergy() const;
	void print2Dsvg(std::ostream &s) const;
public:
	double getCoordScaling() const;
	bool hasAtom(unsigned int id) const;
	double getAtomX(unsigned int id) const;
	double getAtomY(unsigned int id) const;
	lib::IO::Graph::Write::EdgeFake3DType getBondFake3D(unsigned int idSrc, unsigned int idTar) const;
public:
	std::unique_ptr<Pimpl> p;
};

OBMolHandle copyOBMol(const OBMolHandle &mol);
OBMolHandle makeOBMol(const lib::Graph::GraphType &g,
		std::function<const AtomData &(lib::Graph::Vertex) > atomData,
		std::function<BondType(lib::Graph::Edge) > bondData,
		std::function<bool(lib::Graph::Vertex) > hasImportantStereo,
		bool withHydrogen, const lib::Graph::PropStereo *pStereo);
std::tuple<OBMolHandle, OBMolHandle, OBMolHandle> makeOBMol(const lib::Rules::LabelledRule &lr,
		std::function<const AtomData &(lib::Rules::Vertex) > atomData,
		std::function<BondType(lib::Rules::Edge) > bondData,
		std::function<const AtomData &(lib::Rules::Vertex) > atomDataLeft,
		std::function<BondType(lib::Rules::Edge) > bondDataLeft,
		std::function<const AtomData &(lib::Rules::Vertex) > atomDataRight,
		std::function<BondType(lib::Rules::Edge) > bondDataRight,
		const bool withHydrogen);
#endif

} // namespace mod::lib::Chem

#endif // MOD_LIB_CHEM_OBABEL_HPP