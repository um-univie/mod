#ifndef MOD_LIB_GRAPH_SINGLE_H
#define MOD_LIB_GRAPH_SINGLE_H

#include <mod/Config.h>
#include <mod/lib/Graph/Base.h>
#include <mod/lib/Graph/LabelledGraph.h>

#include <boost/optional/optional.hpp>

#include <iosfwd>
#include <string>

namespace mod {
class Graph;
class Rule;
namespace lib {
namespace Graph {
struct PropMolecule;
struct DepictionData;

struct Single : public Base {
	Single(std::unique_ptr<GraphType> g, std::unique_ptr<PropString> pString);
	Single(Single &&) = default;
	~Single();
	const LabelledGraph &getLabelledGraph() const;
	std::size_t getId() const;
	std::shared_ptr<mod::Graph> getAPIReference() const;
	void setAPIReference(std::shared_ptr<mod::Graph> g);
	void printName(std::ostream &s) const;
	const std::string &getName() const;
	void setName(std::string name);
	const std::pair<const std::string&, bool> getGraphDFS() const;
	const std::string &getSmiles() const;
	std::shared_ptr<mod::Rule> getBindRule() const;
	std::shared_ptr<mod::Rule> getIdRule() const;
	std::shared_ptr<mod::Rule> getUnbindRule() const;
	unsigned int getVertexLabelCount(const std::string &label) const;
	unsigned int getEdgeLabelCount(const std::string &label) const;
	DepictionData &getDepictionData();
	const DepictionData &getDepictionData() const;
public: // deprecated interface
	const GraphType &getGraph() const;
	const PropString &getStringState() const;
	const PropMolecule &getMoleculeState() const;
private:
	LabelledGraph g;
	const std::size_t id;
	std::weak_ptr<mod::Graph> apiReference;
	std::string name;
	mutable boost::optional<std::string> dfs;
	mutable bool dfsHasNonSmilesRingClosure;
	mutable boost::optional<std::string> smiles;
	mutable std::shared_ptr<mod::Rule> bindRule, idRule, unbindRule;
	mutable std::unique_ptr<DepictionData> depictionData;
public:
	static std::size_t isomorphismVF2(const Single &gDom, const Single &gCodom, std::size_t maxNumMatches);
	static bool isomorphismBrokenSmilesAndVF2(const Single &gDom, const Single &gCodom);
	static std::size_t isomorphism(const Single &gDom, const Single &gCodom, std::size_t maxNumMatches);
	static std::size_t monomorphism(const Single &gDom, const Single &gCodom, std::size_t maxNumMatches);
	static bool nameLess(const Single *g1, const Single *g2);
};

Single makePermutation(const Single &g);

namespace detail {

struct IsomorphismPredicate {

	bool operator()(const Single *gDom, const Single *gCodom) const {
		return 1 == Single::isomorphism(*gDom, *gCodom, 1);
	}
};

} // namespace detail

inline detail::IsomorphismPredicate makeIsomorphismPredicate() {
	return detail::IsomorphismPredicate();
}

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_SINGLE_H */
