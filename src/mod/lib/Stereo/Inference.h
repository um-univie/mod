#ifndef MOD_LIB_STEREO_INFERENCE_H
#define MOD_LIB_STEREO_INFERENCE_H

#include <mod/Error.h>

#include <mod/lib/Stereo/EdgeCategory.h>
#include <mod/lib/Stereo/EmbeddingEdge.h>
#include <mod/lib/Stereo/GeometryGraph.h>
#include <mod/lib/Stereo/Configuration/Configuration.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/graph/graph_traits.hpp>

namespace mod {
namespace lib {
namespace Stereo {
namespace detail {
template<typename Graph, typename PropMolecule> struct makeInferenceHelper;

struct InferenceBase {

	struct VertexData {
		GeometryGraph::Vertex vGeometry = GeometryGraph::nullGeometry();
		std::size_t nextAvailableVirtual;
		std::vector<EmbeddingEdge> edges;
		bool explicitEmbedding = false;
		Fixation fix = Fixation::free();
	public:
		std::unique_ptr<Configuration> configuration;
	};

	struct EdgeData {

		EdgeData() : valid(EdgeCategorySubset().all()) { }
	public:
		EdgeCategorySubset valid;
		EdgeCategory finalCategory; // will be set by finalize
	};
};

} // namesapce detail

template<typename Graph, typename PropMolecule>
struct Inference : private detail::InferenceBase {
	using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
	using Edge = typename boost::graph_traits<Graph>::edge_descriptor;
public:
	friend class detail::makeInferenceHelper<Graph, PropMolecule>;

	Inference(const Graph &g, PropMolecule&&) = delete;

	Inference(const Graph &g, const PropMolecule &pMolecule, bool asPattern)
	: g(g), pMolecule(pMolecule), asPattern(asPattern),
	hasFinalized(false), vertexData(num_vertices(g)), edgeData(num_edges(g)) {
		for(Vertex v : asRange(vertices(g))) {
			vertexData[get(boost::vertex_index_t(), g, v)].nextAvailableVirtual = out_degree(v, g);
		}
	}

	bool assignGeometry(Vertex v, GeometryGraph::Vertex vGeometry, std::ostream &err) {
		assert(vGeometry != GeometryGraph::nullGeometry());
		auto vId = get(boost::vertex_index_t(), g, v);
		auto &data = vertexData[vId];
		if(data.vGeometry != GeometryGraph::nullGeometry()) {
			err << "Geometry already assigned.\n";
			return false;
		}
		data.vGeometry = vGeometry;
		return true;
	}

	bool assignEdgeCategory(Edge e, EdgeCategory cat, std::ostream &err) {
		auto eId = get(boost::edge_index_t(), g, e);
		auto &data = edgeData[eId];
		if(!data.valid(cat)) {
			err << "Can not assign '" << cat << "'. Not a valid possibility (valid=" << data.valid << ").";
			return false;
		}
		data.valid = EdgeCategorySubset() + cat;
		return true;
	}

	void initEmbedding(Vertex v) {
		auto vId = get(boost::vertex_index_t(), g, v);
		auto &data = vertexData[vId];
		data.explicitEmbedding = true;
	}

	void addEdge(Vertex v, Edge eOut) {
		auto vId = get(boost::vertex_index_t(), g, v);
		auto &data = vertexData[vId];
		data.explicitEmbedding = true;
		auto outRange = out_edges(v, g);
		auto iter = std::find(outRange.first, outRange.second, eOut);
		assert(iter != outRange.second);
		// Add the edge with undefined category. It will be assigned properly in finalization.
		data.edges.emplace_back(std::distance(outRange.first, iter), EmbeddingEdge::Type::Edge, EdgeCategory::Undefined);
	}

	void addLonePair(Vertex v) {
		auto vId = get(boost::vertex_index_t(), g, v);
		auto &data = vertexData[vId];
		data.explicitEmbedding = true;
		auto offset = data.nextAvailableVirtual;
		++data.nextAvailableVirtual;
		data.edges.emplace_back(offset, EmbeddingEdge::Type::LonePair, EdgeCategory::Single);
	}

	void addRadical(Vertex v) {
		auto vId = get(boost::vertex_index_t(), g, v);
		auto &data = vertexData[vId];
		data.explicitEmbedding = true;
		auto offset = data.nextAvailableVirtual;
		++data.nextAvailableVirtual;
		data.edges.emplace_back(offset, EmbeddingEdge::Type::Radical, EdgeCategory::Single);
	}

