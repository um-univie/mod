#ifndef MOD_LIB_DG_NONHYPERDERIVATIONS_H
#define	MOD_LIB_DG_NONHYPERDERIVATIONS_H

#include <mod/lib/DG/NonHyper.h>

namespace mod {
struct Derivation;
namespace lib {
namespace DG {

struct NonHyperDerivations : NonHyper {
	NonHyperDerivations(std::vector<Derivation> derivations);
	std::string getType() const;
private:
	void calculateImpl();
	void listImpl(std::ostream &s) const;
private:
	std::unique_ptr<const std::vector<Derivation> > derivations;
};

} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_NONHYPERDERIVATIONS_H */