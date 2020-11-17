#ifndef MOD_STATESPACE_DYNAMIC_DG_HPP
#define MOD_STATESPACE_DYNAMIC_DG_HPP

#include <mod/dg/DG.hpp>

namespace mod::statespace {

class MOD_DECL DynamicDG {
public:
//	DynamicDG(const std::vector<std::shared_ptr<graph::Graph>>& graphDatabase,
//	          const std::vector<std::shared_ptr<rule::Rule>>& rules,
//	          LabelSettings labelSettings);

//	DynamicDG(std::shared_ptr<dg::DG> dg,
//	          const std::vector<std::shared_ptr<rule::Rule>>& rules
//	          );
	DynamicDG(std::shared_ptr<dg::Builder> builder,
	          const std::vector<std::shared_ptr<rule::Rule>>& rules
	          );
	~DynamicDG();


	std::vector<dg::DG::HyperEdge>
	apply(const std::vector<std::shared_ptr<graph::Graph>>& hosts);

	std::vector<dg::DG::HyperEdge>
	applyAndCache(const std::vector<std::shared_ptr<graph::Graph>>& hosts);

	static std::shared_ptr<DynamicDG> makeDynamicDG(std::shared_ptr<dg::Builder> dg,
	                                                const std::vector<std::shared_ptr<rule::Rule>>& rules
	                                                );

private:
	std::vector<std::shared_ptr<rule::Rule>> rules;
	std::shared_ptr<dg::DG> dg;
	std::shared_ptr<dg::Builder> builder;

private:
	struct Pimpl;
	std::unique_ptr<Pimpl> p;
};

}

#endif
