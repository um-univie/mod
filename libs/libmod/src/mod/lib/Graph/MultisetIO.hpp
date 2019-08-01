#ifndef MOD_LIB_GRAPH_MULTISETIO_H
#define MOD_LIB_GRAPH_MULTISETIO_H

#include <mod/lib/Graph/Multiset.hpp>

#include <iostream>

namespace mod {
namespace lib {
namespace Graph {

template<typename GraphType>
std::ostream &operator<<(std::ostream &s, const Multiset<GraphType> &ms) {
	if(ms.size() == 1) return s << (*ms.begin())->getName();
	s << "{ ";
	for(const auto *g : ms) s << "'" << g->getName() << "' ";
	return s << "}";
}

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_MULTISETIO_H */