#ifndef MOD_LIB_DG_HYPER_H
#define MOD_LIB_DG_HYPER_H

#include <mod/Derivation.h>
#include <mod/lib/DG/NonHyper.h>

namespace mod {
template<typename> class Function;
namespace lib {
namespace Graph {
class Single;
} // namespace Graph
namespace DG {
struct Expanded;
class Matrix;

class HyperCreator {
	HyperCreator(const HyperCreator&) = delete;
	HyperCreator &operator=(const HyperCreator&) = delete;
public:
	friend class Hyper;
private:
	HyperCreator(Hyper &hyper);
public:
	HyperCreator(HyperCreator &&other);
	~HyperCreator();
	void addVertex(const lib::Graph::Single *g);
	void addEdge(NonHyper::Edge eNon);
	void setReverse(NonHyper::Edge e, NonHyper::Edge eBack);
private:
	Hyper &owner;
	bool valid;
	struct Pimpl;
	std::unique_ptr<Pimpl> pimpl;
};

class Hyper {
	Hyper(const Hyper&) = delete;
	Hyper &operator=(const Hyper&) = delete;
	Hyper(Hyper&&) = delete;
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
	const Matrix &getMatrixSparse() const;
	void printStats(std::ostream &s) const;
	void printIncidence(std::ostream &s, const std::shared_ptr<mod::Graph> g) const;
	Vertex getVertexFromDerivationRef(mod::DerivationRef dRef) const;
	Vertex getVertexFromGraph(const lib::Graph::Single *g) const;
	bool isVertexGraph(std::shared_ptr<mod::Graph> g) const;
	const std::vector<std::shared_ptr<mod::Graph> > &getVertexGraphs() const;
public:
	mod::DerivationRef getDerivationRef(Vertex v) const;
	mod::DerivationRef getDerivationRefValid(Vertex v) const;
	mod::Derivation getDerivation(Vertex v) const;
public: // derivation iteration
	unsigned int getFirstDerivation() const;
	unsigned int getEndDerivation() const;
	unsigned int getNextDerivation(unsigned int id) const;
	mod::DerivationRef dereferenceDerivation(unsigned int id) const;
public: // in derivation iteration
	unsigned int getFirstInDerivation(Vertex v) const;
	unsigned int getEndInDerivation(Vertex v) const;
	unsigned int getNextInDerivation(unsigned int id, Vertex v) const;
	mod::DerivationRef dereferenceInDerivation(unsigned int id, Vertex v) const;
public: // out derivation iteration
	unsigned int getFirstOutDerivation(Vertex v) const;
	unsigned int getEndOutDerivation(Vertex v) const;
	unsigned int getNextOutDerivation(unsigned int id, Vertex v) const;
	mod::DerivationRef dereferenceOutDerivation(unsigned int id, Vertex v) const;
private:
	bool hasCalculated;
	const NonHyper &nonHyper;
	GraphType hyper;
private:
	std::map<const lib::Graph::Single*, Vertex> graphToHyperVertex;
	std::vector<std::shared_ptr<mod::Graph> > vertexGraphs;
	mutable const Matrix *matrixSparse;
public:
	static void temp_compare(const Hyper &a, const Hyper &b);
};

} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_HYPER_H */
