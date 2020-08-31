#ifndef MOD_LIB_DG_NONHYPERBUILDER_H
#define MOD_LIB_DG_NONHYPERBUILDER_H

#include <mod/Derivation.hpp>
#include <mod/lib/DG/NonHyper.hpp>

#include <nlohmann/json.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {
struct GraphState;
} // namespace Strategies

struct ExecuteResult {
	ExecuteResult(NonHyperBuilder *owner, int execution);
	const Strategies::GraphState &getResult() const;
	void list(bool withUniverse) const;
private:
	NonHyperBuilder *owner;
	int execution;
};

struct Builder {
	explicit Builder(NonHyperBuilder *dg);
	Builder(Builder &&other);
	Builder &operator=(Builder &&other);
	~Builder();
public:
	// pre: !d.left.empty()
	// pre: !d.right.empty()
	// pre: no nullptrs in d
	std::pair<NonHyper::Edge, bool> addDerivation(const Derivations &d, IsomorphismPolicy graphPolicy);
	// pre: strategy must not have been executed before (i.e., a newly constructed strategy, or a clone)
	ExecuteResult execute(std::unique_ptr<Strategies::Strategy> strategy, int verbosity, bool ignoreRuleLabelTypes);
	std::vector<std::pair<NonHyper::Edge, bool>>
	apply(const std::vector<std::shared_ptr<graph::Graph> > &graphs, std::shared_ptr<rule::Rule> r,
	      int verbosity, IsomorphismPolicy graphPolicy);
	void addAbstract(const std::string &description);
	bool load(const std::vector<std::shared_ptr<rule::Rule>> &ruleDatabase,
	          const std::string &file, std::ostream &err, int verbosity);
public:
	// load a dump, without checking label settings
	// returns false if it did not go well
	bool trustLoadDump(nlohmann::json &&j,
	                   const std::vector<std::shared_ptr<rule::Rule>> &ruleDatabase,
	                   std::ostream &err, int verbosity);
private:
	NonHyperBuilder *dg;
};

struct NonHyperBuilder : public NonHyper {
	// pre: no nullptrs in graphDatabse
	NonHyperBuilder(LabelSettings labelSettings,
	                const std::vector<std::shared_ptr<graph::Graph> > &graphDatabase, IsomorphismPolicy graphPolicy);
	virtual ~NonHyperBuilder();
	virtual std::string getType() const override;
	Builder build();
private:
	friend class ExecuteResult;
	friend class Builder;
	std::set<std::shared_ptr<rule::Rule>> rules; // to keep them alive
	struct ExecutionEnv;
	struct StrategyExecution {
		std::unique_ptr<ExecutionEnv> env;
		std::unique_ptr<Strategies::GraphState> input;
		std::unique_ptr<Strategies::Strategy> strategy;
	};
	std::vector<StrategyExecution> executions;
};

} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_NONHYPERBUILDER_H */
