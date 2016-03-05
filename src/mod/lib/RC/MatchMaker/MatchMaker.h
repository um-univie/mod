#ifndef MOD_LIB_RC_MATCHMAKER_H
#define MOD_LIB_RC_MATCHMAKER_H

#include <mod/lib/Rule/Real.h>

#include <jla_boost/graph/morphism/VectorVertexMap.hpp>

namespace mod {
namespace lib {
namespace RC {

struct HaxMapWrapper {
	using GraphLeft = lib::Rule::GraphType;
	using GraphRight = GraphLeft;
	using Storable = std::false_type;

	using Vertex = lib::Rule::Vertex;
	using Edge = lib::Rule::Edge;

	HaxMapWrapper(const lib::Rule::Real::CoreCoreMap &map) : map(map) { }
public:
	const lib::Rule::Real::CoreCoreMap &map;
public:

	friend Vertex get(const HaxMapWrapper &m, const GraphRight &gRight, const GraphLeft &gLeft, Vertex vRight) {
		auto iter = m.map.right.find(vRight);
		if(iter == m.map.right.end()) return gLeft.null_vertex();
		else return iter->second;
	}

	friend Vertex get_inverse(const HaxMapWrapper &m, const GraphRight &gRight, const GraphLeft &gLeft, Vertex vLeft) {
		auto iter = m.map.left.find(vLeft);
		if(iter == m.map.left.end()) return gRight.null_vertex();
		else return iter->second;
	}
};

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_MATCHMAKER_H */
