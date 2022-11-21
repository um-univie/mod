#ifndef MOD_LIB_GRAPHMORPHISM_STEREOVERTEXMAP_H
#define MOD_LIB_GRAPHMORPHISM_STEREOVERTEXMAP_H

#include <mod/Error.hpp>
#include <mod/lib/Stereo/Configuration/Configuration.hpp>
#include <mod/lib/Stereo/EdgeCategory.hpp>

#include <jla_boost/graph/morphism/Concepts.hpp>
#include <jla_boost/graph/morphism/models/PropertyVertexMap.hpp>

// LocalIso
// LocalSpec
// PredIso
// PredSpec
// ToVertexMap
// - Iso
// - Spec

namespace mod {
namespace lib {
namespace GraphMorphism {

struct StereoDataT {
};

namespace Stereo {

using ConfPtr = std::unique_ptr<const lib::Stereo::Configuration>;
using EdgeCategory = lib::Stereo::EdgeCategory;
using EmbeddingEdge = lib::Stereo::EmbeddingEdge;

struct LocalIso {

	template<typename ...Args>
	bool operator()(const ConfPtr &cDom, const ConfPtr &cCodom, Args &&... args) const {
		if(cDom->getGeometryVertex() != cCodom->getGeometryVertex()) return false;
		if(cDom->getNumLonePairs() != cCodom->getNumLonePairs()) return false;
		if(cDom->getHasRadical() != cCodom->getHasRadical()) return false;
		return cDom->localPredIso(*cCodom);
	}

	template<typename ...Args>
	bool operator()(const EdgeCategory catDom, const EdgeCategory catCodom, Args &&... args) const {
		return catDom == catCodom;
	}

	template<typename ...Args>
	bool operator()(const bool inContextDom, const bool inContextCodom, Args &&... args) const {
		return inContextDom == inContextCodom;
	}
};

struct LocalSpec {

	template<typename ...Args>
	bool operator()(const ConfPtr &cDom, const ConfPtr &cCodom, Args &&... args) const {
		bool isAncestor = lib::Stereo::getGeometryGraph().isAncestorOf(cDom->getGeometryVertex(),
																							cCodom->getGeometryVertex());
		if(!isAncestor) return false;
		// TODO: check #lone pairs and #radicals
		// if they are different, we can't do any further checking
		if(cDom->getGeometryVertex() != cCodom->getGeometryVertex()) return true;
		return cDom->localPredSpec(*cCodom);
	}

	template<typename ...Args>
	bool operator()(const EdgeCategory catDom, const EdgeCategory catCodom, Args &&... args) const {
		return catDom == catCodom || catDom == EdgeCategory::Any;
	}

	template<typename ...Args>
	bool operator()(const bool inContextDom, const bool inContextCodom, Args &&... args) const {
		return inContextCodom || !inContextDom;
	}
};

// PredIso
//------------------------------------------------------------------------------

template<typename Next>
struct PredIso {

	PredIso(Next next) : next(next) {}

	template<typename VEDom, typename VECodom, typename LabGraphDom, typename LabGraphCodom>
	bool operator()(const VEDom &veDom,
						 const VECodom &veCodom,
						 const LabGraphDom &gDom,
						 const LabGraphCodom &gCodom) const {
		const auto &pDom = get_stereo(gDom);
		const auto &pCodom = get_stereo(gCodom);
		using Handler = typename LabGraphDom::PropStereoType::Handler;
		const bool res = Handler::reduce(std::logical_and<>(),
													Handler::fmap2(get(pDom, veDom), get(pCodom, veCodom), gDom, gCodom,
																		LocalIso()));
		return res && next(veDom, veCodom, gDom, gCodom);
	}

private:
	Next next;
};

template<typename Next = jla_boost::AlwaysTrue>
auto makePredIso(Next next = jla_boost::AlwaysTrue()) {
	return PredIso<Next>(next);
}


// PredSpec
//------------------------------------------------------------------------------

template<typename Next>
struct PredSpec {

	PredSpec(Next next) : next(next) {}

