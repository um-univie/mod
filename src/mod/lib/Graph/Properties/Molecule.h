#ifndef MOD_LIB_GRAPH_PROP_MOLECULE_H
#define	MOD_LIB_GRAPH_PROP_MOLECULE_H

#include <mod/BuildConfig.h>
#include <mod/Chem.h>
#include <mod/lib/Graph/Properties/Property.h>

#ifdef MOD_HAVE_OPENBABEL
#include <openbabel/mol.h>
#endif

#include <boost/optional.hpp>

namespace mod {
namespace lib {
namespace Graph {
struct PropString;

struct PropMolecule : Prop<PropMolecule, AtomData, BondType> {
	using Base = Prop<PropMolecule, AtomData, BondType>;
public:
	PropMolecule(const GraphType &g, const PropString &pString);
	bool getIsMolecule() const;
#ifdef MOD_HAVE_OPENBABEL
	OpenBabel::OBMol &getOBMol() const; // should return const, but OpenBabel sucks
#endif
	double getMolarMass() const;
	double getEnergy() const;
	void cacheEnergy(double value) const;
private:
	bool isMolecule;
#ifdef MOD_HAVE_OPENBABEL
	mutable std::unique_ptr<OpenBabel::OBMol> obMol;
#endif
	mutable boost::optional<double> energy;
};

} // namespace Graph
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_GRAPH_PROP_MOLECULE_H */
