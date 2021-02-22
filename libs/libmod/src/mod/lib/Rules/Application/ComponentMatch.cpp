#include <mod/lib/Rules/Application/ComponentMatch.hpp>

#include <iostream>

namespace mod::lib::Rules::Application {

/*
ComponentMatch::ComponentMatch(const Graph::LabelledGraph& pattern,
                               const Graph::LabelledGraph& host,
                               ComponentMatch::Morphism&& m):
    pattern(pattern), host(host), morphism(m) {}

ComponentMatch::GraphType::vertex_descriptor ComponentMatch::operator [] (ComponentMatch::GraphType::vertex_descriptor vp) const {
	const auto& gp = get_graph(pattern);
	const auto& gh = get_graph(host);
	auto vh = get(morphism, gp, gh, vp);
	assert(vh != boost::graph_traits<ComponentMatch::GraphType>::null_vertex());
	return vh;
}

std::ostream& operator << (std::ostream& os, const ComponentMatch& cm) {
	const auto& gp = get_graph(cm.pattern);
	const auto& gh = get_graph(cm.host);
	os << "ComponentMatch(";
	for (const auto vp : asRange(vertices(gp))) {
		auto vh = get(cm.morphism, gp, gh, vp);
		assert(vh != boost::graph_traits<ComponentMatch::GraphType>::null_vertex());
		os << vp << " -> " << cm[vp] << ", ";
	}
	os << ")";
	return os;
}
*/

}
