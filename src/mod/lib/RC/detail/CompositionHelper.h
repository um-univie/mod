#ifndef MOD_LIB_RC_DETAIL_COMPOSITIONHELPER_H
#define MOD_LIB_RC_DETAIL_COMPOSITIONHELPER_H

#include <mod/lib/IO/IO.h>

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
	using GraphFirst = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleFirst>::GraphType;
	using GraphSecond = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleSecond>::GraphType;
	using VertexResult = typename boost::graph_traits<GraphResult>::vertex_descriptor;
	using EdgeResult = typename boost::graph_traits<GraphResult>::edge_descriptor;
	using VertexFirst = typename boost::graph_traits<GraphFirst>::vertex_descriptor;
	using EdgeSecond = typename boost::graph_traits<GraphSecond>::edge_descriptor;
public:

	CompositionHelper(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, Visitor visitor)
	: rFirst(rFirst), rSecond(rSecond), match(match), visitor(visitor) { }

	boost::optional<Result> operator()() && {
		Result result(rFirst, rSecond);
		bool resInit = visitor.template init<Verbose>(rFirst, rSecond, match, result);
		if(!resInit) return boost::none;
		// Vertices
		//--------------------------------------------------------------------------
		copyVerticesFirst(result);
		composeVerticesSecond(result);
		// Edges
		//--------------------------------------------------------------------------
		bool resFirst = copyEdgesFirstUnmatched(result);
		if(!resFirst) return boost::none;
		bool resSecond = composeEdgesSecond(result);
		if(!resSecond) return boost::none;

		// Finish it
		//--------------------------------------------------------------------------
		bool resFinal = visitor.template finalize<Verbose>(rFirst, rSecond, match, result);
		if(!resFinal) return boost::none;
		return boost::optional<Result>(std::move(result)); // something strange is going on with just "return result;"
	}
