#include "Builder.hpp"

#include <mod/Error.hpp>
#include <mod/dg/Strategies.hpp>
#include <mod/lib/DG/Hyper.hpp>
#include <mod/lib/DG/NonHyperBuilder.hpp>
#include <mod/lib/DG/Strategies/GraphState.hpp>
#include <mod/lib/DG/Strategies/Strategy.hpp>
#include <mod/lib/Graph/Single.hpp>

#include <boost/lexical_cast.hpp>

namespace mod {
namespace dg {

struct Builder::Pimpl {
	Pimpl(std::shared_ptr<DG> dg_, lib::DG::NonHyperBuilder &dgLib) : dg_(dg_), b(dgLib.build()) {}
public:
	std::shared_ptr<DG> dg_;
	lib::DG::Builder b;
};

Builder::Builder(lib::DG::NonHyperBuilder &dg_) : p(new Pimpl(dg_.getAPIReference(), dg_)) {}

Builder::Builder(Builder &&other) = default;
Builder &Builder::operator=(Builder &&other) = default;
Builder::~Builder() = default;

bool Builder::isActive() const {
	return p != nullptr;
}

namespace {

template<typename T>
// because Pimpl is private
void check(const T &p) {
	if(!p) throw LogicError("The builder is not active.");
}

} // namespace

DG::HyperEdge Builder::addDerivation(const Derivations &d) {
	return addDerivation(d, IsomorphismPolicy::Check);
}

DG::HyperEdge Builder::addDerivation(const Derivations &d, IsomorphismPolicy graphPolicy) {
	check(p);
	if(d.left.empty()) throw LogicError("Derivation has empty left side: " + boost::lexical_cast<std::string>(d));
	if(d.right.empty()) throw LogicError("Derivation has empty right side: " + boost::lexical_cast<std::string>(d));
	auto innerRes = p->b.addDerivation(d, graphPolicy);
	return p->dg_->getHyper().getInterfaceEdge(p->dg_->getNonHyper().getHyperEdge(innerRes.first));
}

ExecuteResult Builder::execute(std::shared_ptr<Strategy> strategy) {
	return execute(strategy, 1);
}

ExecuteResult Builder::execute(std::shared_ptr<Strategy> strategy, int verbosity) {
	return execute(strategy, verbosity, false);
}

ExecuteResult Builder::execute(std::shared_ptr<Strategy> strategy, int verbosity, bool ignoreRuleLabelTypes) {
	check(p);
	auto res = p->b.execute(std::unique_ptr<lib::DG::Strategies::Strategy>(strategy->getStrategy().clone()),
	                        verbosity, ignoreRuleLabelTypes);
	return ExecuteResult(p->dg_, std::move(res));
}

void Builder::addAbstract(const std::string &description) {
	check(p);
	p->b.addAbstract(description);
}

// -----------------------------------------------------------------------------

struct ExecuteResult::Pimpl {
	Pimpl(std::shared_ptr<DG> dg_, lib::DG::ExecuteResult res) : dg_(dg_), res(std::move(res)) {}
public:
	std::shared_ptr<DG> dg_;
	lib::DG::ExecuteResult res;
	std::vector<std::shared_ptr<graph::Graph>> subset;
	std::vector<std::shared_ptr<graph::Graph>> universe;
};

ExecuteResult::ExecuteResult(std::shared_ptr<DG> dg_, lib::DG::ExecuteResult innerRes)
		: p(new Pimpl(dg_, std::move(innerRes))) {}

ExecuteResult::ExecuteResult(ExecuteResult &&other) = default;
ExecuteResult &ExecuteResult::operator=(ExecuteResult &&other) = default;
ExecuteResult::~ExecuteResult() = default;

const std::vector<std::shared_ptr<graph::Graph>> &ExecuteResult::getSubset() const {
	if(p->subset.empty()) {
		const auto &inner = p->res.getResult().getSubset(0);
		p->subset.reserve(inner.size());
		for(const auto *g : inner)
			p->subset.push_back(g->getAPIReference());
	}
	return p->subset;
}
const std::vector<std::shared_ptr<graph::Graph>> &ExecuteResult::getUniverse() const {
	if(p->universe.empty()) {
		const auto &inner = p->res.getResult().getUniverse();
		p->universe.reserve(inner.size());
		for(const auto *g : inner)
			p->universe.push_back(g->getAPIReference());
	}
	return p->universe;
}

void ExecuteResult::list(bool withUniverse) const {
	p->res.list(withUniverse);
}

} // namespace dg
} // namespace mod