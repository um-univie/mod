#ifndef MOD_LIB_RC_DETAIL_COMPOSITIONHELPER_H
#define MOD_LIB_RC_DETAIL_COMPOSITIONHELPER_H

#include <mod/lib/IO/IO.hpp>

#include <jla_boost/graph/dpo/Rule.hpp>

#include <boost/optional.hpp>

namespace mod {
namespace lib {
namespace RC {
namespace detail {

using jla_boost::GraphDPO::Membership;

template<bool Verbose, typename Result, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Visitor>
struct CompositionHelper {
	using GraphResult = typename jla_boost::GraphDPO::PushoutRuleTraits<typename Result::RuleResult>::GraphType;
	using VertexResult = typename boost::graph_traits<GraphResult>::vertex_descriptor;
	using EdgeResult = typename boost::graph_traits<GraphResult>::edge_descriptor;

	using GraphFirst = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleFirst>::GraphType;
	using VertexFirst = typename boost::graph_traits<GraphFirst>::vertex_descriptor;

	using GraphSecond = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleSecond>::GraphType;
	using VertexSecond = typename boost::graph_traits<GraphSecond>::vertex_descriptor;
	using EdgeSecond = typename boost::graph_traits<GraphSecond>::edge_descriptor;
public:

	CompositionHelper(const RuleFirst &rFirst,
							const RuleSecond &rSecond,
							const InvertibleVertexMap &match,
	                        Visitor visitor)
			: rFirst(rFirst), rSecond(rSecond), match(match), visitor(std::move(visitor)) {}

	boost::optional<Result> operator()(const std::vector<VertexFirst>* copyVertices) &&{
		if(Verbose) std::cout << std::string(80, '=') << std::endl;
		Result result(rFirst, rSecond);
		bool resInit = visitor.template init<Verbose>(rFirst, rSecond, match, result);
		if(!resInit) {
			if(Verbose) std::cout << std::string(80, '=') << std::endl;
			return boost::none;
		}
		// Vertices
		//--------------------------------------------------------------------------
		if (copyVertices == nullptr) {
			copyVerticesFirst(result);
		} else {
			copyVerticesFirst(result, *copyVertices);
		}
		composeVerticesSecond(result);
		// Edges
		//--------------------------------------------------------------------------
//		bool resFirst = copyEdgesFirstUnmatched(result);
		bool resFirst = false;
		if (copyVertices == nullptr) {
			resFirst = copyEdgesFirstUnmatched(result);
		} else {
			resFirst = copyEdgesFirstUnmatched(result, *copyVertices);
		}
		if(!resFirst) {
			if(Verbose) std::cout << std::string(80, '=') << std::endl;
			return boost::none;
		}
		bool resSecond = composeEdgesSecond(result);
		if(!resSecond) {
			if(Verbose) std::cout << std::string(80, '=') << std::endl;
			return boost::none;
		}

		// Finish it
		//--------------------------------------------------------------------------
		bool resFinal = visitor.template finalize<Verbose>(rFirst, rSecond, match, result);
		if(!resFinal) {
			if(Verbose) std::cout << std::string(80, '=') << std::endl;
			return boost::none;
		}
		if(Verbose) std::cout << std::string(80, '=') << std::endl;
		return boost::optional<Result>(std::move(result)); // something strange is going on with just "return result;"
	}

private:
	VertexFirst getNullFirst() const {
		return boost::graph_traits<GraphFirst>::null_vertex();
	}

	VertexSecond getNullSecond() const {
		return boost::graph_traits<GraphSecond>::null_vertex();
	}

	VertexResult getNullResult() const {
		return boost::graph_traits<GraphResult>::null_vertex();
	}

	VertexFirst getVertexFirstChecked(VertexSecond vSecond) const {
		const auto &gCodom = get_graph(get_labelled_right(rFirst));
		const auto &gDom = get_graph(get_labelled_left(rSecond));
		const auto m = membership(rSecond, vSecond);
		if(m == Membership::Right) return getNullFirst();
		else return get(match, gDom, gCodom, vSecond);
	}

	VertexSecond getVertexSecond(VertexFirst vFirst) const {
		const auto &gCodom = get_graph(get_labelled_right(rFirst));
		const auto &gDom = get_graph(get_labelled_left(rSecond));
		assert(membership(rFirst, vFirst) != Membership::Left);
		return get_inverse(match, gDom, gCodom, vFirst);
	}

private:

