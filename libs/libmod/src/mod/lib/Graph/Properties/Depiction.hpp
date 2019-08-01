#ifndef MOD_LIB_GRAPH_DEPICTION_H
#define MOD_LIB_GRAPH_DEPICTION_H

#include <mod/BuildConfig.hpp>
#include <mod/lib/Chem/OBabel.hpp>
#include <mod/lib/Graph/LabelledGraph.hpp>

namespace mod {
struct AtomId;
struct Charge;
struct AtomData;
enum class BondType;
template<typename Sig> class Function;
namespace lib {
namespace IO {
namespace Graph {
namespace Write {
enum class EdgeFake3DType;
} // namespace Write
} // namespace Graph
} // namespace IO
namespace Graph {
struct PropMolecule;
struct PropString;

class DepictionData {
	DepictionData(const DepictionData&) = delete;
	DepictionData &operator=(const DepictionData&) = delete;
	DepictionData(DepictionData&&) = delete;
	DepictionData &operator=(DepictionData&&) = delete;
public:
	DepictionData(const LabelledGraph &lg);
	AtomId getAtomId(Vertex v) const; // shortcut to moleculeState
	Isotope getIsotope(Vertex v) const; // shortcut to moleculeState
	Charge getCharge(Vertex v) const; // shortcut to moleculeState
	bool getRadical(Vertex v) const; // shortcut to moleculeState
	std::string getVertexLabelNoIsotopeChargeRadical(Vertex v) const;
	BondType getBondData(Edge e) const; // shortcut to moleculeState
	bool hasImportantStereo(Vertex v) const;
	std::string getEdgeLabel(Edge e) const;
	const AtomData &operator()(Vertex v) const; // fake data
	BondType operator()(Edge e) const; // fake data
	bool getHasCoordinates() const;
	double getX(Vertex v, bool withHydrogen) const;
	double getY(Vertex v, bool withHydrogen) const;
	lib::IO::Graph::Write::EdgeFake3DType getEdgeFake3DType(Edge e, bool withHydrogen) const;
	std::string getRawStereoString(Vertex v) const;
	std::string getPrettyStereoString(Vertex v) const;
	std::string getStereoString(Edge e) const;
public: // custom depiction
	void setImage(std::shared_ptr<mod::Function<std::string()> > image);
	std::shared_ptr<mod::Function<std::string()> > getImage() const;
	void setImageCommand(std::string cmd);
	std::string getImageCommand() const;
private:
#ifdef MOD_HAVE_OPENBABEL
	const lib::Chem::OBMolHandle &getOB(bool withHydrogen) const;
#endif
private:
	const LabelledGraph &lg;
	bool hasMoleculeEncoding;
	std::map<Vertex, AtomData> nonAtomToPhonyAtom;
	std::map<AtomId, std::string> phonyAtomToStringNoStuff;
	std::map<Edge, std::string> nonBondEdges;
#ifdef MOD_HAVE_OPENBABEL
	mutable lib::Chem::OBMolHandle obMolAll, obMolNoHydrogen;
#endif
	std::shared_ptr<mod::Function<std::string()> > image;
	std::string imageCmd;
};

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_DEPICTION_H */
