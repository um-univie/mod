#include "stuff.h"

namespace awesome {

std::shared_ptr<mod::Graph> doStuff() {
	auto g = mod::Graph::smiles("CCO");
	g->setName("Ethanol");
	return g;
}

} // namespace awesome