	void copyVerticesFirst(Result &result) {
		if(Verbose) std::cout << "copyVerticesFirst\n" << std::string(80, '-') << std::endl;
		auto &rResult = result.rResult;
		auto &gResult = get_graph(rResult);
		const auto &gFirst = get_graph(rFirst);
		const auto &gSecond = get_graph(rSecond);
		for(const auto vFirst : asRange(vertices(gFirst))) {
			if(Verbose) {
				std::cout << "rFirst node:\t"
							 << get(boost::vertex_index_t(), gFirst, vFirst)
							 << "(" << membership(rFirst, vFirst) << ")"
							 << "(";
				visitor.template printVertexFirst(rFirst, rSecond, match, result, std::cout, vFirst);
				std::cout << ")" << std::endl;
			}
			const bool getsDeleted = [&]() {
				// must be only in R to be deleted
				if(membership(rFirst, vFirst) != Membership::Right) return false;
				// must be matched to be deleted
				const auto vSecond = getVertexSecond(vFirst);
				if(vSecond == getNullSecond()) return false;
				// and the matched must only be in L
				return membership(rSecond, vSecond) == Membership::Left;
			}();
			if(getsDeleted) {
				put(result.mFirstToResult, gFirst, gResult, vFirst, getNullResult());
				if(Verbose) std::cout << "gets deleted" << std::endl;
			} else {
				const auto vResult = add_vertex(gResult);
				result.mFirstToResult.resizeRight(gFirst, gResult);
				result.mSecondToResult.resizeRight(gSecond, gResult);
				put(result.mFirstToResult, gFirst, gResult, vFirst, vResult);
				put_membership(rResult, vResult, membership(rFirst, vFirst));
				visitor.template copyVertexFirst<Verbose>(rFirst, rSecond, match, result, vFirst, vResult);
				if(Verbose) {
					std::cout << "new node:\t"
								 << get(boost::vertex_index_t(), gResult, vResult)
								 << "(" << membership(rResult, vResult) << ")(";
					visitor.template printVertexResult(rFirst, rSecond, match, result, std::cout, vResult);
					std::cout << ")" << std::endl;
				}
			}
		}
	}

	void copyVerticesFirst(Result &result, const std::vector<VertexFirst>& copyVertices) {
		if(Verbose) std::cout << "copyVerticesFirst\n" << std::string(80, '-') << std::endl;
		auto &rResult = result.rResult;
		auto &gResult = get_graph(rResult);
		const auto &gFirst = get_graph(rFirst);
		const auto &gSecond = get_graph(rSecond);
		for(const auto vFirst : copyVertices) {
			if(Verbose) {
				std::cout << "rFirst node:\t"
				             << get(boost::vertex_index_t(), gFirst, vFirst)
				             << "(" << membership(rFirst, vFirst) << ")"
				             << "(";
				visitor.template printVertexFirst(rFirst, rSecond, match, result, std::cout, vFirst);
				std::cout << ")" << std::endl;
			}
			const bool getsDeleted = [&]() {
				// must be only in R to be deleted
				if(membership(rFirst, vFirst) != Membership::Right) return false;
				// must be matched to be deleted
				const auto vSecond = getVertexSecond(vFirst);
				if(vSecond == getNullSecond()) return false;
				// and the matched must only be in L
				return membership(rSecond, vSecond) == Membership::Left;
			}();
			if(getsDeleted) {
				put(result.mFirstToResult, gFirst, gResult, vFirst, getNullResult());
				if(Verbose) std::cout << "gets deleted" << std::endl;
			} else {
				const auto vResult = add_vertex(gResult);
				result.mFirstToResult.resizeRight(gFirst, gResult);
				result.mSecondToResult.resizeRight(gSecond, gResult);
				put(result.mFirstToResult, gFirst, gResult, vFirst, vResult);
				put_membership(rResult, vResult, membership(rFirst, vFirst));
				visitor.template copyVertexFirst<Verbose>(rFirst, rSecond, match, result, vFirst, vResult);
				if(Verbose) {
					std::cout << "new node:\t"
					             << get(boost::vertex_index_t(), gResult, vResult)
					             << "(" << membership(rResult, vResult) << ")(";
					visitor.template printVertexResult(rFirst, rSecond, match, result, std::cout, vResult);
					std::cout << ")" << std::endl;
				}
			}
		}
	}

