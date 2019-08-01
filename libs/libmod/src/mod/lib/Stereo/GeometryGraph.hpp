#ifndef MOD_LIB_STEREO_GEOMETRY_GRAPH_H
#define MOD_LIB_STEREO_GEOMETRY_GRAPH_H

#include <mod/Chem.hpp>
#include <mod/lib/Stereo/EdgeCategory.hpp>

#include <boost/graph/adjacency_list.hpp>

#include <functional>
#include <tuple>
#include <vector>

namespace mod {
namespace lib {
namespace Stereo {
struct Configuration;
struct EmbeddingEdge;
struct Fixation;

enum struct DeductionResult {
	Success, Warning, Error
};

struct GeometryGraph {

	struct VProp {
		using Constructor = std::function<std::unique_ptr<Configuration>(const EmbeddingEdge*, const EmbeddingEdge*, const Fixation&, std::ostream&) >;
	public:
		VProp() = default; // because BGL requires it
		VProp(const std::string &name);
	public:
		std::string name;
		Constructor constructor;
	};

	struct EProp {
	};
	using GraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VProp, EProp>;
	using Vertex = boost::graph_traits<GraphType>::vertex_descriptor;
	using Edge = boost::graph_traits<GraphType>::edge_descriptor;

	struct ChemValid {
		AtomId atomId;
		Charge charge;
		bool radical;
		EdgeCategoryCount catCount; // only the bonds, and Undefined must be 0
		std::size_t lonePair;
		Vertex geometry;
	};
private:
	friend const GeometryGraph &getGeometryGraph();
	GeometryGraph();
	GeometryGraph(const GeometryGraph&) = delete;
	GeometryGraph(GeometryGraph&&) = delete;
	GeometryGraph &operator=(const GeometryGraph&) = delete;
	GeometryGraph &operator=(GeometryGraph&&) = delete;
public:
	const GraphType &getGraph() const;
	Vertex findGeometry(const std::string &name) const;
	static Vertex nullGeometry();
public: // deduction
	std::tuple<DeductionResult, unsigned char> deduceLonePairs(const AtomData &ad, const EdgeCategoryCount &catCount, Vertex vGeometry, bool asPattern, std::ostream &err) const;
	std::tuple<DeductionResult, Vertex> deduceGeometry(const AtomData &ad, const EdgeCategoryCount &catCount, unsigned char numLonePairs, bool asPattern, std::ostream &err) const;
	std::tuple<DeductionResult, Vertex, unsigned char> deduceGeometryAndLonePairs(const AtomData &ad, const EdgeCategoryCount &catCount, bool asPattern, std::ostream &err) const;
public: // matching
	bool isAncestorOf(Vertex ancestor, Vertex child) const; // true also if child == ancestor
	Vertex generalize(Vertex a, Vertex b) const;
	Vertex unify(Vertex a, Vertex b) const;
	Vertex pushoutComplement(Vertex a, Vertex b, Vertex d) const; // a -> b -> d, find c, st. a -> c -> d is the pushout complement
private:
	Vertex addGeometry(VProp &&vProp) const;
	Vertex addChild(Vertex vParent, VProp &&vProp);
private:
	mutable GraphType g;
	mutable std::map<std::string, Vertex> nameToVertex;
public:
	std::vector<ChemValid> chemValids;
public:
	Vertex any;
	Vertex linear;
	Vertex trigonalPlanar;
	Vertex tetrahedral;
};

const GeometryGraph &getGeometryGraph(); // singleton

} // namespace Stereo
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_STEREO_GEOMETRY_GRAPH_H */