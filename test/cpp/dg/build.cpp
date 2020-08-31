#include <mod/Config.hpp>
#include <mod/Derivation.hpp>
#include <mod/Error.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/dg/Builder.hpp>
#include <mod/dg/DG.hpp>

#undef NDEBUG

#include <cassert>

using namespace mod;

void testBuildRAII() {
	auto g = graph::Graph::smiles("O");
	auto d = Derivations{{g},
	                     {},
	                     {g}};
	auto dg = mod::dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Specialisation}, {},
	                            IsomorphismPolicy::Check);
	auto b = dg->build();
	assert(b.isActive());
	b.addDerivation(d);
	auto b2 = std::move(b);
	assert(!b.isActive());
	assert(b2.isActive());
	b2.addDerivation(d);
	try {
		b.addDerivation(d);
		std::exit(1);
	} catch(const LogicError &e) {
		assert(e.what() == std::string("The builder is not active."));
	}
}

void testExecuteNull() {
	auto dg = mod::dg::DG::make(LabelSettings{LabelType::String,
	                                          LabelRelation::Specialisation}, {},
	                            IsomorphismPolicy::Check);
	try {
		dg->build().execute(nullptr);
		std::exit(1);
	} catch(const LogicError &e) {
		assert(e.what() == std::string("The strategy may not be a null pointer."));
	}
}

int main() {
	testBuildRAII();
	testExecuteNull();
}