	void composeVerticesSecond(Result &result) {
		if(Verbose) std::cout << "composeVerticesSecond\n" << std::string(80, '-') << std::endl;
		const auto &gFirst = get_graph(rFirst);
		const auto &gSecond = get_graph(rSecond);
		auto &rResult = result.rResult;
		auto &gResult = get_graph(rResult);
		// copy nodes from second, but compose the matched ones which has already been created
		for(const auto vSecond : asRange(vertices(gSecond))) {
			if(Verbose) {
				std::cout << "rSecond node:\t"
							 << get(boost::vertex_index_t(), gSecond, vSecond)
							 << "(" << membership(rSecond, vSecond) << ")"
							 << "(";
				visitor.template printVertexSecond(rFirst, rSecond, match, result, std::cout, vSecond);
				std::cout << ")" << std::endl;
			}
			// check if the vertex is matched
			const auto vFirst = getVertexFirstChecked(vSecond);
			if(vFirst == getNullFirst()) {
				// unmatched vertex, create it
				const auto vResult = add_vertex(gResult);
				result.mFirstToResult.resizeRight(gFirst, gResult);
				result.mSecondToResult.resizeRight(gSecond, gResult);
				put(result.mSecondToResult, gSecond, gResult, vSecond, vResult);
				put_membership(rResult, vResult, membership(rSecond, vSecond));
				visitor.template copyVertexSecond<Verbose>(rFirst, rSecond, match, result, vSecond, vResult);
				if(Verbose) {
					std::cout << "new node:\t" << get(boost::vertex_index_t(), gResult, vResult)
								 << "(" << membership(rResult, vResult) << ")"
								 << "(";
					visitor.template printVertexResult(rFirst, rSecond, match, result, std::cout, vResult);
					std::cout << ")" << std::endl;
				}
			} else { // vertex matched
				const auto vResult = get(result.mFirstToResult, gFirst, gResult, vFirst);
				put(result.mSecondToResult, gSecond, gResult, vSecond, vResult);
				if(vResult == getNullResult()) {
					if(Verbose) std::cout << "deleted" << std::endl;
				} else {
					if(Verbose) {
						std::cout << "match to:\t" << get(boost::vertex_index_t(), gResult, vResult)
									 << "(" << membership(rResult, vResult) << ")"
									 << "(";
						visitor.template printVertexResult(rFirst, rSecond, match, result, std::cout, vResult);
						std::cout << ")" << std::endl;
					}
					// now we calculate the new membership for the node
					const auto mFirst = membership(rResult, vResult); // should be a copy of the one from rFirst
					const auto mSecond = membership(rSecond, vSecond);
					if(mFirst == Membership::Right) {
						// vFirst appears
						if(mSecond == Membership::Left) {
							// and vSecond disappears, so vResult should not exist at all
							// We should have caught that already.
							assert(false);
							std::abort();
						} else {
							// vSecond is CONTEXT, so vResult must be RIGHT (which it already is in rResult)
							visitor.template composeVertexRvsLR<Verbose>(rFirst, rSecond, match, result, vResult, vSecond);
						}
					} else { // vFirst is either LEFT or CONTEXT
						assert(mFirst != Membership::Left); // we can't match a LEFT node in rRight
						// vFirst is CONTEXT
						if(mSecond == Membership::Left) {
							// vSecond is disappearing, so vResult is disappearing as well
							visitor.template composeVertexLRvsL<Verbose>(rFirst, rSecond, match, result, vResult, vSecond);
							put_membership(rResult, vResult, Membership::Left);
						} else { // both vFirst and vSecond is in both of their sides
							// let the visitor do its thing
							visitor.template composeVertexLRvsLR<Verbose>(rFirst, rSecond, match, result, vResult, vSecond);
							assert(membership(rResult, vResult) == Membership::Context);
						} // end if vSecond is LEFT
					} // end if vFirst is RIGHT
				} // end if vResult is null_vertex
			} // end if vSecond is unmatched
		} // end foreach vSecond
	}