	template<typename VEDom, typename VECodom, typename LabGraphDom, typename LabGraphCodom>
	bool operator()(const VEDom &veDom,
						 const VECodom &veCodom,
						 const LabGraphDom &gDom,
						 const LabGraphCodom &gCodom) const {
		using Handler = typename LabGraphDom::PropStereoType::Handler;
		const auto &pDom = get_stereo(gDom);
		const auto &pCodom = get_stereo(gCodom);
		const bool res = Handler::reduce(std::logical_and<>(),
													Handler::fmap2(get(pDom, veDom), get(pCodom, veCodom), gDom, gCodom,
																		LocalSpec()));
		return res && next(veDom, veCodom, gDom, gCodom);
	}

private:
	Next next;
};

template<typename Next = jla_boost::AlwaysTrue>
auto makePredSpec(Next next = jla_boost::AlwaysTrue()) {
	return PredSpec<Next>(next);
}


// ToVertexMap
//------------------------------------------------------------------------------

namespace GM = jla_boost::GraphMorphism;

struct StereoDataIso {
};

struct StereoDataSpec {
	// The codomain has the data.

	template<typename LabGraphLeft, typename LabGraphRight, typename VertexRight>
	friend lib::Stereo::GeometryGraph::Vertex
	get_geometry(const StereoDataSpec &,
					 const LabGraphLeft &lgLeft,
					 const LabGraphRight &lgRight,
					 const VertexRight &vRight) {
		return get_stereo(lgRight)[vRight]->getGeometryVertex();
	}

	template<typename LabGraphLeft, typename LabGraphRight, typename EdgeRight>
	friend lib::Stereo::EdgeCategory
	get_edge_category(const StereoDataSpec &,
							const LabGraphLeft &lgLeft,
							const LabGraphRight &lgRight,
							const EdgeRight &eRight) {
		return get_stereo(lgRight)[eRight];
	}
};

template<typename Derived, typename LabGraphDom, typename LabGraphCodom, typename Next>
struct ToVertexMapBase {
	using Handler = typename LabGraphDom::PropStereoType::Handler;
public:

	ToVertexMapBase(const LabGraphDom &lgDom, const LabGraphCodom &lgCodom, Next next)
			: lgDom(lgDom), lgCodom(lgCodom), next(next) {}

	template<typename VertexMap, typename GraphMorDom, typename GraphMorCodom>
	bool operator()(VertexMap &&m, const GraphMorDom &gMorDom, const GraphMorCodom &gMorCodom) const {
		BOOST_CONCEPT_ASSERT((GM::VertexMapConcept<VertexMap>));
		// Note: GraphMorDom is not necessarily the same as LabGraphDom::GraphType
		//       as LabGraphDom can be a reindexed labelled filtered graph,
		//       and GraphMorDom is the plain filtered graph without reindexing.
		//       Therefore, do not use get_graph(lgDom) and get_graph(lgCodom)!
		const auto &pDom = get_stereo(lgDom);
		const auto &pCodom = get_stereo(lgCodom);
		// Instantiate the edge categories
		for(const auto eDom : asRange(edges(gMorDom))) {
			const auto vDomSrc = source(eDom, gMorDom);
			const auto vDomTar = target(eDom, gMorDom);
			const auto vCodomSrc = get(m, gMorDom, gMorCodom, vDomSrc);
			const auto vCodomTar = get(m, gMorDom, gMorCodom, vDomTar);
			if(vCodomSrc == boost::graph_traits<GraphMorCodom>::null_vertex()) continue;
			if(vCodomTar == boost::graph_traits<GraphMorCodom>::null_vertex()) continue;
			const auto peCodom = edge(vCodomSrc, vCodomTar, gMorCodom);
			if(!peCodom.second) continue;
			const auto eCodom = peCodom.first;
			const bool ok = getDerived().unifyEdges(eDom, eCodom, pDom, pCodom);
			if(!ok) return true;
		}
		// Compare geometries, lone pairs, and radicals
		for(const auto vDom : asRange(vertices(gMorDom))) {
			const auto vCodom = get(m, gMorDom, gMorCodom, vDom);
			if(vCodom == boost::graph_traits<GraphMorCodom>::null_vertex()) continue;
			const bool ok = getDerived().unifyGeometries(vDom, vCodom, pDom, pCodom);
			if(!ok) return true;
		}
		// Map neighbourhoods and check configurations
		for(const auto vDom : asRange(vertices(gMorDom))) {
			const auto vCodom = get(m, gMorDom, gMorCodom, vDom);
			if(vCodom == boost::graph_traits<GraphMorCodom>::null_vertex()) continue;
			const bool ok = getDerived().mapNeighbourhoods(vDom, vCodom, pDom, pCodom, m, gMorDom, gMorCodom);
			if(!ok) return true;
		}

		return next(getDerived().wrapVertexMap(std::move(m), pDom, pCodom, gMorDom, gMorCodom), gMorDom, gMorCodom);
	}

private:

