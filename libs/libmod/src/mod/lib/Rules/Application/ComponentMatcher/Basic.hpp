#ifndef MOD_LIB_RULES_APPLICATION_COMPONENTMATCHER_HPP
#define MOD_LIB_RULES_APPLICATION_COMPONENTMATCHER_HPP

#include <mod/lib/Rules/Real.hpp>
#include <mod/lib/Graph/Single.hpp>

namespace mod::lib::Rules::Application::CompnentMatcher {

class Basic {
	bool isValid(const std::vector<const Graph::Single*>& subset,
	             const std::vector<const Graph::Single*>& universe);


};

}

#endif
