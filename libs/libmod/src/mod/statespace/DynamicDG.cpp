#include <mod/statespace/DynamicDG.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/dg/DG.hpp>
#include <mod/lib/DG/Hyper.hpp>
#include <mod/lib/statespace/DynamicDG.hpp>

namespace mod::statespace {

std::vector<const lib::Rules::Real*> unwrapRules(
        const std::vector<std::shared_ptr<rule::Rule>>& rules) {
	std::vector<const lib::Rules::Real*> libRules;
	for (const std::shared_ptr<rule::Rule>& rule : rules) {
		libRules.push_back(&rule->getRule());
	}
	return libRules;
}

struct DynamicDG::Pimpl {
	Pimpl(lib::DG::NonHyperBuilder* builder, std::vector<const lib::Rules::Real*> rules) :
	    ddg(builder, rules) {}
public:
	mod::lib::statespace::DynamicDG ddg;
};

DynamicDG::DynamicDG(const std::vector<std::shared_ptr<graph::Graph>>& graphDatabase,
                     const std::vector<std::shared_ptr<rule::Rule>>& rules,
                     LabelSettings labelSettings):
    rules(rules),
    dg(dg::DG::make(labelSettings, graphDatabase, IsomorphismPolicy::Check)),
    p(new Pimpl(&dg->getNonHyperBuilder(), unwrapRules(rules))) {
}

DynamicDG::~DynamicDG() = default;

std::vector<dg::DG::HyperEdge>
DynamicDG::apply(const std::vector<std::shared_ptr<graph::Graph>>& hosts) {
	std::vector<const lib::Graph::Single *> hostGraphs;
	for (const auto& h : hosts) {
		hostGraphs.push_back(&h->getGraph());
	}
	auto innerRes = p->ddg.apply(hostGraphs);
	std::vector<dg::DG::HyperEdge> res;
	const auto &nonHyper = dg->getNonHyper();
	const auto &hyper = dg->getHyper();
	for(const auto &rp : innerRes)
		res.push_back(hyper.getInterfaceEdge(nonHyper.getHyperEdge(rp)));

	return res;
}

std::vector<dg::DG::HyperEdge>
DynamicDG::applyAndCache(const std::vector<std::shared_ptr<graph::Graph>>& hosts) {
	std::vector<const lib::Graph::Single *> hostGraphs;
	for (const auto& h : hosts) {
		hostGraphs.push_back(&h->getGraph());
	}
	auto innerRes = p->ddg.applyAndCache(hostGraphs);
	std::vector<dg::DG::HyperEdge> res;
	const auto &nonHyper = dg->getNonHyper();
	const auto &hyper = dg->getHyper();
	for(const auto &rp : innerRes)
		res.push_back(hyper.getInterfaceEdge(nonHyper.getHyperEdge(rp)));

	return res;
}

}
