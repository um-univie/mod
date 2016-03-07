#ifndef MOD_LIB_RULE_PROP_MOLECULE_H
#define	MOD_LIB_RULE_PROP_MOLECULE_H

#include <mod/lib/Rule/GraphDecl.h>
#include <mod/lib/Rule/Properties/Property.h>

namespace mod {
struct AtomData;
enum class BondType;
namespace lib {
namespace Rule {
struct PropStringCore;

class PropMoleculeCore : private PropCore<PropMoleculeCore, GraphType, AtomData, BondType> {
	// read-only of data
	using Base = PropCore<PropMoleculeCore, GraphType, AtomData, BondType>;
public:
	PropMoleculeCore(const GraphType &g, const PropStringCore &labelState);
	using Base::getLeft;
	using Base::getRight;
private:
	bool isReaction;
};

} // namespace Rule
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_RULE_STATE_MOLECULE_H */