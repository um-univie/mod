#ifndef MOD_LIB_RULES_APPLICATION_COMPONENTMATCH_HPP
#define MOD_LIB_RULES_APPLICATION_COMPONENTMATCH_HPP

#include <jla_boost/graph/morphism/models/Vector.hpp>
#include <mod/lib/Graph/LabelledGraph.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <iosfwd>

namespace mod::lib::Rules::Application {


class ComponentMatch {
public:
	using GraphType = Graph::LabelledGraph::GraphType;
	using Morphism = jla_boost::GraphMorphism::VectorVertexMap<GraphType, GraphType>;

	ComponentMatch() {}
	ComponentMatch(size_t componentIndex, bool isSubsetHost, const Graph::Single* host, const Morphism& m):
	    componentIndex(componentIndex), isSubsetHost(isSubsetHost), host(host), morphism(&m), graphInstance(0) {}

	size_t componentIndex;
	bool isSubsetHost;
	const Graph::Single* host;
	const Morphism* morphism;
	int graphInstance;
};

};

#endif
