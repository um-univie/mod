#ifndef MOD_LIB_RULES_PROP_MOLECULE_H
#define	MOD_LIB_RULES_PROP_MOLECULE_H

#include <mod/lib/Chem/MoleculeUtil.hpp>
#include <mod/lib/Rules/GraphDecl.hpp>
#include <mod/lib/Rules/Properties/Property.hpp>

namespace mod {
struct AtomData;
enum class BondType;
namespace lib {
namespace Rules {
struct PropStringCore;

class PropMoleculeCore : private PropCore<PropMoleculeCore, GraphType, AtomData, BondType> {
	// read-only of data
	using Base = PropCore<PropMoleculeCore, GraphType, AtomData, BondType>;
public:
	using Base::LeftType;
	using Base::RightType;
public:
	PropMoleculeCore(const GraphType &g, const PropStringCore &labelState);
	using Base::invert;
	using Base::getLeft;
	using Base::getRight;
private:
	bool isReaction;
};

} // namespace Rules
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RULES_STATE_MOLECULE_H */
