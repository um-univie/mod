#ifndef MOD_LIB_DG_HYPER_H
#define MOD_LIB_DG_HYPER_H

#include <mod/dg/GraphInterface.hpp>
#include <mod/Derivation.hpp>
#include <mod/lib/DG/NonHyper.hpp>

namespace mod {
template<typename>
class Function;
namespace lib {
namespace Graph {
class Single;
} // namespace Graph
namespace DG {

class HyperCreator {
	HyperCreator(const HyperCreator &) = delete;
	HyperCreator &operator=(const HyperCreator &) = delete;
public:
	friend class Hyper;

private:
	HyperCreator() = default;
	explicit HyperCreator(Hyper &hyper);
public:
	HyperCreator(HyperCreator &&other);
	HyperCreator &operator=(HyperCreator &&other);
	~HyperCreator();
	void addVertex(const lib::Graph::Single *g);
	HyperVertex addEdge(NonHyper::Edge eNon);
private:
	Hyper *owner = nullptr;
};

class Hyper {
	Hyper(const Hyper &) = delete;
	Hyper(Hyper &&) = delete;
	Hyper &operator=(const Hyper &) = delete;
	Hyper &operator=(Hyper &&) = delete;
public:
	// bipartite representation
	using GraphType = HyperGraphType;
	using Vertex = HyperVertex;
	using Edge = HyperEdge;
private:
	friend class HyperCreator;

	Hyper(const NonHyper &dg);
public:
	static std::pair<std::unique_ptr<Hyper>, HyperCreator> makeHyper(const NonHyper &dg);
	//	Hyper(const NonHyper &dg, int dummy);
private:
	void addVertex(const lib::Graph::Single *g);
public:
	~Hyper();
	const NonHyper &getNonHyper() const;
	const GraphType &getGraph() const;
	void printStats(std::ostream &s) const;
	bool isVertexGraph(const lib::Graph::Single *g) const;
	Vertex getVertexOrNullFromGraph(const lib::Graph::Single *g) const;
	// requires: isVertexGraph(g)
	Vertex getVertexFromGraph(const lib::Graph::Single *g) const;
	// requires: getGraph()[e].kind == HyperVertexKind::Edge
	// returns null_vertex is no reverse exists, or the data has not been initialized yet
	Vertex getReverseEdge(Vertex e) const;
	// requires: getGraph()[e].kind == HyperVertexKind::Edge
	const std::vector<const lib::Rules::Real *> &getRulesFromEdge(Vertex e) const;
public:
	dg::DG::Vertex getInterfaceVertex(Vertex v) const;
	dg::DG::HyperEdge getInterfaceEdge(Vertex e) const;
public:
	Derivation getDerivation(Vertex v) const;
private:
	bool hasCalculated;
	const NonHyper &nonHyper;
	GraphType hyper;
private:
	std::map<const lib::Graph::Single *, Vertex> graphToHyperVertex;
};

} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_HYPER_H */
