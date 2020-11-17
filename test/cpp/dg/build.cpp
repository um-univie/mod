#include <mod/Config.hpp>
#include <mod/Derivation.hpp>
#include <mod/Error.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/dg/Builder.hpp>
#include <mod/dg/DG.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/graph/Printer.hpp>
#include <mod/dg/Printer.hpp>

#include <iostream>

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

void testApply2() {
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
	std::vector<std::shared_ptr<graph::Graph>>  gs;
	gs.push_back(g);
	auto dg = mod::dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism}, {},
	                            IsomorphismPolicy::Check);
	{
	auto b = dg->build();
	std::cout << "applying edge\n";
	auto ders = b.apply2(gs,  r);
	std::cout << "FOUND " << ders.size() << "\n";
	assert(ders.size() == 0);

	std::cout << "applying singleton\n";
	gs.clear();
	gs.push_back(singleton);
	gs.push_back(singleton);
	ders = b.apply2(gs,  r);
	std::cout << "FOUND " << ders.size() << "\n";
	assert(ders.size() == 2);
	std::cout << "DONE WITH APPLY 2" << std::endl;

	std::cout << "------------------" << std::endl;
	gs.clear();
	gs.push_back(singleton);
	gs.push_back(path3);
	ders = b.apply2(gs,  r);
	assert(ders.size() > 0);
	std::cout << "FOUND " << ders.size() << "\n";
	}
}

int main() {
	testBuildRAII();
	testExecuteNull();
	testApply2();
}