	bool copyEdgesFirstUnmatched(Result &result, const std::vector<VertexFirst>& copyVertices) {
		if(Verbose) std::cout << "copyEdgesFirstUnmatched\n" << std::string(80, '-') << std::endl;
		const auto &gFirst = get_graph(rFirst);
		const auto &gSecond = get_graph(rSecond);
		auto &rResult = result.rResult;
		auto &gResult = get_graph(rResult);
		const auto processEdge = [&](const auto eFirst) {
			// map source and target to core vertices
			const auto vSrcFirst = source(eFirst, gFirst);
			const auto vTarFirst = target(eFirst, gFirst);
			const auto meFirst = membership(rFirst, eFirst);
			if(Verbose) {
				const auto vSrcResult = get(result.mFirstToResult, gFirst, gResult, vSrcFirst);
				const auto vTarResult = get(result.mFirstToResult, gFirst, gResult, vTarFirst);
				// vSrcResult/vTarResult may be null_vertex
				std::cout << "Edge first:\t(" << vSrcFirst << ", " << vTarFirst << ") maybe copy to new (" << vSrcResult
				             << ", " << vTarResult << ")"
				             << "(" << meFirst << ")"
				             << "(";
				visitor.template printEdgeFirst(rFirst, rSecond, match, result, std::cout, eFirst);
				std::cout << ")" << std::endl;
			}
			const auto makeCopy = [&]() {
				const auto vSrcResult = get(result.mFirstToResult, gFirst, gResult, vSrcFirst);
				const auto vTarResult = get(result.mFirstToResult, gFirst, gResult, vTarFirst);
				// vResultSrc/vResultTar should be valid at this point
				assert(vSrcResult != getNullResult());
				assert(vTarResult != getNullResult());
				if(Verbose) std::cout << "\tCopy eFirst" << std::endl;
				const auto peResult = add_edge(vSrcResult, vTarResult, gResult);
				// adding shouldn't fail
				assert(peResult.second);
				const auto eResult = peResult.first;
				put_membership(rResult, eResult, membership(rFirst, eFirst));
				visitor.template copyEdgeFirst<Verbose>(rFirst, rSecond, match, result, eFirst, eResult);
				assert(
				        membership(rResult, vSrcResult) == Membership::Context || membership(rResult, vSrcResult) == meFirst);
				assert(
				        membership(rResult, vTarResult) == Membership::Context || membership(rResult, vTarResult) == meFirst);
			};
			// and now the actual case analysis
			if(meFirst == Membership::Left) {
				if(Verbose) std::cout << "\teFirst in LEFT, clean copy" << std::endl;
				makeCopy();
				return true;
			}
			if(Verbose) std::cout << "\teFirst in RIGHT or CONTEXT" << std::endl;
			const auto vSrcSecond = getVertexSecond(vSrcFirst);
			const auto vTarSecond = getVertexSecond(vTarFirst);
			const bool isSrcMatched = vSrcSecond != getNullSecond();
			const bool isTarMatched = vTarSecond != getNullSecond();
			if(isSrcMatched && isTarMatched) {
				if(Verbose) std::cout << "\tBoth ends matched" << std::endl;
				const auto oeSecond = out_edges(vSrcSecond, gSecond);
				const auto eSecondIter = std::find_if(oeSecond.first, oeSecond.second,
				                                                  [&gSecond, vTarSecond, this](const auto &eSecond) {
					                                                  return target(eSecond, gSecond) == vTarSecond &&
																	            membership(rSecond, eSecond) != Membership::Right;
				                                                  });
				// TODO: why do we check the membership in the find_if?
				// There should be only one edge, so we can bail out here, right?
				const bool isEdgeMatched = eSecondIter != oeSecond.second;
				if(isEdgeMatched) {
					const auto eSecond = *eSecondIter;
					if(meFirst == Membership::Right) {
						if(Verbose) std::cout << "\teFirst matched and in RIGHT, skipping" << std::endl;
						return true;
					}

					if(Verbose)
						std::cout << "\teFirst matched and in CONTEXT, copying to LEFT or CONTEXT (depending on eSecond ("
						             << membership(rSecond, eSecond) << "))" << std::endl;
					const auto vSrcResult = get(result.mFirstToResult, gFirst, gResult, vSrcFirst);
					const auto vTarResult = get(result.mFirstToResult, gFirst, gResult, vTarFirst);
					// vResultSrc/vResultTar can not be null_vertex
					assert(vSrcResult != getNullResult());
					assert(vTarResult != getNullResult());
					const auto peResult = add_edge(vSrcResult, vTarResult, gResult);
					// adding shouldn't fail
					assert(peResult.second);
					const auto eResult = peResult.first;
					// we use the second membership, so do not use the common copy mechanism in the bottom of the function
					put_membership(rResult, eResult, membership(rSecond, eSecond));
					visitor.template copyEdgeFirst<Verbose>(rFirst, rSecond, match, result, eFirst, eResult);
					assert(membership(rResult, vSrcResult) != Membership::Right);
					assert(membership(rResult, vTarResult) != Membership::Right);
					return true;
				}

				// eFirst not matched, the ends must be consistent and not deleted
				const auto vSrcResult = get(result.mFirstToResult, gFirst, gResult, vSrcFirst);
				const auto vTarResult = get(result.mFirstToResult, gFirst, gResult, vTarFirst);
				// vSrcResult/vTarResult may be null_vertex
				const bool endPointDeleted = vSrcResult == getNullResult() || vTarResult == getNullResult();
				if(endPointDeleted) {
					if(Verbose) std::cout << "\tComposition failure: at least one matched vertex deleted" << std::endl;
					return false;
				}

				// check consistency
				const auto mvSrcResult = membership(rResult, vSrcResult);
				const auto mvTarResult = membership(rResult, vTarResult);
				if(mvSrcResult == Membership::Left || mvTarResult == Membership::Left) {
					if(Verbose)
						std::cout << "\tComposition failure: at least one matched vertex has inconsistent context ("
						             << mvSrcResult << " and " << mvTarResult << "), eFirst is (" << meFirst << ")" << std::endl;
					return false;
				}

				if(Verbose) std::cout << "\teFirst not matched and not dangling" << std::endl;
				makeCopy();
				return true;
			}

			// at most 1 end matched
			if(isSrcMatched != isTarMatched) {
				if(Verbose) std::cout << "\tOne end matched" << std::endl;
				const auto vSrcResult = get(result.mFirstToResult, gFirst, gResult, vSrcFirst);
				const auto vTarResult = get(result.mFirstToResult, gFirst, gResult, vTarFirst);
				// vResultSrc/vResultTar may be null_vertex
				const auto vResultMatched = isSrcMatched ? vSrcResult : vTarResult;
				const bool matchedDeleted = vResultMatched == boost::graph_traits<GraphResult>::null_vertex();
				if(matchedDeleted) {
					if(Verbose) std::cout << "\tComposition failure: matched vertex deleted" << std::endl;
					return false;
				}
				// matched, but is it consistent?
				const auto mvMatchedResult = membership(rResult, vResultMatched);
				if(mvMatchedResult == Membership::Left) {
					if(Verbose)
						std::cout << "\tComposition failure: matched vertex has inconsistent context ("
						             << mvMatchedResult << "), eFirst is (" << meFirst << ")" << std::endl;
					return false;
				}

				if(Verbose) std::cout << "\teFirst not matched and not dangling" << std::endl;
				makeCopy();
				return true;
			}

			if(Verbose) std::cout << "\tNo ends matched" << std::endl;
			makeCopy();
			return true;
		};
		for (VertexFirst v : copyVertices) {
			for (const auto eFirst : asRange(out_edges(v, gFirst))) {
				auto tar = target(eFirst, gFirst);
				assert(v != tar);
				if (v > tar) { continue; }
				const bool ok = processEdge(eFirst);
				if(!ok) return false;
			}
		}
//		for(const auto eFirst : asRange(edges(gFirst))) {
//			const bool ok = processEdge(eFirst);
//			if(!ok) return false;
//		}
		return true;
	}

