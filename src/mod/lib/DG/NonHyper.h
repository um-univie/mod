#ifndef MOD_LIB_DG_NONHYPER_H
#define MOD_LIB_DG_NONHYPER_H

#include <mod/Graph.h>
#include <mod/lib/DG/GraphDecl.h>
#include <mod/lib/Graph/Merge.h>

#include <boost/graph/adjacency_list.hpp>

#include <iosfwd>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace mod {
class DerivationRef;
class DG;
class Graph;
namespace lib {
namespace DG {
class Hyper;
class HyperCreator;

class NonHyper {
	friend class HyperCreator;
protected:
	using GraphType = NonHyperGraphType;
	using Vertex = NonHyperVertex;
	using Edge = NonHyperEdge;
public:
	// TODO: change to unordered set, when Boost >= 1.51 is installed
	typedef std::set<std::shared_ptr<mod::Graph>, mod::GraphLess> StdGraphSet;
	// don't touch this, we need a specific ordering on the MergeStore
	typedef std::set<const lib::Graph::Merge*, lib::Graph::MergeLess> MergeStore;
protected:
	NonHyper(const std::vector<std::shared_ptr<mod::Graph> > &graphDatabase);
public: // general
	virtual ~NonHyper();
	std::size_t getId() const;
	std::shared_ptr<mod::DG> getAPIReference() const;
	void setAPIReference(std::shared_ptr<mod::DG> dg);
	virtual std::string getType() const = 0;
public: // calculation
	void calculate();
	bool getHasCalculated() const;
protected: // calculation
	virtual void calculateImpl() = 0;
	// adds the graph to the graph database
	// returns true iff it was a new graph
	bool addGraph(std::shared_ptr<mod::Graph> g);
	// adds the graph to the database if it's not there already
	// makes a vertex in the DG
	// returns true iff it was a new graph
	bool addGraphAsVertex(std::shared_ptr<mod::Graph> g);
	// searches the database for the given graph
	// if found, returns the found graph and false, additionally the given graph is deleted
	// if not found, returns the given wrapped given graph and true
	// does NOT change the graphDatabse
	std::pair<std::shared_ptr<mod::Graph>, bool> checkIfNew(std::unique_ptr<lib::Graph::GraphType> gBoost, std::unique_ptr<lib::Graph::PropString> labelState) const;
	// gives a graph product status, i.e., rename it, put it in the product list and maybe print a status message
	void giveProductStatus(std::shared_ptr<mod::Graph> g);
	// adds the graph to the database if it's not there already
	// if it was not there, give it product status
	// returns true iff it was given product status
	bool addProduct(std::shared_ptr<mod::Graph> g);
	// adds graph to the merge store
	// if an isomorphic graph were already there, the given graph is deleted
	const lib::Graph::Merge *addToMergeStore(const lib::Graph::Merge *g);
	// checks if this derivation already exists
	// if it does then the edge descriptor of that derivation is returned, otherwise the edge descriptor is bogus
	std::pair<Edge, bool> isDerivation(const lib::Graph::Base *left, const lib::Graph::Base *right, const lib::Rules::Real *r) const;
	// adds a derivation if it does not exist already
	// the edge descriptor of the derivation is returned, along with the existence status before the call
	// the rule may be nullptr
	std::pair<Edge, bool> suggestDerivation(const lib::Graph::Base *gSrc, const lib::Graph::Base *gTar, const lib::Rules::Real *r);
	const GraphType &getGraphDuringCalculation() const;
private: // calculation
	// adds the graph as a vertex, if it's not there already, and returns the vertex
	Vertex getVertex(const lib::Graph::Base *g);
	void findReversiblePairs();
public: // post calculation
	void list(std::ostream &s) const;
	const GraphType &getGraph() const;
	Hyper &getHyper();
	const Hyper &getHyper() const;
	const StdGraphSet &getGraphDatabase() const;
	const std::vector<std::shared_ptr<mod::Graph> > &getProducts() const;
	void print() const;
	mod::DerivationRef getDerivationRef(const std::vector<HyperVertex> &sources, const std::vector<HyperVertex> &targets) const;
	std::vector<mod::DerivationRef> getAllDerivationRefs() const;
protected:
	virtual void listImpl(std::ostream &s) const = 0;
private: // general
	std::size_t id;
	std::weak_ptr<mod::DG> apiReference;
	StdGraphSet graphDatabase;
	MergeStore mergedGraphs;
	GraphType dg;
	std::map<const lib::Graph::Base*, Vertex> graphToVertex;
	std::unique_ptr<Hyper> hyper;
	HyperCreator *hyperCreator; // only valid during calculation
private: // calculation
	bool hasCalculated;
	unsigned int productNum;
	std::vector<std::shared_ptr<mod::Graph> > products;
public:
	static void diff(const NonHyper &dg1, const NonHyper &dg2);
};

} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_NONHYPER_H */