	const Derived &getDerived() const {
		return static_cast<const Derived &> (*this);
	}

protected:

	template<typename F, typename F2>
	struct VertexMapper {

		VertexMapper(F f, F2 f2) : f(f), f2(f2) {}

		template<typename ...Args>
		decltype(auto) operator()(Args &&... args) const {
			return f(std::forward<Args>(args)...);
		}

		template<typename ...Args>
		decltype(auto) operator()(bool a, bool b, Args &&... args) const {
			return f2(a, b, std::forward<Args>(args)...);
		}

	private:
		F f;
		F2 f2;
	};

	template<typename F, typename F2>
	static auto makeVertexMapper(F f, F2 f2) {
		return VertexMapper<F, F2>(f, f2);
	}

protected:
	const LabGraphDom &lgDom;
	const LabGraphCodom &lgCodom;
	Next next;
};

template<typename VertexDom, typename VertexCodom, typename VertexMap, typename GraphMorDom, typename GraphMorCodom, typename LabGraphDom, typename LabGraphCodom>
std::vector<std::size_t> makePermutation(const VertexDom &vDom,
													  const VertexCodom &vCodom,
													  const VertexMap &m,
													  const GraphMorDom &gMorDom,
													  const GraphMorCodom &gMorCodom,
													  const ConfPtr &cDom,
													  const ConfPtr &cCodom,
													  const LabGraphDom &gLabDom,
													  const LabGraphCodom &gLabCodom) {
	const auto &gDom = get_graph(gLabDom);
	const auto &gCodom = get_graph(gLabCodom);
	const auto lDeg = cDom->degree();
	//						const auto rDeg = r->degree();
	const auto lBeg = cDom->begin();
	const auto rBeg = cCodom->begin();
	const auto rEnd = cCodom->end();
	std::vector<std::size_t> perm(lDeg);
	for(std::size_t i = 0; i < lDeg; ++i) {
		const auto &lEmb = lBeg[i];
		switch(lEmb.type) {
		case EmbeddingEdge::Type::Edge: {
			const auto &eOutLeft = lEmb.getEdge(vDom, gDom);
			const auto vOutLeft = target(eOutLeft, gDom);
			const auto vOutRight = get(m, gMorDom, gMorCodom, vOutLeft);
			if(vOutRight == boost::graph_traits<GraphMorCodom>::null_vertex()) {
				// TODO: this is getting complicated, examples?
				MOD_ABORT;
			} else {
				const auto oesRight = out_edges(vCodom, gCodom);
				const auto oeRightIter = std::find_if(oesRight.first, oesRight.second,
																  [vOutRight, &gCodom](const auto &eOutRight) {
																	  return target(eOutRight, gCodom) == vOutRight;
																  });
				if(oeRightIter == oesRight.second) {
					// TODO: so, we have an edge to a vertex, which is not mapped to another edge?
					MOD_ABORT;
				}
				const auto oeRightBeg = oesRight.first;
				const std::size_t offsetRight = std::distance(oeRightBeg, oeRightIter);
				// Find the position of this one in the right embedding edges
				const auto embPtrRight = std::find_if(rBeg, rEnd, [offsetRight](const auto &embRight) {
					return embRight.type == Stereo::EmbeddingEdge::Type::Edge && embRight.offset == offsetRight;
				});
				if(embPtrRight == rEnd) {
					// TODO: can this happen?
					MOD_ABORT;
				}
				perm[i] = embPtrRight - rBeg;
			}
		}
			break;
		case Stereo::EmbeddingEdge::Type::LonePair: perm[i] = -1; // to be mapped later
			break;
		case Stereo::EmbeddingEdge::Type::Radical: MOD_ABORT;
			perm[i] = -2; // to be mapped later
			break;
		}
	}
	return perm;
}

template<typename LabGraphDom, typename LabGraphCodom, typename Next>
struct ToVertexMapIso
		: ToVertexMapBase<ToVertexMapIso<LabGraphDom, LabGraphCodom, Next>, LabGraphDom, LabGraphCodom, Next> {
	using Base = ToVertexMapBase<ToVertexMapIso<LabGraphDom, LabGraphCodom, Next>, LabGraphDom, LabGraphCodom, Next>;
	friend Base;
	using typename Base::Handler;
public:
	using Base::Base;
	ToVertexMapIso() = delete; // to get warning to go away
private:

	template<typename EdgeDom, typename EdgeCodom, typename PropDom, typename PropCodom>
	bool unifyEdges(const EdgeDom &eDom, const EdgeCodom &eCodom, const PropDom &pDom, const PropCodom &pCodom) const {
		return Handler::reduce(std::logical_and<>(),
									  Handler::fmap2(get(pDom, eDom), get(pCodom, eCodom), this->lgDom, this->lgCodom,
														  LocalIso()));
	}

	template<typename VertexDom, typename VertexCodom, typename PropDom, typename PropCodom>
	bool unifyGeometries(const VertexDom &vDom,
								const VertexCodom &vCodom,
								const PropDom &pDom,
								const PropCodom &pCodom) const {
		return Handler::reduce(std::logical_and<>(),
									  Handler::fmap2(get(pDom, vDom), get(pCodom, vCodom), this->lgDom, this->lgCodom,
														  LocalIso()));
	}

	template<typename VertexDom, typename VertexCodom, typename PropDom, typename PropCodom, typename VertexMap, typename GraphMorDom, typename GraphMorCodom>
	bool mapNeighbourhoods(const VertexDom &vDom,
								  const VertexCodom &vCodom,
								  const PropDom &pDom,
								  const PropCodom &pCodom,
								  const VertexMap &m,
								  const GraphMorDom &gMorDom,
								  const GraphMorCodom &gMorCodom) const {
		const auto mapper = [&vDom, &gMorDom, &vCodom, &gMorCodom, &m](const ConfPtr &cDom,
																							const ConfPtr &cCodom,
																							const auto &gLabDom,
																							const auto &gLabCodom) {
			if(cDom->morphismStaticOk()) return true;
			if(cCodom->morphismStaticOk()) return true;
			if(cDom->morphismDynamicOk()) return true;
			if(cCodom->morphismDynamicOk()) return true;
			auto perm = makePermutation(vDom, vCodom, m, gMorDom, gMorCodom, cDom, cCodom, gLabDom, gLabCodom);
			return cDom->morphismIso(*cCodom, perm);
		};
		const auto vMapper = Base::makeVertexMapper(mapper, jla_boost::AlwaysTrue());
		return Handler::reduce(std::logical_and<>(),
									  Handler::fmap2(get(pDom, vDom), get(pCodom, vCodom), this->lgDom, this->lgCodom, vMapper));
	}

	template<typename VertexMap, typename PropDom, typename PropCodom, typename GraphMorDom, typename GraphMorCodom>
	auto wrapVertexMap(VertexMap &&m,
							 const PropDom &pDom,
							 const PropCodom &pCodom,
							 const GraphMorDom &gMorDom,
							 const GraphMorCodom &gMorCodom) const {
		// TODO: we _must_ add a stereo prop, otherwise the subsequent stages won't recognize that there is stereo!
		//		return GM::addProp(std::move(m), StereoDataT(), StereoDataIso());
		return std::move(m);
	}
};

template<typename LabGraphDom, typename LabGraphCodom, typename Next>
auto makeToVertexMapIso(const LabGraphDom &gDom, const LabGraphCodom &gCodom, Next next) {
	return ToVertexMapIso<LabGraphDom, LabGraphCodom, Next>(gDom, gCodom, next);
}

template<typename LabGraphDom, typename LabGraphCodom, typename Next>
struct ToVertexMapSpec
		: ToVertexMapBase<ToVertexMapSpec<LabGraphDom, LabGraphCodom, Next>, LabGraphDom, LabGraphCodom, Next> {
	using Base = ToVertexMapBase<ToVertexMapSpec<LabGraphDom, LabGraphCodom, Next>, LabGraphDom, LabGraphCodom, Next>;
	friend Base;
	using typename Base::Handler;
public:
	using Base::Base;
	ToVertexMapSpec() = delete; // to get warning to go away
private:

	template<typename EdgeDom, typename EdgeCodom, typename PropDom, typename PropCodom>
	bool unifyEdges(const EdgeDom &eDom, const EdgeCodom &eCodom, const PropDom &pDom, const PropCodom &pCodom) const {
		const auto &propDom = get(pDom, eDom);
		const auto &propCodom = get(pCodom, eCodom);
		return Handler::reduce(std::logical_and<>(),
									  Handler::fmap2(propDom, propCodom, this->lgDom, this->lgCodom, LocalSpec()));
	}

	template<typename VertexDom, typename VertexCodom, typename PropDom, typename PropCodom>
	bool unifyGeometries(const VertexDom &vDom,
								const VertexCodom &vCodom,
								const PropDom &pDom,
								const PropCodom &pCodom) const {
		return Handler::reduce(std::logical_and<>(),
									  Handler::fmap2(get(pDom, vDom), get(pCodom, vCodom), this->lgDom, this->lgCodom,
														  LocalSpec()));
	}

	template<typename VertexDom, typename VertexCodom, typename PropDom, typename PropCodom, typename VertexMap, typename GraphMorDom, typename GraphMorCodom>
	bool mapNeighbourhoods(const VertexDom &vDom,
								  const VertexCodom &vCodom,
								  const PropDom &pDom,
								  const PropCodom &pCodom,
								  const VertexMap &m,
								  const GraphMorDom &gMorDom,
								  const GraphMorCodom &gMorCodom) const {
		const auto mapper = [&vDom, &gMorDom, &vCodom, &gMorCodom, &m](const ConfPtr &cDom,
																							const ConfPtr &cCodom,
																							const auto &gLabDom,
																							const auto &gLabCodom) {
			if(cDom->morphismStaticOk()) return true;
			if(cCodom->morphismStaticOk()) return true;
			if(cDom->morphismDynamicOk()) return true;
			if(cCodom->morphismDynamicOk()) return true;
			auto perm = makePermutation(vDom, vCodom, m, gMorDom, gMorCodom, cDom, cCodom, gLabDom, gLabCodom);
			return cDom->morphismSpec(*cCodom, perm);
		};
		const auto vMapper = Base::makeVertexMapper(mapper, jla_boost::AlwaysTrue());
		return Handler::reduce(std::logical_and<>(),
									  Handler::fmap2(get(pDom, vDom), get(pCodom, vCodom), this->lgDom, this->lgCodom, vMapper));
	}

	template<typename VertexMap, typename PropDom, typename PropCodom, typename GraphMorDom, typename GraphMorCodom>
	auto wrapVertexMap(VertexMap &&m,
							 const PropDom &pDom,
							 const PropCodom &pCodom,
							 const GraphMorDom &gMorDom,
							 const GraphMorCodom &gMorCodom) const {
		return GM::addProp(std::move(m), StereoDataT(), StereoDataSpec());
	}
};

template<typename LabGraphDom, typename LabGraphCodom, typename Next>
auto makeToVertexMapSpec(const LabGraphDom &lgDom, const LabGraphCodom &lgCodom, Next next) {
	return ToVertexMapSpec<LabGraphDom, LabGraphCodom, Next>(lgDom, lgCodom, next);
}

} // namespace Stereo
} // namespace GraphMorphism
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPHMORPHISM_STEREOVERTEXMAP_H */