	bool copyEdgesFirstUnmatched(Result &result) {
		if(Verbose) std::cout << "copyEdgesFirstUnmatched\n" << std::string(80, '-') << std::endl;
		const auto &gFirst = get_graph(rFirst);
		const auto &gSecond = get_graph(rSecond);
		auto &rResult = result.rResult;
		auto &gResult = get_graph(rResult);
		const auto processEdge = [&](const auto eFirst) {
			// map source and target to core vertices
			const auto vSrcFirst = source(eFirst, gFirst);
			const auto vTarFirst = target(eFirst, gFirst);
			const auto meFirst = membership(rFirst, eFirst);
			if(Verbose) {
				const auto vSrcResult = get(result.mFirstToResult, gFirst, gResult, vSrcFirst);
				const auto vTarResult = get(result.mFirstToResult, gFirst, gResult, vTarFirst);
				// vSrcResult/vTarResult may be null_vertex
				std::cout << "Edge first:\t(" << vSrcFirst << ", " << vTarFirst << ") maybe copy to new (" << vSrcResult
							 << ", " << vTarResult << ")"
							 << "(" << meFirst << ")"
							 << "(";
				visitor.template printEdgeFirst(rFirst, rSecond, match, result, std::cout, eFirst);
				std::cout << ")" << std::endl;
			}
			const auto makeCopy = [&]() {
				const auto vSrcResult = get(result.mFirstToResult, gFirst, gResult, vSrcFirst);
				const auto vTarResult = get(result.mFirstToResult, gFirst, gResult, vTarFirst);
				// vResultSrc/vResultTar should be valid at this point
				assert(vSrcResult != getNullResult());
				assert(vTarResult != getNullResult());
				if(Verbose) std::cout << "\tCopy eFirst" << std::endl;
				const auto peResult = add_edge(vSrcResult, vTarResult, gResult);
				// adding shouldn't fail
				assert(peResult.second);
				const auto eResult = peResult.first;
				put_membership(rResult, eResult, membership(rFirst, eFirst));
				visitor.template copyEdgeFirst<Verbose>(rFirst, rSecond, match, result, eFirst, eResult);
				assert(
						membership(rResult, vSrcResult) == Membership::Context || membership(rResult, vSrcResult) == meFirst);
				assert(
						membership(rResult, vTarResult) == Membership::Context || membership(rResult, vTarResult) == meFirst);
			};
			// and now the actual case analysis
			if(meFirst == Membership::Left) {
				if(Verbose) std::cout << "\teFirst in LEFT, clean copy" << std::endl;
				makeCopy();
				return true;
			}
			if(Verbose) std::cout << "\teFirst in RIGHT or CONTEXT" << std::endl;
			const auto vSrcSecond = getVertexSecond(vSrcFirst);
			const auto vTarSecond = getVertexSecond(vTarFirst);
			const bool isSrcMatched = vSrcSecond != getNullSecond();
			const bool isTarMatched = vTarSecond != getNullSecond();
			if(isSrcMatched && isTarMatched) {
				if(Verbose) std::cout << "\tBoth ends matched" << std::endl;
				const auto oeSecond = out_edges(vSrcSecond, gSecond);
				const auto eSecondIter = std::find_if(oeSecond.first, oeSecond.second,
																  [&gSecond, vTarSecond, this](const auto &eSecond) {
																	  return target(eSecond, gSecond) == vTarSecond &&
																				membership(rSecond, eSecond) != Membership::Right;
																  });
				// TODO: why do we check the membership in the find_if?
				// There should be only one edge, so we can bail out here, right?
				const bool isEdgeMatched = eSecondIter != oeSecond.second;
				if(isEdgeMatched) {
					const auto eSecond = *eSecondIter;
					if(meFirst == Membership::Right) {
						if(Verbose) std::cout << "\teFirst matched and in RIGHT, skipping" << std::endl;
						return true;
					}

					if(Verbose)
						std::cout << "\teFirst matched and in CONTEXT, copying to LEFT or CONTEXT (depending on eSecond ("
									 << membership(rSecond, eSecond) << "))" << std::endl;
					const auto vSrcResult = get(result.mFirstToResult, gFirst, gResult, vSrcFirst);
					const auto vTarResult = get(result.mFirstToResult, gFirst, gResult, vTarFirst);
					// vResultSrc/vResultTar can not be null_vertex
					assert(vSrcResult != getNullResult());
					assert(vTarResult != getNullResult());
					const auto peResult = add_edge(vSrcResult, vTarResult, gResult);
					// adding shouldn't fail
					assert(peResult.second);
					const auto eResult = peResult.first;
					// we use the second membership, so do not use the common copy mechanism in the bottom of the function
					put_membership(rResult, eResult, membership(rSecond, eSecond));
					visitor.template copyEdgeFirst<Verbose>(rFirst, rSecond, match, result, eFirst, eResult);
					assert(membership(rResult, vSrcResult) != Membership::Right);
					assert(membership(rResult, vTarResult) != Membership::Right);
					return true;
				}

				// eFirst not matched, the ends must be consistent and not deleted
				const auto vSrcResult = get(result.mFirstToResult, gFirst, gResult, vSrcFirst);
				const auto vTarResult = get(result.mFirstToResult, gFirst, gResult, vTarFirst);
				// vSrcResult/vTarResult may be null_vertex
				const bool endPointDeleted = vSrcResult == getNullResult() || vTarResult == getNullResult();
				if(endPointDeleted) {
					if(Verbose) std::cout << "\tComposition failure: at least one matched vertex deleted" << std::endl;
					return false;
				}

				// check consistency
				const auto mvSrcResult = membership(rResult, vSrcResult);
				const auto mvTarResult = membership(rResult, vTarResult);
				if(mvSrcResult == Membership::Left || mvTarResult == Membership::Left) {
					if(Verbose)
						std::cout << "\tComposition failure: at least one matched vertex has inconsistent context ("
									 << mvSrcResult << " and " << mvTarResult << "), eFirst is (" << meFirst << ")" << std::endl;
					return false;
				}

				if(Verbose) std::cout << "\teFirst not matched and not dangling" << std::endl;
				makeCopy();
				return true;
			}

			// at most 1 end matched
			if(isSrcMatched != isTarMatched) {
				if(Verbose) std::cout << "\tOne end matched" << std::endl;
				const auto vSrcResult = get(result.mFirstToResult, gFirst, gResult, vSrcFirst);
				const auto vTarResult = get(result.mFirstToResult, gFirst, gResult, vTarFirst);
				// vResultSrc/vResultTar may be null_vertex
				const auto vResultMatched = isSrcMatched ? vSrcResult : vTarResult;
				const bool matchedDeleted = vResultMatched == boost::graph_traits<GraphResult>::null_vertex();
				if(matchedDeleted) {
					if(Verbose) std::cout << "\tComposition failure: matched vertex deleted" << std::endl;
					return false;
				}
				// matched, but is it consistent?
				const auto mvMatchedResult = membership(rResult, vResultMatched);
				if(mvMatchedResult == Membership::Left) {
					if(Verbose)
						std::cout << "\tComposition failure: matched vertex has inconsistent context ("
									 << mvMatchedResult << "), eFirst is (" << meFirst << ")" << std::endl;
					return false;
				}

				if(Verbose) std::cout << "\teFirst not matched and not dangling" << std::endl;
				makeCopy();
				return true;
			}

			if(Verbose) std::cout << "\tNo ends matched" << std::endl;
			makeCopy();
			return true;
		};
		for(const auto eFirst : asRange(edges(gFirst))) {
			const bool ok = processEdge(eFirst);
			if(!ok) return false;
		}
		return true;
	}

