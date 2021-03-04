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
#include <mod/lib/Rules/Application/ValidDPO.hpp>
#include <mod/lib/Rules/Application/CanonMatch.hpp>

namespace mod::lib::Rules::Application::ComponentMatchDB {

bool isMonomorphic(const Real& real, size_t componentIndex,
                   const Graph::LabelledGraph& host,
                   const LabelSettings& ls) {
	namespace GM = jla_boost::GraphMorphism;
	namespace GM_MOD = lib::GraphMorphism;
	const auto& rule = real.getDPORule();

	const Graph::LabelledGraph& pattern = get_component_graph_v2(componentIndex, get_labelled_left(rule));

	auto mr = GM::makeLimit(1);
	lib::GraphMorphism::morphismSelectByLabelSettings(pattern, host,
	                                                  ls, GM_MOD::VF2Monomorphism(),
	                                                  std::ref(mr));
	return mr.getNumHits() > 0;
}

template<typename Next>
struct MatchChecker {
	MatchChecker(const Real& rule, size_t compIndex,
	             const LabelledRule::ComponentGraph_v2& compGraph,
	             const Graph::LabelledGraph& host,
	             Next next):
	    real(rule), componentIndex(compIndex), compGraph(compGraph), host(host), next(next) {}

	template<typename Morphism, typename GraphDom, typename GraphCodom>
	bool operator()(Morphism &&m, const GraphDom &gDom, const GraphCodom &gCodom) const {
		//std::cout << "CHECKING HERE " << std::endl;
		assert(&gDom == &get_graph(compGraph));
		assert(&gCodom == &get_graph(host));

		if (!isValidDPOMatch(real.getDPORule(), compGraph, componentIndex, host, m)) {
			return true;
		}
//		if (!isCanonComponentMatch(real, componentIndex, host, m)) {
//			return true;
//		}
		return next(std::forward<Morphism>(m), gDom, gCodom);
	}
private:
	const Real& real;
	size_t componentIndex;
	const LabelledRule::ComponentGraph_v2& compGraph;
	const Graph::LabelledGraph& host;
	Next next;
};

template<typename Next>
MatchChecker<Next> makeMatchChecker(const Real& rule, size_t compIndex,
             const LabelledRule::ComponentGraph_v2& compGraph,
                 const Graph::LabelledGraph& host,
                 Next next) {
	    return MatchChecker<Next>(rule, compIndex, compGraph, host, next);


}

std::vector<ComponentMatch::Morphism> enumerateMonomorphisms(const Real& real,
                                                          size_t componentIndex,
                                                          const Graph::LabelledGraph& host,
                                                          const LabelSettings& ls) {
	namespace GM = jla_boost::GraphMorphism;
	namespace GM_MOD = lib::GraphMorphism;
	const auto& rule = real.getDPORule();

	std::vector<ComponentMatch::Morphism> morphisms;

	const Graph::LabelledGraph& pattern = get_component_graph_v2(componentIndex, get_labelled_left(rule));

	auto mr = GM::makeTransform(GM::ToVectorVertexMap(),
	                            makeMatchChecker(
	                                real, componentIndex, pattern, host,
	                                GM::makeSliceProps(
	                                    GM::makeStore(std::back_inserter(morphisms))
	                                )
	                                )
	                            );
	lib::GraphMorphism::morphismSelectByLabelSettings(pattern, host,
	                                                  ls, GM_MOD::VF2Monomorphism(),
	                                                  std::ref(mr));
	// should really be in the matchChecker... but I don't want to use a template for this..
	if (getConfig().application.useCanonicalMatches.get()) {
		if (getConfig().application.useCanonicalMatchesExhaustive.get()) {
			morphisms.erase(std::remove_if(morphisms.begin(),
			                               morphisms.end(),
			                               [&](const ComponentMatch::Morphism& m){
				return !isCanonComponentMatch(real, componentIndex, host, m);
			}),
			                morphisms.end());

		} else {
			morphisms.erase(std::remove_if(morphisms.begin(),
			                               morphisms.end(),
			                               [&](const ComponentMatch::Morphism& m){
				return !isCanonComponentMatchFast(real, componentIndex, host, m);
			}),
			                morphisms.end());
		}
	}
	//std::cout << "FOUND " << morphisms.size() << std::endl;;
	return morphisms;
}

}
