#ifndef MOD_RULE_H
#define MOD_RULE_H

#include <mod/Config.h>

#include <iosfwd>
#include <memory>
#include <string>

namespace mod {
class GraphPrinter;
namespace lib {
namespace Rules {
class Real;
} // namespace Rules
} // namespace lib

// rst-class: Rule
// rst:
// rst:		Model of a transformation rule in the Double Pushout formalism.
// rst:		See :ref:`cpp-RuleGraphInterface` for the documentation for the
// rst:		graph interface for this class.
// rst:
// rst-class-start:

struct Rule {
	class Vertex;
	class Edge;
	class VertexIterator;
	class VertexRange;
	class EdgeIterator;
	class EdgeRange;
	class IncidentEdgeIterator;
	class IncidentEdgeRange;
public:
	class LeftGraph;
	class ContextGraph;
	class RightGraph;
private:
	Rule(const Rule&) = delete;
	Rule &operator=(const Rule&) = delete;
	Rule(Rule&&) = delete;
	Rule &operator=(Rule&&) = delete;
private:
	Rule(std::unique_ptr<lib::Rules::Real> r);
public:
	~Rule();
	// rst: .. function:: unsigned int getId() const
	// rst:
	// rst:		:returns: the unique instance id among all :class:`Rule` instances.
	unsigned int getId() const;
	friend std::ostream &operator<<(std::ostream &s, const Rule &r);
	const lib::Rules::Real &getRule() const;
public: // graph interface
	// rst: .. function:: std::size_t numVertices() const
	// rst:
	// rst:		:returns: the number of vertices in the rule.
	std::size_t numVertices() const;
	// rst: .. function:: VertexRange vertices() const
	// rst:
	// rst:		:returns: a range of all vertices in the rule.
	VertexRange vertices() const;
	// rst: .. function:: std::size_t numEdges() const
	// rst:
	// rst:		:returns: the number of edges in the rule.
	std::size_t numEdges() const;
	// rst: .. function:: EdgeRange edges() const
	// rst:
	// rst:		:returns: a range of all edges in the rule.
	EdgeRange edges() const;
	// rst: .. function:: LeftGraph getLeft() const
	// rst:
	// rst:		:returns: a proxy object representing the left graph of the rule.
	LeftGraph getLeft() const;
	// rst: .. function:: ContextGraph getContext() const
	// rst:
	// rst:		:returns: a proxy object representing the context graph of the rule.
	ContextGraph getContext() const;
	// rst: .. function:: RightGraph getRight() const
	// rst:
	// rst:		:returns: a proxy object representing the rule graph of the rule.
	RightGraph getRight() const;
public:
	// rst:	.. function:: std::shared_ptr<Rule> makeInverse() const
	// rst:
	// rst:			:returns: a rule representing the inversion of this rule.
	// rst:			:raises: :class:`InputError` if inversion is not possible (due to matching constraints).
	std::shared_ptr<Rule> makeInverse() const;
	// rst: .. function:: std::pair<std::string, std::string> print() const
	// rst:               std::pair<std::string, std::string> print(const GraphPrinter &first, const GraphPrinter &second) const
	// rst:
	// rst:		Print the rule, using either the default options or the options in ``first`` and ``second``.
	// rst:		If ``first`` and ``second`` are the same, only one depiction will be made.
	// rst:
	// rst:		:returns: a pair of filename prefixes for the PDF-files that will be compiled in post-processing.
	// rst:			The actual names can be obtained by appending ``_L.pdf``, ``_K.pdf``, and ``_R.pdf`` for
	// rst:			respectively the left side, context, and right side graphs.
	// rst:			If ``first`` and ``second`` are the same, the two file prefixes are equal.
	std::pair<std::string, std::string> print() const;
	std::pair<std::string, std::string> print(const GraphPrinter &first, const GraphPrinter &second) const;
	// rst: .. function:: std::string getGMLString(bool withCoords = false) const
	// rst:
	// rst:		:returns: the :ref:`GML <rule-gml>` representation of the rule,
	// rst:		          optionally with generated 2D coordinates.
	// rst:		:throws: :any:`LogicError` when coordinates are requested, but
	// rst:		         none can be generated.
	std::string getGMLString(bool withCoords = false) const;
	// rst: .. function:: std::string printGML(bool withCoords = false) const
	// rst:
	// rst:		Print the rule in :ref:`GML <rule-gml>` format,
	// rst:		optionally with generated 2D coordinates.
	// rst:
	// rst:		:returns: the filename of the printed GML file.
	// rst:		:throws: :any:`LogicError` when coordinates are requested, but
	// rst:		         none can be generated.
	std::string printGML(bool withCoords = false) const;
	// rst: .. function:: const std::string &getName() const
	// rst:	              void setName(std::string name)
	// rst:
	// rst:		Access the name of the rule.
	const std::string &getName() const;
	void setName(std::string name);
	// rst: .. function:: std::size_t getNumLeftComponents() const
	// rst:
	// rst:		:returns: the number of connected components in the left graph.
	std::size_t getNumLeftComponents() const;
	// rst: .. function:: std::size_t getNumRightComponents() const
	// rst:
	// rst:		:returns: the number of connected components in the right graph.
	std::size_t getNumRightComponents() const;
	// rst: .. function:: std::size_t isomorphism(std::shared_ptr<Rule> r, std::size_t maxNumMatches) const
	// rst:
	// rst:		:returns: the number of isomorphisms found between ``r`` and this graph, but at most ``maxNumMatches``.
	std::size_t isomorphism(std::shared_ptr<Rule> r, std::size_t maxNumMatches) const;
	// rst: .. function:: std::size_t monomorphism(std::shared_ptr<Rule> r, std::size_t maxNumMatches) const
	// rst:
	// rst:		:returns: the number of monomorphism from this rule to subgraphs of ``r``, though at most ``maxNumMatches``.
	std::size_t monomorphism(std::shared_ptr<Rule> r, std::size_t maxNumMatches) const;
public:
	// rst: .. function:: static std::shared_ptr<Rule> ruleGMLString(const std::string &data, bool invert)
	// rst:
	// rst:		Load a rule from a :ref:`GML <rule-gml>` string, and store either that rule or its inverse.
	// rst:		The name of the rule is the one specified in the GML string, though when ``invert=True``
	// rst:		the string ", inverse" is appended to the name.
	// rst:
	// rst:		.. note::
	// rst:
	// rst:			If the GML string specifies matching constraints it is not possible to invert the rule.
	// rst:			There is however a configuration option to ignore matching constraints when inverting rules.
	// rst:
	// rst:		:returns: the loaded (possibly inverted) rule.
	// rst:		:throws: :class:`InputError` on bad data and when inversion fails due to constraints.
	static std::shared_ptr<Rule> ruleGMLString(const std::string &data, bool invert);
	// rst: .. function:: static std::shared_ptr<Rule> ruleGML(const std::string &file, bool invert)
	// rst:		
	// rst:		Read ``file`` and pass the contents to :cpp:func:`ruleGMLString`.
	// rst:
	// rst:		:returns: the loaded (possibly inverted) rule.
	// rst:		:throws: :class:`InputError` on bad data and when inversion fails due to constraints.
	static std::shared_ptr<Rule> ruleGML(const std::string &file, bool invert);
	// rst: .. function:: static std::shared_ptr<Rule> makeRule(std::unique_ptr<lib::Rules::Real> r)
	// rst:
	// rst:		:returns: a rule wrapping the given internal rule object.
	static std::shared_ptr<Rule> makeRule(std::unique_ptr<lib::Rules::Real> r);
private:
	struct Pimpl;
	std::unique_ptr<Pimpl> p;
};
// rst-class-end:

struct RuleLess {

	bool operator()(std::shared_ptr<Rule> r1, std::shared_ptr<Rule> r2) const {
		return r1->getId() < r2->getId();
	}
};

} // namespace mod

#endif /* MOD_RULE_H */