private:

	void copyVerticesFirst(Result &result) {
		if(Verbose) IO::log() << "copyVerticesFirst\n" << std::string(80, '-') << std::endl;
		auto &rResult = result.rResult;
		auto &gResult = get_graph(rResult);
		const auto &gFirst = get_graph(rFirst);
		const auto &gSecond = get_graph(rSecond);
		for(auto vFirst : asRange(vertices(gFirst))) {
			bool getsDeleted = false;
			if(membership(rFirst, vFirst) == Membership::Right) {
				auto vSecond = get_inverse(match, gSecond, gFirst, vFirst);
				if(vSecond != boost::graph_traits<GraphSecond>::null_vertex()) {
					if(membership(rSecond, vSecond) == Membership::Left) {
						getsDeleted = true;
					}
				}
			}
			if(getsDeleted) {
				put(result.mFirstToResult, gFirst, gResult, vFirst, boost::graph_traits<GraphResult>::null_vertex());
				if(Verbose) IO::log() << "gets deleted" << std::endl;
			} else {
				auto vResult = add_vertex(gResult);
				put(result.mFirstToResult, gFirst, gResult, vFirst, vResult);
				put_membership(rResult, vResult, membership(rFirst, vFirst));
				visitor.template copyVertexFirst<Verbose>(rFirst, rSecond, match, result, vFirst, vResult);
				if(Verbose) {
					IO::log() << "new node:\t"
							<< get(boost::vertex_index_t(), gResult, vResult)
							<< "(" << membership(rResult, vResult) << ")(";
					visitor.template printVertexResult(rFirst, rSecond, match, result, IO::log(), vResult);
					IO::log() << ")" << std::endl;
				}
			}
		}
	}

	void composeVerticesSecond(Result &result) {
		if(Verbose) IO::log() << "composeVerticesSecond\n" << std::string(80, '-') << std::endl;
		const auto &gFirst = get_graph(rFirst);
		const auto &gSecond = get_graph(rSecond);
		auto &rResult = result.rResult;
		auto &gResult = get_graph(rResult);
		// copy nodes from second, but compose the matched ones which has already been created
		for(auto vSecond : asRange(vertices(gSecond))) {
			if(Verbose) {
				IO::log() << "rSecond node:\t"
						<< get(boost::vertex_index_t(), gSecond, vSecond)
						<< "(" << membership(rSecond, vSecond) << ")"
						<< "(";
				visitor.template printVertexSecond(rFirst, rSecond, match, result, IO::log(), vSecond);
				IO::log() << ")" << std::endl;
			}
			// check if the vertex is matched
			auto vFirst = get(match, gSecond, gFirst, vSecond);
			if(vFirst == boost::graph_traits<GraphFirst>::null_vertex()) {
				// the vertex was not there
				auto vResult = add_vertex(gResult);
				put(result.mSecondToResult, gSecond, gResult, vSecond, vResult);
				put_membership(rResult, vResult, membership(rSecond, vSecond));
				visitor.template copyVertexSecond<Verbose>(rFirst, rSecond, match, result, vSecond, vResult);
				if(Verbose) {
					IO::log() << "new node:\t" << get(boost::vertex_index_t(), gResult, vResult)
							<< "(" << membership(rResult, vResult) << ")"
							<< "(";
					visitor.template printVertexResult(rFirst, rSecond, match, result, IO::log(), vResult);
					IO::log() << ")" << std::endl;
				}
			} else {
				auto vResult = get(result.mFirstToResult, gFirst, gResult, vFirst);
				put(result.mSecondToResult, gSecond, gResult, vSecond, vResult);
				if(vResult == boost::graph_traits<GraphResult>::null_vertex()) {
					if(Verbose) IO::log() << "deleted" << std::endl;
				} else {
					if(Verbose) {
						IO::log() << "match to:\t" << get(boost::vertex_index_t(), gResult, vResult)
								<< "(" << membership(rResult, vResult) << ")"
								<< "(";
						visitor.template printVertexResult(rFirst, rSecond, match, result, IO::log(), vResult);
						IO::log() << ")" << std::endl;
					}
					// now we calculate the new membership for the node
					auto mFirst = membership(rResult, vResult); // should be a copy of the one from rFirst
					auto mSecond = membership(rSecond, vSecond);
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
		}
	}

	bool copyEdgesFirstUnmatched(Result &result) {
		if(Verbose) IO::log() << "copyEdgesFirstUnmatched\n" << std::string(80, '-') << std::endl;
		const auto &gFirst = get_graph(rFirst);
		const auto &gSecond = get_graph(rSecond);
		auto &rResult = result.rResult;
		auto &gResult = get_graph(rResult);
		for(const auto eFirst : asRange(edges(gFirst))) {
			// map source and target to core vertices
			auto vFirstSrc = source(eFirst, gFirst);
			auto vFirstTar = target(eFirst, gFirst);
			auto meFirst = membership(rFirst, eFirst);

			if(Verbose) {
				auto vResultSrc = get(result.mFirstToResult, gFirst, gResult, vFirstSrc);
				auto vResultTar = get(result.mFirstToResult, gFirst, gResult, vFirstTar);
				// vResultSrc/vResultTar may be null_vertex
				IO::log() << "Edge first:\t(" << vFirstSrc << ", " << vFirstTar << ") copy to new (" << vResultSrc << ", " << vResultTar << ")"
						<< "(" << meFirst << ")"
						<< "(";
				visitor.template printEdgeFirst(rFirst, rSecond, match, result, IO::log(), eFirst);
				IO::log() << ")" << std::endl;
			}

			bool doCopy = false;
			if(meFirst == Membership::Left) {
				if(Verbose) IO::log() << "\teFirst in LEFT, clean copy" << std::endl;
				doCopy = true;
			} else {
				if(Verbose) IO::log() << "\teFirst in RIGHT or CONTEXT" << std::endl;
				auto vSecondSrc = get_inverse(match, gSecond, gFirst, vFirstSrc);
				auto vSecondTar = get_inverse(match, gSecond, gFirst, vFirstTar);
				bool isSrcMatched = vSecondSrc != boost::graph_traits<GraphSecond>::null_vertex();
				bool isTarMatched = vSecondTar != boost::graph_traits<GraphSecond>::null_vertex();
				if(isSrcMatched && isTarMatched) {
					if(Verbose) IO::log() << "\tBoth ends matched" << std::endl;
					auto edgesSecond = out_edges(vSecondSrc, gSecond);
					auto pred = [&gSecond, vSecondTar, this](EdgeSecond eSecond) {
						return target(eSecond, gSecond) == vSecondTar && membership(rSecond, eSecond) != Membership::Right;
					};
					auto eSecondIter = std::find_if(edgesSecond.first, edgesSecond.second, pred);
					// TODO: why do we check the membership in the find_if?
					// There should be only one edge, so we can bail out here, right?
					bool isEdgeMatched = eSecondIter != edgesSecond.second;
					if(isEdgeMatched) {
						auto eSecond = *eSecondIter;
						if(meFirst == Membership::Right) {
							if(Verbose) IO::log() << "\teFirst matched and in RIGHT, skipping" << std::endl;
						} else {
							if(Verbose) IO::log() << "\teFirst matched and in CONTEXT, copying to LEFT or CONTEXT (depending on eSecond ("
								<< membership(rSecond, eSecond) << "))" << std::endl;
							auto vResultSrc = get(result.mFirstToResult, gFirst, gResult, vFirstSrc);
							auto vResultTar = get(result.mFirstToResult, gFirst, gResult, vFirstTar);
							// vResultSrc/vResultTar can not be null_vertex
							assert(vResultSrc != boost::graph_traits<GraphResult>::null_vertex());
							assert(vResultTar != boost::graph_traits<GraphResult>::null_vertex());
							auto peResult = add_edge(vResultSrc, vResultTar, gResult);
							// adding shouldn't fail
							assert(peResult.second);
							auto eResult = peResult.first;
							// we use the second membership, so do not use the common copy mechanism in the bottom of the function
							put_membership(rResult, eResult, membership(rSecond, eSecond));
							visitor.template copyEdgeFirst<Verbose>(rFirst, rSecond, match, result, eFirst, eResult);
							assert(membership(rResult, vResultSrc) != Membership::Right);
							assert(membership(rResult, vResultTar) != Membership::Right);
						}
					} else { // eFirst not matched, the ends must be consistent and not deleted
						auto vResultSrc = get(result.mFirstToResult, gFirst, gResult, vFirstSrc);
						auto vResultTar = get(result.mFirstToResult, gFirst, gResult, vFirstTar);
						// vSrcNew/vTarNew may be null_vertex
						bool endPointDeleted =
								vResultSrc == boost::graph_traits<GraphResult>::null_vertex()
								|| vResultTar == boost::graph_traits<GraphResult>::null_vertex();
						if(endPointDeleted) {
							if(Verbose) IO::log() << "\tComposition failure: at least one matched vertex deleted" << std::endl;
							return false;
						} else { // check consistency
							auto mvResultSrc = membership(rResult, vResultSrc);
							auto mvResultTar = membership(rResult, vResultTar);
							if(mvResultSrc == Membership::Left || mvResultTar == Membership::Left) {
								if(Verbose) IO::log() << "\tComposition failure: at least one matched vertex has inconsistent context ("
									<< mvResultSrc << " and " << mvResultTar << "), eFirst is (" << meFirst << ")" << std::endl;
								return false;
							} else {
								if(Verbose) IO::log() << "\teFirst not matched and not dangling" << std::endl;
								doCopy = true;
							}
						}
					}
				} else { // at most 1 end matched
					if(isSrcMatched != isTarMatched) {
						if(Verbose) IO::log() << "\tOne end matched" << std::endl;
						auto vResultSrc = get(result.mFirstToResult, gFirst, gResult, vFirstSrc);
						auto vResultTar = get(result.mFirstToResult, gFirst, gResult, vFirstTar);
						// vResultSrc/vResultTar may be null_vertex
						auto vResultMatched = isSrcMatched ? vResultSrc : vResultTar;
						bool matchedDeleted = vResultMatched == boost::graph_traits<GraphResult>::null_vertex();
						if(matchedDeleted) {
							if(Verbose) IO::log() << "\tComposition failure: matched vertex deleted" << std::endl;
							return false;
						} else { // matched, is it consistent?
							auto mvResultMatched = membership(rResult, vResultMatched);
							if(mvResultMatched == Membership::Left) {
								if(Verbose) IO::log() << "\tComposition failure: matched vertex has inconsistent context ("
									<< mvResultMatched << "), eFirst is (" << meFirst << ")" << std::endl;
								return false;
							} else {
								if(Verbose) IO::log() << "\teFirst not matched and not dangling" << std::endl;
								doCopy = true;
							}
						}
					} else {
						if(Verbose) IO::log() << "\tNo ends matched" << std::endl;
						doCopy = true;
					}
				}
			}
			if(doCopy) {
				auto vResultSrc = get(result.mFirstToResult, gFirst, gResult, vFirstSrc);
				auto vResultTar = get(result.mFirstToResult, gFirst, gResult, vFirstTar);
				// vResultSrc/vResultTar should be valid at this point
				assert(vResultSrc != boost::graph_traits<GraphResult>::null_vertex());
				assert(vResultTar != boost::graph_traits<GraphResult>::null_vertex());
				if(Verbose) IO::log() << "\tCopy eFirst" << std::endl;
				auto peResult = add_edge(vResultSrc, vResultTar, gResult);
				// adding shouldn't fail
				assert(peResult.second);
				auto eResult = peResult.first;
				put_membership(rResult, eResult, membership(rFirst, eFirst));
				visitor.template copyEdgeFirst<Verbose>(rFirst, rSecond, match, result, eFirst, eResult);
				assert(membership(rResult, vResultSrc) == Membership::Context || membership(rResult, vResultSrc) == meFirst);
				assert(membership(rResult, vResultTar) == Membership::Context || membership(rResult, vResultTar) == meFirst);
			}
		} // for each edge
		return true;
	}

	bool composeEdgesSecond(Result &result) {
		if(Verbose) IO::log() << "composeEdgesSecond\n" << std::string(80, '-') << std::endl;
		const auto &gFirst = get_graph(rFirst);
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
				IO::log() << "Edge second:\t(" << vSecondSrc << ", " << vSecondTar << ")\tmapped to (" << vResultSrc << ", " << vResultTar << ")"
						<< ", (" << membership(rSecond, eSecond) << ")"
						<< "(";
				visitor.template printEdgeSecond(rFirst, rSecond, match, result, IO::log(), eSecond);
				IO::log() << ")" << std::endl;
			}
			// vResultSrc/vResultTar may be null_vertex
			// check for match on vFirstSrc and vFirstTar
			auto vFirstSrc = get(match, gSecond, gFirst, vSecondSrc);
			auto vFirstTar = get(match, gSecond, gFirst, vSecondTar);
			bool isSrcMatched = vFirstSrc != boost::graph_traits<GraphFirst>::null_vertex();
			bool isTarMatched = vFirstTar != boost::graph_traits<GraphFirst>::null_vertex();
			if(!isSrcMatched || !isTarMatched) { // new edge
				if(!isSrcMatched && !isTarMatched) {
					if(Verbose) IO::log() << "No ends matched. Copy eSecond." << std::endl;
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
					bool isOk = composeEdgeSecond_oneEndMatched(result, eSecond, vResultSrc, vResultTar, isSrcMatched, isTarMatched);
					if(!isOk) return false;
				} // if 0 or 1 end matched
			} else { // maybe a matched edge
				bool isOk = composeEdgeSecond_bothEndsMatched(result, eSecond, vFirstSrc, vFirstTar, vResultSrc, vResultTar);
				if(!isOk) return false;
			} // end if 0 or 1 matched endNodes
		} // end for each eSecond
		return true;
	}
