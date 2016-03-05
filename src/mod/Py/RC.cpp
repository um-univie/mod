#include <mod/Py/Common.h>

#include <mod/RC.h>

// rst: This page contains the core functionality for composing rules and creating rules from graphs.
// rst: Note that there is both a low-level interface for constructing expressions,
// rst: and an embedded mini-language for defining the expressions using normal Python operators.
// rst:
// rst: The Embedded Rule Composition Expression Language
// rst: #################################################
// rst:
// rst: The embedded language is really a collection of proxy classes with a lot of operator overloading, thus
// rst: the normal syntax and semantics of Python applies.
// rst:
// rst: A rule composition expression always returns a list of rules when evaluated.
// rst: The following is the grammar for the expressions.
// rst:
// rst: .. productionlist:: RCExpExp
// rst:    rcExp: `rcExp` `op` `rcExp`
// rst:         : "rcBind(" `graphs` ")"
// rst:         : "rcUnbind(" `graphs` ")"
// rst:         : "rcId(" `graphs` ")"
// rst:         : `rules`
// rst:    op: "*" `opObject` "*"
// rst:    opObject: "rcParallel"
// rst:            : "rcSuper(allowPartial=False)"
// rst:            : "rcSuper"
// rst:            : "rcSub(allowPartial=False)"
// rst:            : "rcSub"
// rst:            : "rcCommon"
// rst:
// rst: Here a ``graphs`` is any Python expression that is either a single :class:`Graph` or an iterable of graphs.
// rst: Similarly, a ``rules`` must be either a :class:`Rule` or an iterable of rules.
// rst: An :token:`rcExp` may additionally be an iterable of expressions.
// rst: See the API below for more details on the semantics of each expression, and the corresponding :ref:`C++ page<cpp-RC>`.
// rst:

namespace mod {
namespace Py {
namespace {

std::vector<std::shared_ptr<mod::Rule> > getRuleDatabase(std::shared_ptr<mod::RCEvaluator> rc) {
	return std::vector < std::shared_ptr<mod::Rule> >(begin(rc->getRuleDatabase()), end(rc->getRuleDatabase()));
}

std::vector<std::shared_ptr<mod::Rule> > getProducts(std::shared_ptr<mod::RCEvaluator> rc) {
	return std::vector < std::shared_ptr<mod::Rule> >(begin(rc->getProducts()), end(rc->getProducts()));
}

std::vector<std::shared_ptr<mod::Rule> > eval(std::shared_ptr<mod::RCEvaluator> rc, const mod::RCExp::Expression &e) {
	auto result = rc->eval(e);
	return std::vector<std::shared_ptr<mod::Rule> >(begin(result), end(result));
}

std::shared_ptr<mod::RCEvaluator> create(const std::vector<std::shared_ptr<mod::Rule> > dVec) {
	std::unordered_set<std::shared_ptr<mod::Rule> > dUSet(begin(dVec), end(dVec));
	return mod::RCEvaluator::create(dUSet);
}

} // namespace 

void RC_doExport() {

	// rst: Expression Evaluator
	// rst: #################################################
	// rst:
	// rst: .. class:: RCEvaluator
	// rst:
	// rst:		This class can evaluate rule composition expressions. During evaluation an expression graph is recorded.
	// rst: 
	// rst:		The expression graph is a directed hypergraph :math:`(V, E)`, with each vertex representing a rule.
	// rst:		Each edge represent all compositions calculated for a unique input. That is every edge :math:`e\in E` is on the form
	// rst:		:math:`((u, v), R_e)` with :math:`(u, v)\in V\times V` as an *ordered* pair of rules and :math:`R_e\subseteq V` is the set
	// rst:		of all resulting rules found.
	// rst:
	// rst:		The graph is visualised as a bipartite graph with point-shaped vertices representing the hyperedges. The in-edges to these
	// rst:		hyperedge vertices are labelled with 1 and 2.
	// rst:
	py::class_<mod::RCEvaluator, std::shared_ptr<mod::RCEvaluator>, boost::noncopyable>("RCEvaluator", py::no_init)
			// rst:		.. attribute:: ruleDatabase
			// rst:
			// rst:			(Read-only) The list of unique rules known by the evaluator.
			// rst:
			// rst:			:type: list of :class:`Rule`
			.add_property("_ruleDatabase", &getRuleDatabase)
			// rst:		.. attribute:: products
			// rst:
			// rst:			(Read-only) The list of unique rules this evaluator has constructed.
			// rst:
			// rst:			:type: list of :class:`Rule`
			.add_property("_products", &getProducts)
			// rst:		.. method:: eval(exp)
			// rst:
			// rst:			Evaluates a rule composition expression. Any created rule is replaced by a rule in the database if they are isomorphic.
			// rst:
			// rst:			:returns: the resulting list of rules of the expression.
			// rst:			:param exp: the expression to evaluate.
			// rst:			:type exp: :class:`RCExpExp`
			.def("eval", &eval)
			// rst:		.. method:: print()
			// rst:
			// rst:			Print the graph representing all expressions evaluated so far.
			.def("print", &mod::RCEvaluator::print)
			;

	// rst: .. method:: rcEvaluator(database)
	// rst:
	// rst:		:param database: a list of isomorphic rules the evaluator will compare against.
	// rst:		:type database: list of :class:`Rule`
	// rst:		:returns: a rule composition expression evaluator.
	// rst:		:rtype: :class:`RCEvaluator`
	// rst:
	// rst:		.. note:: The caller is responsible for ensuring the given rules are unique.
	// rst:
	py::def("rcEvaluator", create);

	// rst: Rule Composition Expressions
	// rst: #################################################
	// rst:
	// rst: An expression, ``RCExpExp``, can be evaluated through the method :meth:`RCEvaluator.eval`.
	// rst: The result of an expression is a list of rules.
	// rst:

	// Nullary/unary
	//--------------------------------------------------------------------------
	// rst: .. py:class:: RCExpUnion
	// rst:
	// rst:		Return the union of the subexpressions. I.e., flatten the subresult lists into a single list.
	// rst:
	py::class_<mod::RCExp::Union>("RCExpUnion", py::no_init)
			.def(py::init<std::vector<mod::RCExp::Expression> >())
			.def(str(py::self))
			;
	// rst: .. py:class:: RCExpBind
	// rst:
	// rst:		Return the singleton list with the rule :math:`(\emptyset, \emptyset, G)` for the given graph :math:`G`.
	// rst:
	py::class_<mod::RCExp::Bind>("RCExpBind", py::no_init)
			.def(py::init<std::shared_ptr<mod::Graph> >())
			.def(str(py::self))
			;
	// rst: .. py:class:: RCExpId
	// rst:
	// rst:		Return the singleton list with the rule :math:`(G, G, G)` for the given graph :math:`G`.
	// rst:
	py::class_<mod::RCExp::Id>("RCExpId", py::no_init)
			.def(py::init<std::shared_ptr<mod::Graph> >())
			.def(str(py::self))
			;
	// rst: .. py:class:: RCExpUnbind
	// rst:
	// rst:		Return the singleton list with the rule :math:`(G, \emptyset, \emptyset)` for the given graph :math:`G`.
	// rst:
	py::class_<mod::RCExp::Unbind>("RCExpUnbind", py::no_init)
			.def(py::init<std::shared_ptr<mod::Graph> >())
			.def(str(py::self))
			;

	// Expresssion
	//--------------------------------------------------------------------------
	// rst: .. py:class:: RCExpExp
	// rst:
	// rst:		A generic rule composition expression.
	// rst:
	py::class_<mod::RCExp::Expression>("RCExpExp", py::no_init)
			.def(str(py::self))
			;

	// Binary
	//--------------------------------------------------------------------------
	// rst: .. py:class:: RCExpComposeCommon
	// rst:
	// rst:		The base class for the composition of two rule :math:`(L_1, K_1, R_1)` and :math:`(L_2, K_2, R_2)`.
	// rst:
	py::class_<mod::RCExp::ComposeCommon>("RCExpComposeCommon", py::no_init)
			.def(py::init < mod::RCExp::Expression, mod::RCExp::Expression, bool, bool, bool>())
			.def(str(py::self))
			;
	// rst: .. py:class:: RCExpComposeParallel
	// rst:
	// rst:		Compose the rules by all common subgraphs of :math:`R_1` and :math:`L_2`,
	// rst:		possibly limited to connected subgraphs or to the subgraphs of maximum size.
	// rst:
	py::class_<mod::RCExp::ComposeParallel>("RCExpComposeParallel", py::no_init)
			.def(py::init < mod::RCExp::Expression, mod::RCExp::Expression, bool>())
			.def(str(py::self))
			;
	// rst: .. py:class:: RCExpComposeSub
	// rst:
	// rst:		Compose the rules by the empty graph, i.e., create a rule representing the parallel application of two input rules.
	// rst:
	py::class_<mod::RCExp::ComposeSub>("RCExpComposeSub", py::no_init)
			.def(py::init < mod::RCExp::Expression, mod::RCExp::Expression, bool, bool>())
			.def(str(py::self))
			;
	// rst: .. py:class:: RCExpComposeSuper
	// rst:
	// rst:		Compose the rules such that overlapping connected components of :math:`R_1` and :math:`L_2` have the :math:`L_2` component as a subgraph of :math:`R_1`.
	// rst:		The overlap is `partial` if not every connected component of :math:`L_2` is participating in the common subgraph.
	// rst:
	py::class_<mod::RCExp::ComposeSuper>("RCExpComposeSuper", py::no_init)
			.def(py::init < mod::RCExp::Expression, mod::RCExp::Expression, bool, bool, bool>())
			.def(str(py::self))
			;

	py::implicitly_convertible<std::shared_ptr<mod::Rule>, mod::RCExp::Expression>();
	py::implicitly_convertible<mod::RCExp::Union, mod::RCExp::Expression>();
	py::implicitly_convertible<mod::RCExp::Bind, mod::RCExp::Expression>();
	py::implicitly_convertible<mod::RCExp::Id, mod::RCExp::Expression>();
	py::implicitly_convertible<mod::RCExp::Unbind, mod::RCExp::Expression>();
	py::implicitly_convertible<mod::RCExp::ComposeCommon, mod::RCExp::Expression>();
	py::implicitly_convertible<mod::RCExp::ComposeParallel, mod::RCExp::Expression>();
	py::implicitly_convertible<mod::RCExp::ComposeSub, mod::RCExp::Expression>();
	py::implicitly_convertible<mod::RCExp::ComposeSuper, mod::RCExp::Expression>();
}

} // namespace Py
} // namespace mod