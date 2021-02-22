#ifndef MOD_LIB_RULES_APPLICATION_COMPONENTMATCH_HPP
#define MOD_LIB_RULES_APPLICATION_COMPONENTMATCH_HPP

#include <jla_boost/graph/morphism/models/Vector.hpp>
#include <mod/lib/Graph/LabelledGraph.hpp>
#include <iosfwd>

namespace mod::lib::Rules::Application {


class ComponentMatch {
public:
	using GraphType = Graph::LabelledGraph::GraphType;
	using Morphism = jla_boost::GraphMorphism::VectorVertexMap<GraphType, GraphType>;

	ComponentMatch(size_t componentIndex, size_t hostIndex, const Morphism& m):
	    componentIndex(componentIndex), hostIndex(hostIndex), morphism(m), graphInstance(0) {}

	size_t componentIndex, hostIndex;
	const Morphism& morphism;
	int graphInstance;
};

};

#endif