	void fixSimpleGeometry(Vertex v) {
		auto vId = get(boost::vertex_index_t(), g, v);
		auto &data = vertexData[vId];
		assert(data.explicitEmbedding); // the embedding should have been initialised at this point
		data.fix = Fixation::simpleFixed();
	}

	template<typename VertexPrinter>
	DeductionResult finalize(std::ostream &err, VertexPrinter vertexPrinter) {
		auto result = DeductionResult::Success;
		const auto &geo = getGeometryGraph();
		const auto &gGeometry = geo.getGraph();
		assert(!hasFinalized);
		// Finalize edge categories.
		//--------------------------------------------------------------------------
		for(Edge e : asRange(edges(g))) {
			auto eId = get(boost::edge_index_t(), g, e);
			auto &data = edgeData[eId];
			const EdgeCategorySubset &eCat = data.valid;
			if(eCat.count() == 0) MOD_ABORT; // should have been handled earlier
			if(eCat.count() == 1) {
				data.finalCategory = eCat.selectFirst();
			} else {
				// assume chemical
				auto eCatChem = bondTypeToEdgeCategory(pMolecule[e]);
				if(!eCat(eCatChem)) {
					MOD_ABORT; // TODO: implement
				}
				data.finalCategory = eCatChem;
			}
		}

		// Assign edge categories to embedding edges.
		//--------------------------------------------------------------------------
		for(Vertex v : asRange(vertices(g))) {
			auto vId = get(boost::vertex_index_t(), g, v);
			auto &data = vertexData[vId];
			for(auto &emb : data.edges) {
				if(emb.type != EmbeddingEdge::Type::Edge) continue;
				auto e = emb.getEdge(v, g);
				emb.cat = edgeData[get(boost::edge_index_t(), g, e)].finalCategory;
			}
		}

		for(Vertex v : asRange(vertices(g))) {
			auto res = finalizeVertex(v, err, vertexPrinter);
			if(res == DeductionResult::Error) return res;
			else if(res == DeductionResult::Warning) result = DeductionResult::Warning;
		}

		// Construct the configurations.
		//--------------------------------------------------------------------------
		for(Vertex v : asRange(vertices(g))) {
			auto vId = get(boost::vertex_index_t(), g, v);
			auto &data = vertexData[vId];
			assert(data.vGeometry != GeometryGraph::nullGeometry());
			assert(!data.configuration);
			std::stringstream ssErr;
			data.configuration = gGeometry[data.vGeometry].constructor(
					data.edges.data(), data.edges.data() + data.edges.size(), data.fix, ssErr);
			if(!data.configuration) {
				err << "Error in configuration construction for vertex " << vertexPrinter(v) << ".\n";
				err << ssErr.str();
				return DeductionResult::Error;
			}
		}
		hasFinalized = true;
		return result;
	}
private:

	EdgeCategoryCount addEdgesFromGraph(Vertex v) {
		auto vId = get(boost::vertex_index_t(), g, v);
		auto &data = vertexData[vId];
		auto d = out_degree(v, g);
		assert(data.edges.empty());
		data.edges.reserve(d);
		std::size_t offset = 0;
		EdgeCategoryCount catCount;
		for(Edge eOut : asRange(out_edges(v, g))) {
			data.edges.emplace_back(offset, EmbeddingEdge::Type::Edge, edgeData[get(boost::edge_index_t(), g, eOut)].finalCategory);
			++catCount[edgeData[get(boost::edge_index_t(), g, eOut)].finalCategory];
			++offset;
		}
		return catCount;
	}

	void addLonePairs(Vertex v, std::size_t n) {
		auto vId = get(boost::vertex_index_t(), g, v);
		auto &data = vertexData[vId];
		data.edges.reserve(data.edges.size() + n);
		for(std::size_t i = 0; i < n; ++i) {
			data.edges.emplace_back(data.edges.size(), EmbeddingEdge::Type::LonePair, EdgeCategory::Single);
		}
	}