	bool composeEdgesSecond(Result &result) {
		if(Verbose) std::cout << "composeEdgesSecond\n" << std::string(80, '-') << std::endl;
		//		const auto &gFirst = get_graph(rFirst);
		const auto &gSecond = get_graph(rSecond);
		auto &rResult = result.rResult;
		auto &gResult = get_graph(rResult);
		for(auto eSecond : asRange(edges(gSecond))) {
			auto vSecondSrc = source(eSecond, gSecond);
			auto vSecondTar = target(eSecond, gSecond);
			auto vResultSrc = get(result.mSecondToResult, gSecond, gResult, vSecondSrc);
			auto vResultTar = get(result.mSecondToResult, gSecond, gResult, vSecondTar);
			if(Verbose) {
				// vSrcNew/vTarNew may be null_vertex
				std::cout << "Edge second:\t(" << vSecondSrc << ", " << vSecondTar << ")\tmapped to (" << vResultSrc << ", "
							 << vResultTar << ")"
							 << ", (" << membership(rSecond, eSecond) << ")"
							 << "(";
				visitor.template printEdgeSecond(rFirst, rSecond, match, result, std::cout, eSecond);
				std::cout << ")" << std::endl;
			}
			// vResultSrc/vResultTar may be null_vertex
			// check for match on vFirstSrc and vFirstTar
			auto vFirstSrc = getVertexFirstChecked(vSecondSrc);
			auto vFirstTar = getVertexFirstChecked(vSecondTar);
			bool isSrcMatched = vFirstSrc != boost::graph_traits<GraphFirst>::null_vertex();
			bool isTarMatched = vFirstTar != boost::graph_traits<GraphFirst>::null_vertex();
			if(!isSrcMatched || !isTarMatched) { // new edge
				if(!isSrcMatched && !isTarMatched) {
					if(Verbose) std::cout << "No ends matched. Copy eSecond." << std::endl;
					// vResultSrc/vResultTar can not be null_vertex
					assert(vResultSrc != boost::graph_traits<GraphResult>::null_vertex());
					assert(vResultTar != boost::graph_traits<GraphResult>::null_vertex());
					auto peResult = add_edge(vResultSrc, vResultTar, gResult);
					// adding shouldn't fail
					assert(peResult.second);
					auto eResult = peResult.first;
					put_membership(rResult, eResult, membership(rSecond, eSecond));
					visitor.template copyEdgeSecond<Verbose>(rFirst, rSecond, match, result, eSecond, eResult);
				} else { // one end matched
					assert(vResultSrc != boost::graph_traits<GraphResult>::null_vertex()
							 || vResultTar != boost::graph_traits<GraphResult>::null_vertex());
					bool isOk = composeEdgeSecond_oneEndMatched(result, eSecond, vResultSrc, vResultTar, isSrcMatched,
																			  isTarMatched);
					if(!isOk) return false;
				} // if 0 or 1 end matched
			} else { // maybe a matched edge
				bool isOk = composeEdgeSecond_bothEndsMatched(result, eSecond, vFirstSrc, vFirstTar, vResultSrc,
																			 vResultTar);
				if(!isOk) return false;
			} // end if 0 or 1 matched endNodes
		} // end for each eSecond
		return true;
	}

private:

