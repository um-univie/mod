#ifndef MOD_LIB_DG_NONHYPERDERIVATIONS_H
#define	MOD_LIB_DG_NONHYPERDERIVATIONS_H

#include <mod/lib/DG/NonHyper.h>

namespace mod {
struct Derivation;
namespace lib {
namespace DG {

struct NonHyperDerivations : NonHyper {
	NonHyperDerivations(std::vector<Derivation> derivations);
	virtual std::string getType() const override;
private:
	virtual void calculateImpl(bool printInfo) override;
	virtual void listImpl(std::ostream &s) const override;
private:
	std::unique_ptr<const std::vector<Derivation> > derivations;
};

} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_NONHYPERDERIVATIONS_H */