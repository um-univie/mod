#ifndef MOD_LIB_DG_HYPER_H
#define MOD_LIB_DG_HYPER_H

#include <mod/dg/GraphInterface.h>
#include <mod/Derivation.h>
#include <mod/lib/DG/NonHyper.h>

namespace mod {
template<typename> class Function;
namespace lib {
namespace Graph {
class Single;
} // namespace Graph
namespace DG {

class HyperCreator {
	HyperCreator(const HyperCreator&) = delete;
	HyperCreator &operator=(const HyperCreator&) = delete;
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
	void addRuleToEdge(NonHyper::Edge eNon, const lib::Rules::Real *r);
	void setReverse(NonHyper::Edge e, NonHyper::Edge eBack);
private:
	Hyper *owner = nullptr;
};

class Hyper {
	Hyper(const Hyper&) = delete;
	Hyper(Hyper&&) = delete;
	Hyper &operator=(const Hyper&) = delete;
	Hyper &operator=(Hyper&&) = delete;
public:
	// bipartite representation
	using GraphType = HyperGraphType;
	using Vertex = HyperVertex;
	using Edge = HyperEdge;
	using VertexKind = HyperVertexKind;
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
	std::map<const lib::Graph::Single*, Vertex> graphToHyperVertex;
public:
	static void temp_compare(const Hyper &a, const Hyper &b);
};

} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_HYPER_H */
