#include <mod/Config.hpp>
#include <mod/Derivation.hpp>
#include <mod/Error.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/dg/Builder.hpp>
#include <mod/dg/DG.hpp>
#include <mod/rule/Rule.hpp>
#include <iostream>

#undef NDEBUG

#include <cassert>

using namespace mod;

void testApplyV2(){
	auto dg = mod::dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism}, {},
	                            IsomorphismPolicy::Check);
	auto b = dg->build();
	auto g = graph::Graph::smiles("[C][C]");
	auto path3 = graph::Graph::smiles("[C][C][C]");
	auto singleton = graph::Graph::smiles("[C]");
	auto r = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                   ruleID "AddEdge"
	                                   left [ ]
	                                   context [ node [ id 0 label "C" ] node [ id 1 label "C" ] ]
	                                   right [ edge [ source 0 target 1 label "-" ] ]
	                                   ]
	                                   )", false);


	std::cout << "applying singleton\n";
	auto ders = b.apply_v2({singleton, singleton},  r);
}

void testBuildRAII() {
	auto g = graph::Graph::fromSMILES("O");
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
	//testBuildRAII();
	//testExecuteNull();
	testApplyV2();
}
