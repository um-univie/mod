#ifndef MOD_LIB_IO_RC_H
#define MOD_LIB_IO_RC_H

#include <mod/lib/Rules/Real.h>

#include <boost/bimap.hpp>

#include <string>

namespace mod {
namespace lib {
namespace RC {
class Evaluator;
} // namespace RC
namespace IO {
namespace RC {
namespace Read {
} // namespace Read
namespace Write {
std::string dot(const lib::RC::Evaluator &rc);
std::string svg(const lib::RC::Evaluator &rc);
std::string pdf(const lib::RC::Evaluator &rc);

using CoreCoreMap = boost::bimap<lib::Rules::Vertex, lib::Rules::Vertex>;

void test(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, const CoreCoreMap &match, const lib::Rules::Real &rNew);

template<typename VertexMap>
void test(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, const VertexMap &map, const lib::Rules::Real &rNew) {
	CoreCoreMap match;
	for(auto vFirst : asRange(vertices(rFirst.getGraph()))) {
		auto vSecond = get_inverse(map, rSecond.getGraph(), rFirst.getGraph(), vFirst);
		if(vSecond == boost::graph_traits<lib::Rules::GraphType>::null_vertex()) continue;
		match.insert(CoreCoreMap::relation(vFirst, vSecond));
	}
	test(rFirst, rSecond, match, rNew);
}

} // namespace Write
} // namespace RC
} // namespace IO
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_RC_H */