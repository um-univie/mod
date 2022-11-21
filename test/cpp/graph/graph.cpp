#include <mod/Error.hpp>
#include <mod/graph/Graph.hpp>

#undef NDEBUG

#include <cassert>

using namespace mod;

int main() {
	auto dom = graph::Graph::fromSMILES("O");
	auto codom = dom;
	try {
		dom->enumerateMonomorphisms(dom, {}, LabelSettings(LabelType::String, LabelRelation::Isomorphism));
		assert(false);
	} catch(const LogicError &e) {
		assert(e.what() == std::string("callback is null."));
	}
}