	template<typename VertexPrinter>
	DeductionResult finalizeVertex(Vertex v, std::ostream &err, VertexPrinter vertexPrinter) {
		auto result = DeductionResult::Success;
		const auto &geo = getGeometryGraph();
		auto vId = get(boost::vertex_index_t(), g, v);
		auto &data = vertexData[vId];
		AtomData ad = pMolecule[v];
		EdgeCategoryCount catCount;

		bool explicitGeometry = data.vGeometry != GeometryGraph::nullGeometry();
		bool explicitEmbedding = data.explicitEmbedding;

		std::size_t numLonePairs = 0;
		bool radical = false;
		if(!explicitEmbedding) {
			catCount = addEdgesFromGraph(v);
			// TODO: shouldn't we deduce radical?
		} else {
			std::vector<bool> neighbourPresent(data.edges.size(), false);
			if(data.edges.size() < out_degree(v, g)) {
				err << "Too few edges in embedding for vertex " << vertexPrinter(v) << ". Got " << data.edges.size() << ", but the degree is " << out_degree(v, g) << "." << std::endl;
				return DeductionResult::Error;
			}
			for(const auto &emb : data.edges) {
				if(emb.offset >= data.edges.size()) {
					MOD_ABORT; // error, offset out of bounds
					return DeductionResult::Error;
				}
				if(neighbourPresent[emb.offset]) {
					MOD_ABORT; // error, duplicate neighbour
					return DeductionResult::Error;
				}
				if(emb.offset < out_degree(v, g) && emb.type != EmbeddingEdge::Type::Edge) {
					MOD_ABORT; // error, [0, d[ are reserved for the real edges
					return DeductionResult::Error;
				}
				neighbourPresent[emb.offset] = true;
				switch(emb.type) {
				case EmbeddingEdge::Type::Edge:
					++catCount[emb.cat];
					break;
				case EmbeddingEdge::Type::LonePair:
					++numLonePairs;
					break;
				case EmbeddingEdge::Type::Radical:
					if(radical) {
						MOD_ABORT; // only one radical per vertex
						return DeductionResult::Error;
					}
					radical = true;
					break;
				}
			}
		}
		if(explicitGeometry && explicitEmbedding) {
			// the user has spoken
		} else if(explicitGeometry && !explicitEmbedding) {
			std::tie(result, numLonePairs) = geo.deduceLonePairs(ad, catCount, data.vGeometry, asPattern, err);
		} else if(!explicitGeometry && explicitEmbedding) {
			std::tie(result, data.vGeometry) = geo.deduceGeometry(ad, catCount, numLonePairs, asPattern, err);
		} else {
			assert(!explicitGeometry && !explicitEmbedding);
			std::tie(result, data.vGeometry, numLonePairs) = geo.deduceGeometryAndLonePairs(ad, catCount, asPattern, err);
		}
		if(result == DeductionResult::Error) return result;
		if(!explicitEmbedding) addLonePairs(v, numLonePairs);
		return result;
	}
public: // if hasFinalized

	std::unique_ptr<Configuration> extractConfiguration(Vertex v) {
		assert(hasFinalized);
		auto vId = get(boost::vertex_index_t(), g, v);
		assert(vertexData[vId].configuration);
		return std::move(vertexData[vId].configuration);
	}

	EdgeCategory getEdgeCategory(Edge e) const {
		auto eId = get(boost::edge_index_t(), g, e);
		assert(eId < num_edges(g));
		return edgeData[eId].finalCategory;
	}
public:
	const Graph &g;
	const PropMolecule &pMolecule;
	const bool asPattern;
private:
	bool hasFinalized;
	std::vector<VertexData> vertexData;
	std::vector<EdgeData> edgeData;
};

namespace detail {

// see http://stackoverflow.com/questions/27835925/overload-between-rvalue-reference-and-const-lvalue-reference-in-template

template<typename Graph, typename PropMolecule>
struct makeInferenceHelper {
	static Inference<Graph, PropMolecule> make(const Graph &g, PropMolecule &&pMolecule, bool asPattern) = delete;

	static Inference<Graph, PropMolecule> make(const Graph &g, const PropMolecule &pMolecule, bool asPattern) {
		return Inference<Graph, PropMolecule>(g, pMolecule, asPattern);
	}
};

} // detail

template<typename Graph, typename PropMolecule>
Inference<typename std::decay<Graph>::type, typename std::decay<PropMolecule>::type> makeInference(Graph &&g, PropMolecule &&pMolecule, bool asPattern) {
	return detail::makeInferenceHelper<typename std::decay<Graph>::type, typename std::decay<PropMolecule>::type>
			::make(std::forward<Graph>(g), std::forward<PropMolecule>(pMolecule), asPattern);
}

} // namespace Stereo
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_STEREO_INFERENCE_H */