private:

	bool composeEdgeSecond_oneEndMatched(Result &result, EdgeSecond eSecond, VertexResult vResultSrc, VertexResult vResultTar,
			bool isSrcMatched, bool isTarMatched) {
		auto &rResult = result.rResult;
		auto &gResult = get_graph(rResult);
		if(Verbose) IO::log() << "One end matched, eSecond should be copied" << std::endl;
		assert(isSrcMatched ^ isTarMatched);
		// at most one of vResultSrc and vResultTar may be null_vertex
		assert(vResultSrc != boost::graph_traits<GraphResult>::null_vertex()
				|| vResultTar != boost::graph_traits<GraphResult>::null_vertex());
		auto vResultMatched = isSrcMatched ? vResultSrc : vResultTar;
		auto vResultOther = isSrcMatched ? vResultTar : vResultSrc;
		// vResultMatched may be null_vertex, if it's a RIGHT vs. LEFT vertex
		assert(vResultOther != boost::graph_traits<GraphResult>::null_vertex());
		auto meSecond = membership(rSecond, eSecond);
		auto mvResultOther = membership(rResult, vResultOther);
		assert(mvResultOther == Membership::Context || mvResultOther == meSecond);

		if(vResultMatched == boost::graph_traits<GraphResult>::null_vertex()) {
			if(Verbose) IO::log() << "\tComposition failure: matched vertex deleted" << std::endl;
			return false;
		}

		auto mvResultMatched = membership(rResult, vResultMatched);
		if(mvResultMatched != Membership::Context && mvResultMatched != meSecond) {
			if(Verbose) IO::log() << "\tComposition failure: matched vertex has inconsistent context ("
				<< mvResultMatched << "), eSecond is (" << meSecond << ")" << std::endl;
			return false;
		}

		if(Verbose) IO::log() << "\tCopy edge" << std::endl;
		auto peResult = add_edge(vResultSrc, vResultTar, gResult);
		// adding shouldn't fail
		assert(peResult.second);
		auto eResult = peResult.first;
		put_membership(rResult, eResult, membership(rSecond, eSecond));
		visitor.template copyEdgeSecond<Verbose>(rFirst, rSecond, match, result, eSecond, eResult);
		return true;
	}

	bool composeEdgeSecond_bothEndsMatched(Result &result, EdgeSecond eSecond, VertexFirst vFirstSrc, VertexFirst vFirstTar,
			VertexResult vResultSrc, VertexResult vResultTar) {
		const auto &gFirst = get_graph(rFirst);
		auto &rResult = result.rResult;
		auto &gResult = get_graph(rResult);
		if(Verbose) IO::log() << "Both ends matched" << std::endl;
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
			if(Verbose) IO::log() << "\tSimple copy of eSecond" << std::endl;
			if(meSecond == Membership::Left) {
				if(Verbose) IO::log() << "\t\teSecond in LEFT, check ends" << std::endl;
				// vResultSrc/vResultTar may be null_vertex
				if(vResultSrc == boost::graph_traits<GraphResult>::null_vertex()) {
					if(Verbose) IO::log() << "\tComposition failure: vResultSrc deleted" << std::endl;
					return false;
				}
				if(vResultTar == boost::graph_traits<GraphResult>::null_vertex()) {
					if(Verbose) IO::log() << "\tComposition failure: vResultTar deleted" << std::endl;
					return false;
				}
				if(membership(rResult, vResultSrc) == Membership::Right) {
					if(Verbose) IO::log() << "\tComposition failure: vResultSrc not in LEFT or CONTEXT" << std::endl;
					return false;
				}
				if(membership(rResult, vResultTar) == Membership::Right) {
					if(Verbose) IO::log() << "\tComposition failure: vResultTar not in LEFT or CONTEXT" << std::endl;
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
			if(Verbose) IO::log() << "\tComposition failure: duplicate edge in L" << std::endl;
			return false;
		}
		if(meFirst != Membership::Left && meSecond == Membership::Right) {
			// creating parallel in right
			//    -> | vs.    -> |
			// |  -> | vs.    -> |
			if(Verbose) IO::log() << "\tComposition failure: eSecond in RIGHT, eFirst in CONTEXT or RIGHT, i.e., parallel edge in R" << std::endl;
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
			if(Verbose) IO::log() << "\teSecond matched in (" << meSecond << ")"
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
			if(Verbose) IO::log() << "\t   -> | vs. |  -> |, copy to RIGHT" << std::endl;
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
		if(Verbose) IO::log() << "\t'|  ->   vs.    -> |' or '|  -> | vs. |  -> |', promote eNew to CONTEXT and set right from second right" << std::endl;
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