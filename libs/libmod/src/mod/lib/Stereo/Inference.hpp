#ifndef MOD_LIB_STEREO_INFERENCE_HPP
#define MOD_LIB_STEREO_INFERENCE_HPP

#include <mod/Error.hpp>

#include <mod/lib/IO/Result.hpp>
#include <mod/lib/Stereo/EdgeCategory.hpp>
#include <mod/lib/Stereo/EmbeddingEdge.hpp>
#include <mod/lib/Stereo/GeometryGraph.hpp>
#include <mod/lib/Stereo/Configuration/Configuration.hpp>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/lexical_cast.hpp>

namespace mod::lib::Stereo {
namespace detail {
template<typename Graph, typename PropMolecule>
struct makeInferenceHelper;

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
		EdgeCategorySubset valid = EdgeCategorySubset().all();
		EdgeCategory finalCategory; // will be set by finalize
	};
};

} // namespace detail

template<typename Graph, typename PropMolecule>
struct Inference : private detail::InferenceBase {
	using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
	using Edge = typename boost::graph_traits<Graph>::edge_descriptor;
public:
	friend class detail::makeInferenceHelper<Graph, PropMolecule>;

	Inference(const Graph &g, PropMolecule &&) = delete;

	Inference(const Graph &g, const PropMolecule &pMolecule, bool asPattern)
			: g(g), pMolecule(pMolecule), asPattern(asPattern),
			  hasFinalized(false), vertexData(num_vertices(g)), edgeData(num_edges(g)) {
		for(Vertex v : asRange(vertices(g))) {
			vertexData[get(boost::vertex_index_t(), g, v)].nextAvailableVirtual = out_degree(v, g);
		}
	}

	lib::IO::Result<> assignGeometry(Vertex v, GeometryGraph::Vertex vGeometry) {
		assert(vGeometry != GeometryGraph::nullGeometry());
		auto vId = get(boost::vertex_index_t(), g, v);
		auto &data = vertexData[vId];
		if(data.vGeometry != GeometryGraph::nullGeometry())
			return lib::IO::Result<>::Error("Geometry already assigned.");
		data.vGeometry = vGeometry;
		return {};
	}

	lib::IO::Result<> assignEdgeCategory(Edge e, EdgeCategory cat) {
		auto eId = get(boost::edge_index_t(), g, e);
		auto &data = edgeData[eId];
		if(!data.valid(cat)) {
			return lib::IO::Result<>::Error(
					"Can not assign '" + boost::lexical_cast<std::string>(cat)
					+ "'. Not a valid possibility (valid="
					+ boost::lexical_cast<std::string>(data.valid) + ").");
		}
		data.valid = EdgeCategorySubset() + cat;
		return {};
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
	lib::IO::Result<> finalize(lib::IO::Warnings &warnings, VertexPrinter vertexPrinter) {
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
		for(const Vertex v : asRange(vertices(g))) {
			const auto vId = get(boost::vertex_index_t(), g, v);
			auto &data = vertexData[vId];
			for(auto &emb : data.edges) {
				if(emb.type != EmbeddingEdge::Type::Edge) continue;
				auto e = emb.getEdge(v, g);
				emb.cat = edgeData[get(boost::edge_index_t(), g, e)].finalCategory;
			}
		}

		for(const Vertex v : asRange(vertices(g))) {
			auto res = finalizeVertex(warnings, v, vertexPrinter);
			if(!res) return res;
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
			if(!data.configuration)
				return lib::IO::Result<>::Error("Error in configuration construction for vertex " + vertexPrinter(v) + ".\n"
				                                + ssErr.str());
		}
		hasFinalized = true;
		return {};
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
			data.edges.emplace_back(offset, EmbeddingEdge::Type::Edge,
			                        edgeData[get(boost::edge_index_t(), g, eOut)].finalCategory);
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
	lib::IO::Result<> finalizeVertex(lib::IO::Warnings &warnings, Vertex v, VertexPrinter vertexPrinter) {
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
				return lib::IO::Result<>::Error(
						"Too few edges in embedding for vertex " + vertexPrinter(v) + ". Got " +
						std::to_string(data.edges.size())
						+ ", but the degree is " + std::to_string(out_degree(v, g)) + "."
				);
			}
			for(const auto &emb : data.edges) {
				if(emb.offset >= data.edges.size()) {
					MOD_ABORT; // error, offset out of bounds
					return lib::IO::Result<>::Error("");
				}
				if(neighbourPresent[emb.offset]) {
					MOD_ABORT; // error, duplicate neighbour
					return lib::IO::Result<>::Error("");
				}
				if(emb.offset < out_degree(v, g) && emb.type != EmbeddingEdge::Type::Edge) {
					MOD_ABORT; // error, [0, d[ are reserved for the real edges
					return lib::IO::Result<>::Error("");
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
						return lib::IO::Result<>::Error("");
					}
					radical = true;
					break;
				}
			}
		}
		if(explicitGeometry && explicitEmbedding) {
			// the user has spoken
		} else if(explicitGeometry && !explicitEmbedding) {
			if(auto res = geo.deduceLonePairs(warnings, ad, catCount, data.vGeometry, asPattern))
				numLonePairs = *res;
			else return std::move(res);
		} else if(!explicitGeometry && explicitEmbedding) {
			if(auto res = geo.deduceGeometry(warnings, ad, catCount, numLonePairs, asPattern))
				data.vGeometry = *res;
			else return std::move(res);
		} else {
			assert(!explicitGeometry && !explicitEmbedding);
			if(auto res = geo.deduceGeometryAndLonePairs(warnings, ad, catCount, asPattern))
				std::tie(data.vGeometry, numLonePairs) = *res;
			else return std::move(res);
		}
		if(!explicitEmbedding) addLonePairs(v, numLonePairs);
		return {};
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
Inference<typename std::decay<Graph>::type, typename std::decay<PropMolecule>::type>
makeInference(Graph &&g, PropMolecule &&pMolecule, bool asPattern) {
	return detail::makeInferenceHelper<typename std::decay<Graph>::type, typename std::decay<PropMolecule>::type>
	::make(std::forward<Graph>(g), std::forward<PropMolecule>(pMolecule), asPattern);
}

} // namespace mod::lib::Stereo

#endif // MOD_LIB_STEREO_INFERENCE_HPP