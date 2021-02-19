#include <mod/lib/Rules/Application/Enumerate.hpp>
#include <iostream>


namespace mod::lib::Rules::Application {

std::vector<std::unique_ptr<Rules::Real>> computeDerivations(const Rules::Real& rule,
                                                             const std::vector<const Graph::Single*>& subset,
                                                             const std::vector<const Graph::Single*>& universe) {

	std::vector<std::unique_ptr<Rules::Real>> derivations;
	std::cout << "Now I'm here" << std::endl;
	return derivations;
}

}
