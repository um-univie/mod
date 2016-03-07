#ifndef MOD_LIB_DG_STRATEGIES_ADD_H
#define	MOD_LIB_DG_STRATEGIES_ADD_H

#include <mod/lib/DG/Strategies/Strategy.h>

namespace mod {
template<typename Sig> class Function;
class Graph;
namespace lib {
namespace DG {
namespace Strategies {

struct Add : Strategy {
	Add(const std::vector<std::shared_ptr<mod::Graph> > graphs, bool onlyUniverse);
	Add(const std::shared_ptr<mod::Function<std::vector<std::shared_ptr<mod::Graph> >() > > generator, bool onlyUniverse);
	~Add();
	Strategy *clone() const;
	void printInfo(std::ostream &s) const;
	bool isConsumed(const lib::Graph::Single *g) const;
private:
	void executeImpl(std::ostream &s, const GraphState &input);
private:
	const std::vector<std::shared_ptr<mod::Graph> > graphs;
	const std::shared_ptr<mod::Function<std::vector<std::shared_ptr<mod::Graph> >() > > generator;
	const bool onlyUniverse;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_STRATEGIES_ADD_H */

