#ifndef MOD_LIB_RULES_STATE_DEPICTION_H
#define MOD_LIB_RULES_STATE_DEPICTION_H

#include <mod/BuildConfig.h>
#include <mod/lib/Rules/GraphDecl.h>

#ifdef MOD_HAVE_OPENBABEL
#include <openbabel/mol.h>
#endif

#include <map>
#include <vector>

namespace mod {
struct AtomId;
struct Charge;
struct AtomData;
enum class BondType;
namespace lib {
namespace Rules {
struct PropStringCore;
struct PropMoleculeCore;

struct DepictionDataCore {

	template<Membership membership>
	struct DepictionData { // instantiated in the cpp file
		DepictionData(const DepictionDataCore &depict);
		AtomId getAtomId(Vertex v) const; // shortcut to moleculeState
		Charge getCharge(Vertex v) const; // shortcut to moleculeState
		bool getRadical(Vertex v) const; // shortcut to moleculeState
		BondType getBondData(Edge e) const; // shortcut to moleculeState
		std::string getVertexLabelNoChargeRadical(Vertex v) const;
		std::string getEdgeLabel(Edge e) const;
		const AtomData &operator()(Vertex v) const; // fake data
		BondType operator()(Edge e) const; // fake data
		bool getHasCoordinates() const;
		double getX(Vertex v, bool withHydrogen) const;
		double getY(Vertex v, bool withHydrogen) const;
		//	private:
		//		bool isAtomIdInvalidContext(Vertex v) const;
	private:
		const DepictionDataCore &depict;
	};
public:
	DepictionDataCore(const GraphType &g, const PropStringCore &labelState, const PropMoleculeCore &moleculeState);
	DepictionDataCore(const DepictionDataCore&) = delete;
	DepictionDataCore &operator=(const DepictionDataCore&) = delete;
	const AtomData &operator()(Vertex v) const; // fake data, for creating OBMol
	BondType operator()(Edge e) const; // fake data, for creating OBMol
	bool getHasCoordinates() const;
	double getX(Vertex v) const;
	double getY(Vertex v) const;
	void copyCoords(const DepictionDataCore &other, const std::map<Vertex, Vertex> &vMap);
public: // projections
	DepictionData<Membership::Left> getLeft() const;
	DepictionData<Membership::Context> getContext() const;
	DepictionData<Membership::Right> getRight() const;
private:
	const GraphType &g;
	const PropMoleculeCore &moleculeState;
	bool hasMoleculeEncoding, hasCoordinates;
	std::map<Vertex, AtomData> nonAtomToPhonyAtomLeft, nonAtomToPhonyAtomRight;
	std::map<AtomId, std::string> phonyAtomToString;
	std::map<Edge, std::string> nonBondEdgesLeft, nonBondEdgesRight;
	std::vector<double> x, y;
#ifdef MOD_HAVE_OPENBABEL
	std::unique_ptr<OpenBabel::OBMol> obMol;
#endif
};

} // namespace Rules
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RULES_STATE_DEPICTION_H */