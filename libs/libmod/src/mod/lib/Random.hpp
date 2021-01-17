#ifndef MOD_LIB_RANDOM_HPP
#define MOD_LIB_RANDOM_HPP

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>

#include <cassert>
#include <map>
#include <random>

namespace mod::lib {

struct Random {
	using Engine = std::mt19937;
private:
	Random();
public:
	Engine &getEngine();
	void reseed(unsigned int seed);
private:
	unsigned int seed;
	Engine engine;
public:
	static Random &getInstance();
};

Random::Engine &getRng();

// makes a random permutation of [0; n[
std::vector<size_t> makePermutation(std::size_t n);
std::vector<size_t> invertPermutation(const std::vector<std::size_t> &p);

template<typename Graph, typename Initialiser, typename VertexCopier, typename EdgeCopier>
std::unique_ptr<Graph> makePermutedGraph(const Graph &g, Initialiser initialize, VertexCopier copyVertex, EdgeCopier copyEdge) {
	using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
	using Edge = typename boost::graph_traits<Graph>::edge_descriptor;
	using SizeType = typename boost::graph_traits<Graph>::vertices_size_type;
	std::vector<Vertex> vertexFromId;
	vertexFromId.reserve(num_vertices(g));
	for(Vertex v : asRange(vertices(g))) {
		assert(get(boost::vertex_index_t(), g, v) == vertexFromId.size());
		vertexFromId.push_back(v);
	}
	auto oldFromPermuted = makePermutation(num_vertices(g));
	auto gPermutedPtr = std::make_unique<Graph>();
	initialize(*gPermutedPtr);
	auto &gPermuted = *gPermutedPtr;
	std::map<Vertex, Vertex> permutedVertexFromOld;
	for(SizeType iPermuted = 0; iPermuted < num_vertices(g); iPermuted++) {
		Vertex vPermuted = add_vertex(gPermuted);
		assert(get(boost::vertex_index_t(), gPermuted, vPermuted) == iPermuted);
		assert(oldFromPermuted[iPermuted] < num_vertices(g));
		Vertex vOld = vertexFromId[oldFromPermuted[iPermuted]];
		assert(permutedVertexFromOld.find(vOld) == end(permutedVertexFromOld));
		permutedVertexFromOld[vOld] = vPermuted;
		copyVertex(vOld, g, vPermuted, gPermuted);
	}
	for(auto eOld : asRange(edges(g))) {
		Vertex vSrcOld = source(eOld, g);
		Vertex vTarOld = target(eOld, g);
		assert(permutedVertexFromOld.find(vSrcOld) != end(permutedVertexFromOld));
		assert(permutedVertexFromOld.find(vTarOld) != end(permutedVertexFromOld));
		Vertex vSrcNew = permutedVertexFromOld[vSrcOld];
		Vertex vTarNew = permutedVertexFromOld[vTarOld];
		auto epNew = add_edge(vSrcNew, vTarNew, gPermuted);
		assert(epNew.second);
		copyEdge(eOld, g, epNew.first, gPermuted);
	}
	return gPermutedPtr;
}

} // namespace mnod::lib

#endif // MOD_LIB_RANDOM_HPP