#ifndef MOD_DG_BUILDER_H
#define MOD_DG_BUILDER_H

#include <mod/BuildConfig.hpp>
#include <mod/dg/ForwardDecl.hpp>
#include <mod/dg/GraphInterface.hpp>

#include <memory>

namespace mod {
struct Derivations;
namespace lib {
namespace DG {
struct ExecuteResult;
} // namespace DG
} // namespace lib
namespace dg {

// rst-class: dg::Builder
// rst:
// rst:		An RAII-style object obtained from :cpp:func:`DG::build`.
// rst:		On destruction of an active builder object the owning :cpp:class:`DG` will be locked
// rst:		for further modifications.
// rst:		Builder objects are move-only types, and a moved from object becomes an inactive builder,
// rst:		not associated with any :cpp:class:`DG`.
// rst:
// rst-class-start:
class MOD_DECL Builder {
	friend class DG;
	explicit Builder(lib::DG::NonHyperBuilder &dg_);
public:
	Builder(Builder &&other);
	Builder &operator=(Builder &&other);
	~Builder();
	// rst: .. function:: bool isActive() const
	// rst:
	// rst:		:returns: whether this object is associated with a :cpp:class:`DG`.
	bool isActive() const;
public:
	// rst: .. function:: DG::HyperEdge addDerivation(const Derivations &d)
	// rst:               DG::HyperEdge addDerivation(const Derivations &d, IsomorphismPolicy graphPolicy)
	// rst:
	// rst:		Adds a hyperedge corresponding to the given derivation to the associated :class:`DG`.
	// rst:		If it already exists, only add the given rules to the edge.
	// rst:		The given :var:`graphPolicy` refers to adding the graphs in :var:`d`,
	// rst:		and it defaults to :enumerator:`IsomorphismPolicy::Check`.
	// rst:
	// rst:		:returns: the hyperedge corresponding to the given derivation.
	// rst:		:throws: :class:`LogicError` if `!isActive()`.
	// rst:		:throws: :class:`LogicError` if `d.left.empty()`.
	// rst:		:throws: :class:`LogicError` if `d.right.empty()`.
	// rst:		:throws: :class:`LogicError` if `graphPolicy == IsomorphismPolicy::Check` and a given graph object
	// rst:			is different but isomorphic to another given graph object or to a graph object already
	// rst:			in the internal graph database in the associated derivation graph.
	DG::HyperEdge addDerivation(const Derivations &d);
	DG::HyperEdge addDerivation(const Derivations &d, IsomorphismPolicy graphPolicy);
	// rst: .. function:: ExecuteResult execute(std::shared_ptr<Strategy> strategy)
	// rst:               ExecuteResult execute(std::shared_ptr<Strategy> strategy, int verbosity)
	// rst:               ExecuteResult execute(std::shared_ptr<Strategy> strategy, int verbosity, bool ignoreRuleLabelTypes)
	// rst:
	// rst:		Execute the given strategy (:ref:`dgStrat`) and as a side-effect add
	// rst:		vertices and hyperedges to the underlying derivation graph.
	// rst:
	// rst:		The :cpp:var:`verbosity` defaults to level 2.
	// rst:		The levels have the following meaning:
	// rst:
	// rst:		- 0 (or less): no information is printed.
	// rst:		- 2: Repetition strategies print information for each round.
	// rst:		- 4: All strategies print minimal information.
	// rst:		- 6: Derivation predicate strategies and filtering strategies also print their predicates.
	// rst:		- 8: Rule strategies print minimal information about graph binding.
	// rst:		- 10: Rule strategies print more information about graph binding, including failure due to derivation predicates.
	// rst:		- 50: Print information about morphism generation for rule composition.
	// rst:		- 60: Print rule composition information.
	// rst:
	// rst:		:throws: :class:`LogicError` if a static "add" strategy has `IsomorphismPolicy::Check` as graph policy,
	// rst:			and it tries to add a graph object isomorphic to an already known, but different, graph object in the database.
	// rst:			This is checked before execution, so there is strong exception guarantee.
	// rst:		:throws: :class:`LogicError` if a dynamic "add" strategy has `IsomorphismPolicy::Check` as graph policy,
	// rst:			and it tries to add a graph object isomorphic to an already known, but different, graph object in the database.
	// rst:
	// rst:			.. warning:: This is checked during execution, so while the basic exception guarantee is provided,
	// rst:				there may be modifications to the underlying derivation graph.
	// rst:		:throws: :class:`LogicError`: if `ignoreRuleLabelTypes` is `false`, which is the default,
	// rst:			and a rule in the given strategy has an associated :enum:`LabelType` which is different from the one
	// rst:			in the derivation graph.
	ExecuteResult execute(std::shared_ptr<Strategy> strategy);
	ExecuteResult execute(std::shared_ptr<Strategy> strategy, int verbosity);
	ExecuteResult execute(std::shared_ptr<Strategy> strategy, int verbosity, bool ignoreRuleLabelTypes);
	// rst: .. function:: void addAbstract(const std::string &description)
	// rst:
	// rst:		Add vertices and hyperedges based on the given abstract description.
	// rst:		The description must adhere to the grammar described at :ref:`dg_abstract-desc`.
	// rst:
	// rst:		For each vertex named in the description a graph object with no vertices will be created,
	// rst:		and its name set to the given identifier.
	// rst:
	// rst:		:throws: :class:`InputError` if the description could not be parsed.
	void addAbstract(const std::string &description);
private:
	struct Pimpl;
	std::unique_ptr<Pimpl> p;
};
// rst-class-end:

// rst-class: dg::ExecuteResult
// rst:
// rst:		The result from calling :func:`Builder::execute`.
// rst:
// rst-class-start:
class MOD_DECL ExecuteResult {
	friend class Builder;
	explicit ExecuteResult(std::shared_ptr<DG> dg_, lib::DG::ExecuteResult innerRes);
public:
	ExecuteResult(ExecuteResult &&other);
	ExecuteResult &operator=(ExecuteResult &&other);
	~ExecuteResult();
	// rst: .. function:: const std::vector<std::shared_ptr<graph::Graph>> &getSubset() const
	// rst:               const std::vector<std::shared_ptr<graph::Graph>> &getUniverse() const
	// rst:
	// rst:		:returns: respectively the subset and the universe computed by the strategy execution (see also :ref:`dgStrat`).
	const std::vector<std::shared_ptr<graph::Graph>> &getSubset() const;
	const std::vector<std::shared_ptr<graph::Graph>> &getUniverse() const;
	// rst: .. function:: void list(bool withUniverse) const
	// rst:
	// rst:		Output information from the execution of the strategy.
	// rst:		The universe lists can be rather long so with `withUniverse == false` they are omitted.
	void list(bool withUniverse) const;
private:
	struct Pimpl;
	std::unique_ptr<Pimpl> p;
};
// rst-class-end:

} // namespace dg
} // namespace mod

#endif /* MOD_DG_BUILDER_H */
