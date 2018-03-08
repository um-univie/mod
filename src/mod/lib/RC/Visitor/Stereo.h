#ifndef MOD_LIB_RC_VISITOR_STEREO_H
#define MOD_LIB_RC_VISITOR_STEREO_H

#include <mod/lib/GraphMorphism/StereoVertexMap.h>
#include <mod/lib/RC/Visitor/Compound.h>
#include <mod/lib/Rules/Properties/Stereo.h>
#include <mod/lib/Stereo/Inference.h>

#include <vector>

// TODO: the edge categories are probably not instantiated correctly if the Any category is used

namespace mod {
namespace lib {
namespace RC {
namespace Visitor {

// During the algorithm we:
// - allocate space in the data arrays
// - instantiate and compose edge categories
// - keep track of 'inContext' flags
// In finalization we:
// - instantiate and compose geometries
// - instantiate and compose embeddings

struct Stereo {
	using Membership = jla_boost::GraphDPO::Membership;

	template<typename G>
	static auto NullVertex() {
		return boost::graph_traits<G>::null_vertex();
	}

	template<typename G>
	static auto NullVertex(const G &g) {
		return boost::graph_traits<G>::null_vertex();
	}
public:
	std::vector<lib::Stereo::detail::InferenceBase::VertexData> vDataLeft, vDataRight;
	std::vector<lib::Stereo::EdgeCategory> eDataLeft, eDataRight;
	std::vector<bool> vertexInContext, edgeInContext;
private:

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename VertexSecond>
	auto getVertexFirstChecked(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const VertexSecond &vSecond) const {
		const auto &gCodom = get_graph(get_labelled_right(rFirst));
		const auto &gDom = get_graph(get_labelled_left(rSecond));
		const auto m = membership(rSecond, vSecond);
		if(m == Membership::Right) return NullVertex(gDom);
		else return get(match, gDom, gCodom, vSecond);
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename VertexFirst>
	auto getVertexSecondChecked(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const VertexFirst &vFirst) const {
		const auto &gCodom = get_graph(get_labelled_right(rFirst));
		const auto &gDom = get_graph(get_labelled_left(rSecond));
		const auto m = membership(rFirst, vFirst);
		if(m == Membership::Left) return NullVertex(gCodom);
		else return get_inverse(match, gDom, gCodom, vFirst);
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result>
	bool init(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result) {
		return true;
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result>
	bool finalize(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result) {
		const auto &gFirst = get_graph(rFirst);
		const auto &gSecond = get_graph(rSecond);
		const auto &gResult = get_graph(result.rResult);
		using GraphFirst = typename std::decay < decltype(gFirst)>::type;
		using GraphSecond = typename std::decay < decltype(gSecond)>::type;
		const auto getGeoName = [](const auto &vGeo) -> const std::string& {
			return lib::Stereo::getGeometryGraph().getGraph()[vGeo].name;
		};
		const auto copyAllFromSide = [&](
				const auto safe,
				const auto &glSide, const auto &vInput, const auto &gInput,
				const auto &mInputToResult, const auto &vResult,
				const auto &gResultSide, auto &vData, const auto &eData) {
			bool partial = false;
			const auto vResultId = get(boost::vertex_index_t(), gResult, vResult);
			auto &data = vData[vResultId];
			const auto &conf = *get_stereo(glSide)[vInput];
			data.vGeometry = conf.getGeometryVertex();
			if(Verbose) IO::log() << "\tGeometry: " << getGeoName(data.vGeometry) << "\n";
			for(const auto &emb : conf) {
				switch(emb.type) {
				case lib::Stereo::EmbeddingEdge::Type::LonePair:
					// offsets should be corrected somewhere else
					data.edges.emplace_back(-1, lib::Stereo::EmbeddingEdge::Type::LonePair, emb.cat);
					break;
				case lib::Stereo::EmbeddingEdge::Type::Radical:
					MOD_ABORT; // TODO
					// offsets should be corrected somewhere else
					data.edges.emplace_back(-1, lib::Stereo::EmbeddingEdge::Type::Radical, emb.cat);
					break;
				case lib::Stereo::EmbeddingEdge::Type::Edge:
					const auto eInput = emb.getEdge(vInput, get_graph(glSide));
					const auto vAdjInput = target(eInput, gInput);
					if(Verbose) {
						IO::log() << "\tmapping edge: ("
								<< get(boost::vertex_index_t(), gInput, vInput) << ", "
								<< get(boost::vertex_index_t(), gInput, vAdjInput) << ")\n";
					}
					const auto vAdjResult = get(mInputToResult, gInput, gResult, vAdjInput);
					if(safe) assert(vAdjResult != NullVertex(gResult));
					else if(vAdjResult == NullVertex(gResult)) {
						// the vertex is deleted, so let's skip it
						if(Verbose) IO::log() << "\tdeleted\n";
						partial = true;
						break; // the case statement
					}
					// now find the corresponding edge in result
					const auto eResultOffset = [&]() {
						const auto oeResult = out_edges(vResult, gResultSide);
						const auto oeIter = std::find_if(oeResult.first, oeResult.second, [&](const auto &eResultCand) {
							return target(eResultCand, gResult) == vAdjResult;
						});
						assert(oeIter != oeResult.second);
						return std::distance(oeResult.first, oeIter);
					}();
					if(Verbose) {
						IO::log() << "\tto edge: ("
								<< get(boost::vertex_index_t(), gResult, vResult) << ", "
								<< get(boost::vertex_index_t(), gResult, vAdjResult) << "), offset = "
								<< eResultOffset << " (of " << out_degree(vResult, gResultSide) << ")\n";
					}
					const auto eResult = out_edges(vResult, gResult).first[eResultOffset];
					const auto eIdResult = get(boost::edge_index_t(), gResult, eResult);
					const auto cat = eData[eIdResult];
					data.edges.emplace_back(eResultOffset, lib::Stereo::EmbeddingEdge::Type::Edge, cat);
					break;
				} // end switch(emb.type)
			} // end foreach emb in conf
			// handle fixation
			if(partial) { // let it remain free
				MOD_ABORT; // TODO
				assert(data.fix == lib::Stereo::Fixation::free());
				if(Verbose) IO::log() << "\tfix: remain free (" << data.fix << ")\n";
			} else { // copy from conf
				data.fix = conf.getFixation();
				if(Verbose) IO::log() << "\tfix: copy (" << data.fix << ")\n";
			}
			return partial;
		};
		const auto handleOnly = [&](auto vResult, auto vInput, const auto &rInput, const auto &mInputToResult) {
			const auto &gInput = get_graph(rInput);
			// yay, just copy the embedding, both in the right and left side
			const auto m = membership(result.rResult, vResult);
			if(m != Membership::Right) {
				// copy left
				if(Verbose) IO::log() << "\tLeft:\n";
				const bool partial = copyAllFromSide(std::true_type(), get_labelled_left(rInput), vInput, gInput, mInputToResult, vResult, get_left(result.rResult), vDataLeft, eDataLeft);
				assert(!partial);
			}
			if(m != Membership::Left) {
				// copy right
				if(Verbose) IO::log() << "\tRight:\n";
				const bool partial = copyAllFromSide(std::true_type(), get_labelled_right(rInput), vInput, gInput, mInputToResult, vResult, get_right(result.rResult), vDataRight, eDataRight);
				assert(!partial);
			}
		};
		const auto handleBoth = [&](auto vResult, auto vFirst, auto vSecond) {
			using EmbEdge = lib::Stereo::EmbeddingEdge;
			//			const auto &geo = lib::Stereo::getGeometryGraph();
			const auto m = membership(result.rResult, vResult);
			const auto vResultId = get(boost::vertex_index_t(), gResult, vResult);
			const auto &gR1 = get_right(rFirst);
			const auto &gL2 = get_left(rSecond);
			const auto &confR1 = *get_stereo(get_labelled_right(rFirst))[vFirst];
			const auto &confL2 = *get_stereo(get_labelled_left(rSecond))[vSecond];
			// Let's firs tmake a map of the offsets to each other.
			// 0 <= i < out_degree is the offsets for the edges.
			// out_degree <= i < conf.degree is the virtual edges.
			std::vector<std::size_t>
					offsetFirstToSecond(confR1.degree(), -1),
					offsetSecondToFirst(confL2.degree(), -1);
			{ // heuristically we 'usually' have gSecondL as a subgraph of gFirstR
				std::size_t offsetFirst = 0;
				for(auto oeFirst = out_edges(vFirst, gR1); oeFirst.first != oeFirst.second; ++oeFirst.first, (void) ++offsetFirst) {
					const auto eFirst = *oeFirst.first;
					const auto vAdjFirst = target(eFirst, gR1);
					const auto vAdjSecond = get_inverse(match, gL2, gR1, vAdjFirst);
					if(vAdjSecond == NullVertex(gSecond)) continue;
					std::size_t offsetSecond = 0;
					for(auto oeSecond = out_edges(vSecond, gL2); oeSecond.first != oeSecond.second; ++oeSecond.first, (void) ++offsetSecond) {
						const auto eSecondCand = *oeSecond.first;
						if(target(eSecondCand, gL2) == vAdjSecond) {
							offsetFirstToSecond[offsetFirst] = offsetSecond;
							offsetSecondToFirst[offsetSecond] = offsetFirst;
							break;
						}
					}
				}
			} // edge mapping
			// If both have virtuals, we should try to merge them, otherwise we can still claim subgraphness.
			const auto firstNumVirtuals = offsetFirstToSecond.size() - out_degree(vFirst, gR1);
			const auto secondNumVirtuals = offsetSecondToFirst.size() - out_degree(vSecond, gL2);
			if(firstNumVirtuals > 0 && secondNumVirtuals > 0) {

				// TODO: how do we merge virtuals?
				MOD_ABORT;
			}
			const bool firstToSecondSubgraph = std::none_of(offsetFirstToSecond.begin(), offsetFirstToSecond.end(), [&](auto o) {
				return o == -1;
			});
			const bool secondToFirstSubgraph = std::none_of(offsetSecondToFirst.begin(), offsetSecondToFirst.end(), [&](auto o) {
				return o == -1;
			});
			const auto geoR1 = confR1.getGeometryVertex();
			const auto geoL2 = confL2.getGeometryVertex();

			if(m != Membership::Right) {
				assert(membership(rFirst, vFirst) == Membership::Context);
				//				auto &data = vDataLeft[vResultId];
				//				const auto &eData = eDataLeft;
				const auto firstInContext = get_stereo(rFirst).inContext(vFirst);
				const auto &confL1 = *get_stereo(get_labelled_left(rFirst))[vFirst];
				const auto geoL1 = confL1.getGeometryVertex();
				if(Verbose)
					IO::log() << "\tHandling L\n"
					<< "\t\tGeo L1: " << getGeoName(geoL1) << "\n"
					<< "\t\tGeo R1: " << getGeoName(geoR1) << "\n"
					<< "\t\tGeo L2: " << getGeoName(geoL2) << "\n";
				if(firstInContext) {
					if(Verbose) IO::log() << "\t\tFirst stereo in context\n";
					if(secondToFirstSubgraph) {
						if(Verbose) IO::log() << "\t\tSecond-to-first subgraph: copy and map L1/R1 to L\n";
						const bool partial = copyAllFromSide(std::true_type(), get_labelled_left(rFirst), vFirst, gFirst, result.mFirstToResult, vResult, get_left(result.rResult), vDataLeft, eDataLeft);
						assert(!partial);
					} else if(firstToSecondSubgraph) {
						if(Verbose) IO::log() << "\t\tFirst-to-second subgraph: copy and map L2 to L\n";
						MOD_ABORT;
					} else {
						if(Verbose) IO::log() << "\t\tNon-subgraph: do a merge\n";
						MOD_ABORT;
					}
				} else { // !firstInContext
					if(Verbose) IO::log() << "\t\tFirst stereo changes\n";
					if(secondToFirstSubgraph) {
						if(Verbose) IO::log() << "\t\tSecond-to-first subgraph: copy and map L1 to L\n";
						const bool partial = copyAllFromSide(std::true_type(), get_labelled_left(rFirst), vFirst, gFirst, result.mFirstToResult, vResult, get_left(result.rResult), vDataLeft, eDataLeft);
						assert(!partial);
					} else if(firstToSecondSubgraph) {
						if(Verbose) IO::log() << "\t\tFirst-to-second subgraph: hmm\n";
						MOD_ABORT;
					} else {
						if(Verbose) IO::log() << "\t\tNon-subgraph: do a merge\n";
						MOD_ABORT;
					}
				}
			}
			if(m != Membership::Left) {
				assert(membership(rSecond, vSecond) == Membership::Context);
				auto &data = vDataRight[vResultId];
				const auto &eData = eDataRight;
				const auto secondInContext = get_stereo(rSecond).inContext(vSecond);
				const auto &confR2 = *get_stereo(get_labelled_right(rSecond))[vSecond];
				const auto geoR2 = confR2.getGeometryVertex();
				if(Verbose)
					IO::log() << "\tHandling R\n"
					<< "\t\tGeo R1: " << getGeoName(geoR1) << "\n"
					<< "\t\tGeo L2: " << getGeoName(geoL2) << "\n"
					<< "\t\tGeo R2: " << getGeoName(geoR2) << "\n";
				if(secondInContext) {
					if(Verbose) IO::log() << "\t\tSecond stereo in context\n";
					if(firstToSecondSubgraph) {
						if(Verbose) IO::log() << "\t\tFirst-to-second subgraph: copy and map L2/R2 to R\n";
						const bool partial = copyAllFromSide(std::true_type(), get_labelled_right(rSecond), vSecond, gSecond, result.mSecondToResult, vResult, get_right(result.rResult), vDataRight, eDataRight);
						assert(!partial);
					} else if(secondToFirstSubgraph) {
						if(Verbose) IO::log() << "\t\tSecond-to-first subgraph: copy and map R1 to R\n";
						const bool partial = copyAllFromSide(std::true_type(), get_labelled_right(rFirst), vFirst, gFirst, result.mFirstToResult, vResult, get_right(result.rResult), vDataRight, eDataRight);
						assert(!partial);
					} else {
						if(Verbose) IO::log() << "\t\tNon-subgraph: do a merge\n";
						MOD_ABORT;
					}
				} else { // !secondInContext
					if(Verbose) IO::log() << "\t\tSecond stereo changes\n";
					if(firstToSecondSubgraph) {
						if(Verbose) IO::log() << "\t\tFirst-to-second subgraph: copy and map R2 to R\n";
						const bool partial = copyAllFromSide(std::true_type(), get_labelled_right(rSecond), vSecond, gSecond, result.mSecondToResult, vResult, get_right(result.rResult), vDataRight, eDataRight);
						assert(!partial);
					} else if(secondToFirstSubgraph) {
						if(Verbose) {
							IO::log() << "\t\tSecond-to-first subgraph:\n"
									<< "\t\t\t- Copy all from R2.\n"
									<< "\t\t\t- Copy unmatched from R1.\n"
									<< "\t\t\t- Match R2 stereo onto the result and check if the pushout is valid.\n"
									;
							IO::log() << "\tCopying all from R2\n";
						}
						const bool partial = copyAllFromSide(std::true_type(), get_labelled_right(rSecond), vSecond, gSecond, result.mSecondToResult, vResult, get_right(result.rResult), vDataRight, eDataRight);
						assert(!partial);
						const auto sizeAfterR2 = data.edges.size();
						{ // copy unmatched from R1
							if(Verbose) IO::log() << "\tCopying unmatched from R1\n";
							const auto vInput = vFirst;
							const auto &gBaseInput = get_graph(rFirst);
							const auto &glSide = get_labelled_right(rFirst);
							const auto &glSideOther = get_labelled_left(rSecond);
							const auto &gResultSide = get_right(result.rResult);
							const auto &mInputToResult = result.mFirstToResult;
							const auto mapToOtherInput = [&](const auto &vFirst) {
								// the "this->" part is needed to get GCC to not seg. fault
								return this->getVertexSecondChecked(rFirst, rSecond, match, vFirst);
							};
							// the rest should be side invariant
							const auto &conf = *get_stereo(glSide)[vInput];
							for(const auto &emb : conf) {
								switch(emb.type) {
								case lib::Stereo::EmbeddingEdge::Type::LonePair:
								case lib::Stereo::EmbeddingEdge::Type::Radical:
									MOD_ABORT;
									break;
								case lib::Stereo::EmbeddingEdge::Type::Edge:
									const auto &gInput = get_graph(glSide);
									const auto &gInputOther = get_graph(glSideOther);
									const auto eInput = emb.getEdge(vInput, gInput);
									const auto vAdjInput = target(eInput, gInput);
									if(Verbose) {
										IO::log() << "\tmapping edge: ("
												<< get(boost::vertex_index_t(), gInput, vInput) << ", "
												<< get(boost::vertex_index_t(), gInput, vAdjInput) << ")\n";
									}
									const auto vAdjResult = get(mInputToResult, gBaseInput, gResult, vAdjInput);
									if(vAdjResult == NullVertex(gResult)) {
										// the vertex is deleted, so let's skip it
										if(Verbose) IO::log() << "\tdeleted\n";
										break; // the case statement
									}
									// is it mapped?
									const auto vInputOther = mapToOtherInput(vInput);
									const auto vAdjInputOther = mapToOtherInput(vAdjInput);
									const bool isMatched = [&]() {
										if(vInputOther == NullVertex(gInput)) {
											if(Verbose) IO::log() << "\tnot matched, due to vInputOther = null\n";
											return false;
										}
										if(vAdjInputOther == NullVertex(gInput)) {
											if(Verbose) IO::log() << "\tnot matched, due to vAdjInputOther = null\n";
											return false;
										}
										for(auto eInputOther : asRange(out_edges(vInputOther, gInputOther))) {
											if(target(eInputOther, gInputOther) != vAdjInputOther) {
												if(Verbose) {
													IO::log() << "\tcand = ("
															<< get(boost::vertex_index_t(), gInputOther, vInputOther) << ", "
															<< get(boost::vertex_index_t(), gInputOther, vAdjInputOther) << ") not it\n";
												}
												continue;
											}
											if(Verbose) {
												IO::log() << "\tcand = ("
														<< get(boost::vertex_index_t(), gInputOther, vInputOther) << ", "
														<< get(boost::vertex_index_t(), gInputOther, vAdjInputOther) << ") is it\n";
											}
											// TODO: shouldn't we check the membership as well?
											return true;
										}
										if(Verbose) IO::log() << "\tnot matched, due to no edge found\n";
										return false;
									}();
									if(isMatched) {
										if(Verbose) IO::log() << "\tmatched\n";
										break;
									}

									// now find the corresponding edge in result
									const auto eResultOffset = [&]() {
										const auto oeResult = out_edges(vResult, gResultSide);
										const auto oeIter = std::find_if(oeResult.first, oeResult.second, [&](const auto &eResultCand) {
											return target(eResultCand, gResult) == vAdjResult;
										});
										assert(oeIter != oeResult.second);
										return std::distance(oeResult.first, oeIter);
									}();
									if(Verbose) {
										IO::log() << "\tto edge: ("
												<< get(boost::vertex_index_t(), gResult, vResult) << ", "
												<< get(boost::vertex_index_t(), gResult, vAdjResult) << "), offset = "
												<< eResultOffset << " (of " << out_degree(vResult, gResultSide) << ")\n";
									}
									const auto eResult = out_edges(vResult, gResult).first[eResultOffset];
									const auto eIdResult = get(boost::edge_index_t(), gResult, eResult);
									const auto cat = eData[eIdResult];
									data.edges.emplace_back(eResultOffset, lib::Stereo::EmbeddingEdge::Type::Edge, cat);
									break;
								} // switch(emb.type)
							} // for each(emb in conf)
						} // end of copy all unmatched R1
						// now map the R2 geometry down to the embedding
						assert(sizeAfterR2 < data.edges.size()); // otherwise it would have been first-to-second subgraph
						if(data.vGeometry != lib::Stereo::getGeometryGraph().any) {
							MOD_ABORT; // bah, we need to do something, or reject the pushout
						}
					} else {
						if(Verbose) IO::log() << "\t\tNon-subgraph: do a merge\n";
						MOD_ABORT;
					}
				}
				if(false) {
					copyAllFromSide(std::false_type(), get_labelled_right(rSecond), vSecond, gSecond, result.mSecondToResult, vResult, get_right(result.rResult), vDataRight, eDataRight);
					const auto prevEmbSize = data.edges.size();

					if(data.edges.size() > prevEmbSize) {
						// we the fixation must be free
						if(Verbose) IO::log() << "\tfix: data.edges.size() = " << data.edges.size() << " > " << prevEmbSize << " = prevEmbSize, so set free (was " << data.fix << ")\n";
						data.fix = lib::Stereo::Fixation::free();
					} else {
						if(Verbose) IO::log() << "\tfix: not changing it (" << data.fix << ")\n";
					}
				} // if false, old code
			} // if vResult in R
		}; // handleBoth()
		if(Verbose) IO::log() << "Stereo Finalization\n" << std::string(80, '-') << '\n';
		const auto &gGeometry = lib::Stereo::getGeometryGraph().getGraph();
		for(auto vResult : asRange(vertices(gResult))) {
			if(Verbose) IO::log() << "Result vertex: " << get(boost::vertex_index_t(), gResult, vResult) << "\n";
			const auto m = membership(result.rResult, vResult);
			const auto vResultId = get(boost::vertex_index_t(), gResult, vResult);
			// If vResult is in only first or only second, we should be able to just copy the embedding.
			const auto vFirst = get_inverse(result.mFirstToResult, gFirst, gResult, vResult);
			const auto vSecond = get_inverse(result.mSecondToResult, gSecond, gResult, vResult);
			assert(vFirst != NullVertex<GraphFirst>() || vSecond != NullVertex<GraphSecond>());
			std::stringstream ssErr;
			const auto instantiateConfs = [&]() {
				if(Verbose) IO::log() << "\tinstantiating configurations\n";
				// TODO: we should probably correct LonePair and Radical offsets here
				if(m != Membership::Right) {
					auto &data = vDataLeft[vResultId];
					data.configuration = gGeometry[data.vGeometry].constructor(
							data.edges.data(), data.edges.data() + data.edges.size(), data.fix, ssErr);
					if(!data.configuration) {
						IO::log() << "Error in configuration construction in L.\n";
						IO::log() << ssErr.str();
						MOD_ABORT;
					}
				}
				if(m != Membership::Left) {
					auto &data = vDataRight[vResultId];
					data.configuration = gGeometry[data.vGeometry].constructor(
							data.edges.data(), data.edges.data() + data.edges.size(), data.fix, ssErr);
					if(!data.configuration) {
						IO::log() << "Error in configuration construction in R.\n";
						IO::log() << ssErr.str();
						MOD_ABORT;
					}
				}
			};
			if(vFirst == NullVertex<GraphFirst>()) {
				if(Verbose) IO::log() << "\tnot in First, copy only from Second\n";
				handleOnly(vResult, vSecond, rSecond, result.mSecondToResult);
				instantiateConfs();
			} else if(vSecond == NullVertex<GraphSecond>()) {
				if(Verbose) IO::log() << "\tnot in Second, copy only from First\n";
				handleOnly(vResult, vFirst, rFirst, result.mFirstToResult);
				instantiateConfs();
			} else {
				if(Verbose) IO::log() << "\tin both\n";
				handleBoth(vResult, vFirst, vSecond);
				instantiateConfs();
			}
		}

		using GraphResult = typename std::decay < decltype(gResult)>::type;
		using ResultVertex = typename boost::graph_traits<GraphResult>::vertex_descriptor;
		using ResultEdge = typename boost::graph_traits<GraphResult>::edge_descriptor;

		struct Inf {
			const GraphResult &g;
			std::vector<lib::Stereo::detail::InferenceBase::VertexData> vData;
			std::vector<lib::Stereo::EdgeCategory> eData;
		public:

			std::unique_ptr<lib::Stereo::Configuration> extractConfiguration(const ResultVertex &v) {
				auto vId = get(boost::vertex_index_t(), g, v);
				assert(vData[vId].configuration);
				return std::move(vData[vId].configuration);
			}

			lib::Stereo::EdgeCategory getEdgeCategory(const ResultEdge &e) const {
				auto eId = get(boost::edge_index_t(), g, e);
				assert(eId < num_edges(g));
				return eData[eId];
			}
		};
		const auto vertexInContext = [this, &gResult](const auto &v) -> bool {
			return this->vertexInContext[get(boost::vertex_index_t(), gResult, v)];
		};
		const auto edgeInContext = [this, &gResult](const auto &e) -> bool {
			return this->edgeInContext[get(boost::edge_index_t(), gResult, e)];
		};
		result.rResult.pStereo.reset(new lib::Rules::PropStereoCore(gResult,
				Inf{gResult, std::move(vDataLeft), std::move(eDataLeft)}, Inf{gResult, std::move(vDataRight), std::move(eDataRight)},
		vertexInContext, edgeInContext));
		return true;
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexFirst, typename VertexResult>
	void copyVertexFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const VertexFirst &vFirst, const VertexResult &vResult) {
		const auto &gResult = get_graph(result.rResult);
		const auto vResultId = get(boost::vertex_index_t(), gResult, vResult);
		assert(vDataLeft.size() + 1 == num_vertices(gResult));
		assert(vDataLeft.size() == vDataRight.size());
		assert(vDataLeft.size() == vResultId);
		assert(vertexInContext.size() == vDataLeft.size());
		vDataLeft.emplace_back();
		vDataRight.emplace_back();
		vertexInContext.push_back(get_stereo(rFirst).inContext(vFirst));
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexSecond, typename VertexResult>
	void copyVertexSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const VertexSecond &vSecond, const VertexResult &vResult) {
		const auto &gResult = get_graph(result.rResult);
		const auto vResultId = get(boost::vertex_index_t(), gResult, vResult);
		assert(vDataLeft.size() + 1 == num_vertices(gResult));
		assert(vDataLeft.size() == vDataRight.size());
		assert(vDataLeft.size() == vResultId);
		assert(vertexInContext.size() == vDataLeft.size());
		vDataLeft.emplace_back();
		vDataRight.emplace_back();
		vertexInContext.push_back(get_stereo(rSecond).inContext(vSecond));
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeFirst, typename EdgeResult>
	void copyEdgeFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const EdgeFirst &eFirst, const EdgeResult &eResult) {
		using GraphSecond = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleSecond>::GraphType;
		const auto m = membership(rFirst, eFirst);
		const auto &gFirst = get_graph(rFirst);
		const auto &gSecond = get_graph(rSecond);
		const auto &gResult = get_graph(result.rResult);
		const auto eResultId = get(boost::edge_index_t(), gResult, eResult);
		assert(eDataLeft.size() + 1 == num_edges(gResult));
		assert(eDataLeft.size() == eDataRight.size());
		assert(eDataLeft.size() == eResultId);
		assert(edgeInContext.size() == eDataLeft.size());
		eDataLeft.emplace_back();
		eDataRight.emplace_back();
		edgeInContext.push_back(get_stereo(rFirst).inContext(eFirst));
		if(m != Membership::Right) {
			// copy from rFirst left
			eDataLeft[eResultId] = get_stereo(get_labelled_left(rFirst))[eFirst];
		}
		if(m != Membership::Left) {
			// copy from the epimorphic overlap
			// if the edge is matched, get data from the unifier
			// otherwise from rFirst
			const auto vSecondSrc = getVertexSecondChecked(rFirst, rSecond, match, source(eFirst, gFirst));
			const auto vSecondTar = getVertexSecondChecked(rFirst, rSecond, match, target(eFirst, gFirst));
			const bool isMatched = [&]() {
				if(vSecondSrc == NullVertex<GraphSecond>()) return false;
				if(vSecondTar == NullVertex<GraphSecond>()) return false;
				for(auto eSecond : asRange(out_edges(vSecondSrc, gSecond))) {
					if(target(eSecond, gSecond) != vSecondTar) continue;
					// TODO: shouldn't we check the membership as well?
					return true;
				}
				return false;
			}();
			if(!isMatched) {
				eDataRight[eResultId] = get_stereo(get_labelled_right(rFirst))[eFirst];
			} else {
				auto data = get_prop(lib::GraphMorphism::StereoDataT(), match);
				eDataRight[eResultId] = get_edge_category(data, get_labelled_left(rSecond), get_labelled_right(rFirst), eFirst);
			}
		}
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeSecond, typename EdgeResult>
	void copyEdgeSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const EdgeSecond &eSecond, const EdgeResult &eResult) {
		using GraphFirst = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleFirst>::GraphType;
		const auto m = membership(result.rResult, eResult);
		const auto &gFirst = get_graph(rFirst);
		const auto &gSecond = get_graph(rSecond);
		const auto &gResult = get_graph(result.rResult);
		const auto eResultId = get(boost::edge_index_t(), gResult, eResult);
		assert(eDataLeft.size() + 1 == num_edges(gResult));
		assert(eDataLeft.size() == eDataRight.size());
		assert(eDataLeft.size() == eResultId);
		assert(edgeInContext.size() == eDataLeft.size());
		eDataLeft.emplace_back();
		eDataRight.emplace_back();
		edgeInContext.push_back(get_stereo(rSecond).inContext(eSecond));
		if(m != Membership::Right) {
			// copy from the epimorphic overlap
			// if the edge is matched, get data from the unifier
			// otherwise from rSecond
			const auto vFirstSrc = getVertexFirstChecked(rFirst, rSecond, match, source(eSecond, gSecond));
			const auto vFirstTar = getVertexFirstChecked(rFirst, rSecond, match, target(eSecond, gSecond));
			const bool isMatched = [&]() {
				if(vFirstSrc == NullVertex<GraphFirst>()) return false;
				if(vFirstTar == NullVertex<GraphFirst>()) return false;
				for(auto eFirst : asRange(out_edges(vFirstSrc, gFirst))) {
					if(target(eFirst, gFirst) != vFirstTar) continue;
					// TODO: shouldn't we check the membership as well?
					return true;
				}
				return false;
			}();
			if(isMatched) {
				MOD_ABORT; // this can't actually happen, right?
				//				eDataLeft[eResultId] = get_edge_category(match, get_labelled_left(rSecond), get_labelled_right(rFirst), eFirst);
			} else {
				eDataLeft[eResultId] = get_stereo(get_labelled_left(rSecond))[eSecond];
			}
		}
		if(m != Membership::Left) {
			// copy from rSecond right
			eDataRight[eResultId] = get_stereo(get_labelled_right(rSecond))[eSecond];
		}
	}
private:

	void printData(std::ostream &s, auto geometryVertex, auto firstEmb, auto lastEmb) {
		const auto &gGeometry = lib::Stereo::getGeometryGraph().getGraph();
		if(geometryVertex == lib::Stereo::getGeometryGraph().nullGeometry()) {
			s << "nullGeo";
			return;
		}
		s << gGeometry[geometryVertex].name << "[";
		bool first = true;
		for(; firstEmb != lastEmb; ++firstEmb) {
			if(!first) s << ", ";
			first = false;
			const lib::Stereo::EmbeddingEdge &emb = *firstEmb;
			switch(emb.type) {
			case lib::Stereo::EmbeddingEdge::Type::LonePair:
				s << "e";
				break;
			case lib::Stereo::EmbeddingEdge::Type::Radical:
				MOD_ABORT;
				break;
			case lib::Stereo::EmbeddingEdge::Type::Edge:
				s << emb.offset;
				break;
			}
		}
		s << "]";
	}

public:

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename VertexFirst>
	void printVertexFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const VertexFirst &vFirst) {
		const auto m = membership(rFirst, vFirst);
		s << ", Stereo(";
		if(m != Membership::Right) {
			const auto &conf = *get_stereo(get_labelled_left(rFirst))[vFirst];
			printData(s, conf.getGeometryVertex(), conf.begin(), conf.end());
		} else s << "<>";
		s << " ->";
		if(!get_stereo(rFirst).inContext(vFirst)) s << "!";
		s << " ";
		if(m != Membership::Left) {
			const auto &conf = *get_stereo(get_labelled_right(rFirst))[vFirst];
			printData(s, conf.getGeometryVertex(), conf.begin(), conf.end());
		} else s << "<>";
		s << ")";
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename VertexSecond>
	void printVertexSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const VertexSecond &vSecond) {
		const auto m = membership(rSecond, vSecond);
		s << ", Stereo(";
		if(m != Membership::Right) {
			const auto &conf = *get_stereo(get_labelled_left(rSecond))[vSecond];
			printData(s, conf.getGeometryVertex(), conf.begin(), conf.end());
		} else s << "<>";
		s << " ->";
		if(!get_stereo(rSecond).inContext(vSecond)) s << "!";
		s << " ";
		if(m != Membership::Left) {
			const auto &conf = *get_stereo(get_labelled_right(rSecond))[vSecond];
			printData(s, conf.getGeometryVertex(), conf.begin(), conf.end());
		} else s << "<>";
		s << ")";
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename VertexResult>
	void printVertexResult(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const VertexResult &vResult) {
		const auto m = membership(result.rResult, vResult);
		const auto &gResult = get_graph(result.rResult);
		const auto vResultId = get(boost::vertex_index_t(), gResult, vResult);
		const auto &dLeft = vDataLeft[vResultId];
		const auto &dRight = vDataRight[vResultId];
		s << ", Stereo(";
		if(m != Membership::Right)
			printData(s, dLeft.vGeometry, dLeft.edges.begin(), dLeft.edges.end());
		else s << "<>";
		s << " ->";
		if(!vertexInContext[vResultId]) s << "!";
		s << " ";
		if(m != Membership::Left)
			printData(s, dRight.vGeometry, dRight.edges.begin(), dRight.edges.end());
		else s << "<>";
		s << ")";
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename EdgeFirst>
	void printEdgeFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const EdgeFirst &eFirst) {
		const auto m = membership(rFirst, eFirst);
		s << ", Stereo(";
		if(m != Membership::Right) {
			s << get_stereo(get_labelled_left(rFirst))[eFirst];
		} else s << "<>";
		s << " ->";
		if(!get_stereo(rFirst).inContext(eFirst)) s << "!";
		s << " ";
		if(m != Membership::Left) {
			s << get_stereo(get_labelled_right(rFirst))[eFirst];
		} else s << "<>";
		s << ")";
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename EdgeSecond>
	void printEdgeSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const EdgeSecond &eSecond) {
		const auto m = membership(rSecond, eSecond);
		s << ", Stereo(";
		if(m != Membership::Right) {
			s << get_stereo(get_labelled_left(rSecond))[eSecond];
		} else s << "<>";
		s << " ->";
		if(!get_stereo(rSecond).inContext(eSecond)) s << "!";
		s << " ";
		if(m != Membership::Left) {
			s << get_stereo(get_labelled_right(rSecond))[eSecond];
		} else s << "<>";
		s << ")";
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexRvsLR(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) {
		// -> a | a -> b, maybe a == b
		// to
		// -> b
		assert(!vertexInContext[get(boost::vertex_index_t(), get_graph(result.rResult), vResult)]);
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexLRvsL(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) {
		// a -> a | a ->
		// b -> a | a ->
		// to
		// a ->
		// b ->
		const auto vIdResult = get(boost::vertex_index_t(), get_graph(result.rResult), vResult);
		vertexInContext[vIdResult] = false;
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexLRvsLR(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) {
		// a != b, a != c, b =? c
		// a -> a | a -> a
		// a -> a | a -> c
		// b -> a | a -> a
		// b -> a | a -> c
		// if both vertices are in context, so should the result be
		const auto vIdResult = get(boost::vertex_index_t(), get_graph(result.rResult), vResult);
		vertexInContext[vIdResult] = vertexInContext[vIdResult] && get_stereo(rSecond).inContext(vSecond);
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeResult, typename EdgeSecond>
	void setEdgeResultRightFromSecondRight(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			EdgeResult eResult, EdgeSecond eSecond) {
		// |  ->   vs.    -> |, simply copy the R2 to R
		// |  -> | vs. |  -> |, do the copy, but also set inContext if both are in context
		const auto eIdResult = get(boost::edge_index_t(), get_graph(result.rResult), eResult);
		eDataRight[eIdResult] = get_stereo(get_labelled_right(rSecond))[eSecond];
		edgeInContext[eIdResult] = edgeInContext[eIdResult] && get_stereo(rSecond).inContext(eSecond);
	}
};

} // namespace Composer
} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_VISITOR_STEREO_H */
