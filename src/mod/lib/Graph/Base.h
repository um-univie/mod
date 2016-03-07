#ifndef MOD_LIB_GRAPH_BASE_H
#define MOD_LIB_GRAPH_BASE_H

#include <mod/lib/Graph/LabelledGraph.h>

#include <iosfwd>
#include <set>

namespace mod {
namespace lib {
namespace Graph {
struct Single;

struct Less { // implemented in Single.cpp
	bool operator()(const Single *g1, const Single *g2) const;
};

struct Base {

	virtual ~Base() { }
	virtual void printName(std::ostream &s) const = 0;
	virtual const GraphType &getGraph() const = 0;

	inline const std::multiset<const Single*, Less> &getSingles() const {
		return singles;
	}
protected:
	std::multiset<const Single*, Less> singles;
};

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_BASE_H */