	bool composeEdgeSecond_oneEndMatched(Result &result,
													 EdgeSecond eSecond,
													 VertexResult vResultSrc,
													 VertexResult vResultTar,
													 bool isSrcMatched,
													 bool isTarMatched) {
		auto &rResult = result.rResult;
		auto &gResult = get_graph(rResult);
		if(Verbose) std::cout << "One end matched, eSecond should be copied" << std::endl;
		assert(isSrcMatched ^ isTarMatched);
		// at most one of vResultSrc and vResultTar may be null_vertex
		assert(vResultSrc != boost::graph_traits<GraphResult>::null_vertex()
				 || vResultTar != boost::graph_traits<GraphResult>::null_vertex());
		auto vResultMatched = isSrcMatched ? vResultSrc : vResultTar;
		auto vResultOther = isSrcMatched ? vResultTar : vResultSrc;
		// vResultMatched may be null_vertex, if it's a RIGHT vs. LEFT vertex
		assert(vResultOther != boost::graph_traits<GraphResult>::null_vertex());
		const auto meSecond = membership(rSecond, eSecond);
		const auto mvResultOther = membership(rResult, vResultOther);
		(void) mvResultOther;
		assert(mvResultOther == Membership::Context || mvResultOther == meSecond);

		if(vResultMatched == boost::graph_traits<GraphResult>::null_vertex()) {
			if(Verbose) std::cout << "\tComposition failure: matched vertex deleted" << std::endl;
			return false;
		}

		auto mvResultMatched = membership(rResult, vResultMatched);
		if(mvResultMatched != Membership::Context && mvResultMatched != meSecond) {
			if(Verbose)
				std::cout << "\tComposition failure: matched vertex has inconsistent context ("
							 << mvResultMatched << "), eSecond is (" << meSecond << ")" << std::endl;
			return false;
		}

		if(Verbose) std::cout << "\tCopy edge" << std::endl;
		auto peResult = add_edge(vResultSrc, vResultTar, gResult);
		// adding shouldn't fail
		assert(peResult.second);
		auto eResult = peResult.first;
		put_membership(rResult, eResult, membership(rSecond, eSecond));
		visitor.template copyEdgeSecond<Verbose>(rFirst, rSecond, match, result, eSecond, eResult);
		return true;
	}

