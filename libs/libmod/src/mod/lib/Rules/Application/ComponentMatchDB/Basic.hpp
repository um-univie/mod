#ifndef MOD_LIB_RULES_APPLICATION_COMPONENTMATCHER_BASIC_HPP
#define MOD_LIB_RULES_APPLICATION_COMPONENTMATCHER_BASIC_HPP

#include <mod/lib/Rules/Application/ComponentMatch.hpp>
#include <mod/lib/Rules/Real.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/Config.hpp>

namespace mod::lib::Rules::Application::ComponentMatchDB {

class Basic {
public:
	Basic(const LabelSettings& labelSettings);
	bool isValid(const Real& rule,
	             size_t numSubsetGraphs,
	             const std::vector<const Graph::Single*>& universe);

	//std::map<std::pair<const Graph::Single*, size_t>, std::vector<ComponentMatch>>
	std::vector<ComponentMatch>
	getMatches(const Real& rule,
	           size_t numSubsetGraphs,
	           const std::vector<const Graph::Single*>& universe);

private:
	LabelSettings labelSettings;
	std::map<std::pair<const Graph::Single*, size_t>, std::vector<ComponentMatch::Morphism>> matches;
};

}

#endif
