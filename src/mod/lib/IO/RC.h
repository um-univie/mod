#ifndef MOD_LIB_IO_RC_H
#define MOD_LIB_IO_RC_H

#include <mod/lib/Rule/Real.h>

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

void test(const lib::Rule::Real &rFirst, const lib::Rule::Real &rSecond, const lib::Rule::Real::CoreCoreMap &match, const lib::Rule::Real &rNew);

template<typename VertexMap>
void test(const lib::Rule::Real &rFirst, const lib::Rule::Real &rSecond, const VertexMap &map, const lib::Rule::Real &rNew) {
	lib::Rule::Real::CoreCoreMap match;
	for(auto vFirst : asRange(vertices(rFirst.getGraph()))) {
		auto vSecond = get_inverse(map, rSecond.getGraph(), rFirst.getGraph(), vFirst);
		if(vSecond == boost::graph_traits<lib::Rule::GraphType>::null_vertex()) continue;
		match.insert(lib::Rule::Real::CoreCoreMap::relation(vFirst, vSecond));
	}
	test(rFirst, rSecond, match, rNew);
}
} // namespace Write
} // namespace RC
} // namespace IO
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_RC_H */