	bool composeEdgeSecond_bothEndsMatched(Result &result,
														EdgeSecond eSecond,
														VertexFirst vFirstSrc,
														VertexFirst vFirstTar,
														VertexResult vResultSrc,
														VertexResult vResultTar) {
		const auto &gFirst = get_graph(rFirst);
		auto &rResult = result.rResult;
		auto &gResult = get_graph(rResult);
		if(Verbose) std::cout << "Both ends matched" << std::endl;
		// vResultSrc/vResultTar may be null_vertex
		boost::optional<Membership> omeFirst;
		// we search in coreFirst, because it has the matched edges
		for(auto eFirst : asRange(out_edges(vFirstSrc, gFirst))) {
			if(target(eFirst, gFirst) != vFirstTar) continue;
			omeFirst = membership(rFirst, eFirst);
			break;
		}
		auto meSecond = membership(rSecond, eSecond);

		// possibilities:
		//    ->   vs. |  -> 
		//    ->   vs.    -> |
		//    ->   vs. |  -> |
		// |  ->   vs. |  -> 
		// |  ->   vs.    -> |
		// |  ->   vs. |  -> |
		//    -> | vs. |  ->	first not in result
		//    -> | vs.    -> |
		//    -> | vs. |  -> |	first not in result
		// |  -> | vs. |  -> 	first in result as LEFT
		// |  -> | vs.    -> |
		// |  -> | vs. |  -> |	first in result as CONTEXT
		// can we do a simple copy?
		if(!omeFirst) {
			//    ->   vs. |  -> 
			//    ->   vs.    -> |
			//    ->   vs. |  -> |
			if(Verbose) std::cout << "\tSimple copy of eSecond" << std::endl;
			if(meSecond == Membership::Left) {
				if(Verbose) std::cout << "\t\teSecond in LEFT, check ends" << std::endl;
				// vResultSrc/vResultTar may be null_vertex
				if(vResultSrc == boost::graph_traits<GraphResult>::null_vertex()) {
					if(Verbose) std::cout << "\tComposition failure: vResultSrc deleted" << std::endl;
					return false;
				}
				if(vResultTar == boost::graph_traits<GraphResult>::null_vertex()) {
					if(Verbose) std::cout << "\tComposition failure: vResultTar deleted" << std::endl;
					return false;
				}
				if(membership(rResult, vResultSrc) == Membership::Right) {
					if(Verbose) std::cout << "\tComposition failure: vResultSrc not in LEFT or CONTEXT" << std::endl;
					return false;
				}
				if(membership(rResult, vResultTar) == Membership::Right) {
					if(Verbose) std::cout << "\tComposition failure: vResultTar not in LEFT or CONTEXT" << std::endl;
					return false;
				}
			}
			// vResultSrc/vResultTar may not be null_vertex
			assert(vResultSrc != boost::graph_traits<GraphResult>::null_vertex());
			assert(vResultTar != boost::graph_traits<GraphResult>::null_vertex());
			auto peResult = add_edge(vResultSrc, vResultTar, gResult);
			// adding shouldn't fail
			assert(peResult.second);
			auto eResult = peResult.first;
			put_membership(rResult, eResult, membership(rSecond, eSecond));
			visitor.template copyEdgeSecond<Verbose>(rFirst, rSecond, match, result, eSecond, eResult);
			return true;
		}

		// check for parallel errors
		assert(omeFirst);
		auto meFirst = *omeFirst;
		if(meFirst == Membership::Left && meSecond != Membership::Right) {
			// creating parallel in left
			// |  ->   vs. |  ->
			// |  ->   vs. |  -> |
			if(Verbose) std::cout << "\tComposition failure: duplicate edge in L" << std::endl;
			return false;
		}
		if(meFirst != Membership::Left && meSecond == Membership::Right) {
			// creating parallel in right
			//    -> | vs.    -> |
			// |  -> | vs.    -> |
			if(Verbose)
				std::cout << "\tComposition failure: eSecond in RIGHT, eFirst in CONTEXT or RIGHT, i.e., parallel edge in R"
							 << std::endl;
			return false;
		}
		// remaining possibilities:
		// |  ->   vs.    -> |
		//    -> | vs. |  ->	first not in result
		//    -> | vs. |  -> |	first not in result
		// |  -> | vs. |  -> 	first in result as LEFT
		// |  -> | vs. |  -> |	first in result as CONTEXT
		// check if we should just ignore the edge (matched or handled
		if(meSecond == Membership::Left) {
			//    -> | vs. |  ->	first not in result
			// |  -> | vs. |  -> 	first in result as LEFT
			if(Verbose)
				std::cout << "\teSecond matched in (" << meSecond << ")"
							 << ", either in LEFT or both eSecond and eFirst in CONTEXT" << std::endl;
			return true;
		}
		// remaining possibilities:
		// |  ->   vs.    -> |
		//    -> | vs. |  -> |	first not in result
		// |  -> | vs. |  -> |	first in result as CONTEXT
		// vResultSrc/vResultTar can not be null_vertex
		assert(vResultSrc != boost::graph_traits<GraphResult>::null_vertex());
		assert(vResultTar != boost::graph_traits<GraphResult>::null_vertex());
		if(meFirst == Membership::Right) {
			// copy eSecond as RIGHT to result
			assert(meSecond == Membership::Context);
			//    -> | vs. |  -> |
			if(Verbose) std::cout << "\t   -> | vs. |  -> |, copy to RIGHT" << std::endl;
			auto peResult = add_edge(vResultSrc, vResultTar, gResult);
			// adding shouldn't fail
			assert(peResult.second);
			auto eResult = peResult.first;
			put_membership(rResult, eResult, Membership::Right);
			visitor.template copyEdgeSecond<Verbose>(rFirst, rSecond, match, result, eSecond, eResult);
			return true;
		}
		// remaining possibilities:
		// |  ->   vs.    -> |
		// |  -> | vs. |  -> |	first in new as CONTEXT
		// we have to find the eNew in LEFT or CONTEXT
		auto edgesResult = out_edges(vResultSrc, gResult);
		auto edgeIter = std::find_if(edgesResult.first, edgesResult.second, [&gResult, vResultTar](EdgeResult eCand) {
			if(target(eCand, gResult) != vResultTar) return false;
			return true;
		});
		assert(edgeIter != edgesResult.second);
		auto eResult = *edgeIter;
		if(Verbose)
			std::cout
					<< "\t'|  ->   vs.    -> |' or '|  -> | vs. |  -> |', promote eNew to CONTEXT and set right from second right"
					<< std::endl;
		put_membership(rResult, eResult, Membership::Context);
		visitor.template setEdgeResultRightFromSecondRight<Verbose>(rFirst, rSecond, match, result, eResult, eSecond);
		return true;
	}

private:
	const RuleFirst &rFirst;
	const RuleSecond &rSecond;
	const InvertibleVertexMap &match;
	Visitor visitor;
};

} // namespace detail
} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_DETAIL_COMPOSITIONHELPER_H */
