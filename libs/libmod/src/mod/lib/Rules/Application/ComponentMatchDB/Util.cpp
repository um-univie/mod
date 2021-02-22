#include <mod/lib/Rules/Application/ComponentMatchDB/Util.hpp>
#include <mod/lib/Rules/Real.hpp>
#include<mod/lib/Graph/Properties/String.hpp>
#include<mod/lib/Graph/Properties/Term.hpp>
#include<mod/lib/Graph/Properties/Stereo.hpp>
#include <mod/lib/GraphMorphism/LabelledMorphism.hpp>
#include <mod/lib/GraphMorphism/VF2Finder.hpp>
#include <jla_boost/graph/morphism/callbacks/Limit.hpp>
#include <jla_boost/graph/morphism/callbacks/Store.hpp>
#include <jla_boost/graph/morphism/callbacks/SliceProps.hpp>
#include <jla_boost/graph/morphism/callbacks/Transform.hpp>
#include <mod/lib/GraphMorphism/Constraints/CheckVisitor.hpp>

namespace mod::lib::Rules::Application::ComponentMatchDB {

bool isMonomorphic(const LabelledRule& rule, size_t componentIndex,
                   const Graph::LabelledGraph& host,
                   const LabelSettings& ls) {
	namespace GM = jla_boost::GraphMorphism;
	namespace GM_MOD = lib::GraphMorphism;

	const Graph::LabelledGraph& pattern = get_component_graph_v2(componentIndex, get_labelled_left(rule));

	auto mr = GM::makeLimit(1);
	lib::GraphMorphism::morphismSelectByLabelSettings(pattern, host,
	                                                  ls, GM_MOD::VF2Monomorphism(),
	                                                  std::ref(mr));
	return mr.getNumHits() > 0;
}

std::vector<ComponentMatch::Morphism> enumerateMonomorphisms(const LabelledRule& rule,
                                                          size_t componentIndex,
                                                          const Graph::LabelledGraph& host,
                                                          const LabelSettings& ls) {
	namespace GM = jla_boost::GraphMorphism;
	namespace GM_MOD = lib::GraphMorphism;

	std::vector<ComponentMatch::Morphism> morphisms;

	const Graph::LabelledGraph& pattern = get_component_graph_v2(componentIndex, get_labelled_left(rule));
	auto mr = GM::makeTransform(GM::ToVectorVertexMap(),
	                                GM::makeSliceProps(
	                                    GM::makeStore(std::back_inserter(morphisms))
	                                )
	                            );
	lib::GraphMorphism::morphismSelectByLabelSettings(pattern, host,
	                                                  ls, GM_MOD::VF2Monomorphism(),
	                                                  std::ref(mr));
	return morphisms;
}

}
