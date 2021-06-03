#include <mod/Config.hpp>
#include <mod/Derivation.hpp>
#include <mod/Error.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/dg/Builder.hpp>
#include <mod/dg/DG.hpp>
#include <mod/rule/Rule.hpp>
#include <iostream>
#include <set>
#include <mod/dg/Strategies.hpp>

#undef NDEBUG

#include <cassert>

using namespace mod;

void testExecute(){
	auto path2 = graph::Graph::smiles("[C][C]");
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
	auto r2 = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                        ruleID "AddEdge"
	                                        left [ node [ id 0 label "C" ] ]
	                                        context [  node [ id 1 label "C" ] edge [ source 0 target 1 label "-" ] ]
	                                        right [ node [ id 0 label "A"] ]
	                                   ]
	                                   )", false);

	auto dg = mod::dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism},
	                            {singleton, path2, path3},
	                            IsomorphismPolicy::Check);
	mod::getConfig().dg.useExplicitMatchRuleApplication.set(true);
	auto b = dg->build();

	auto policy = IsomorphismPolicy::Check;
	auto strat = dg::Strategy::makeSequence({
	        dg::Strategy::makeAdd(false, {singleton}, policy),
	        dg::Strategy::makeAdd(true, {path2}, policy),
	        dg::Strategy::makeRule(r)});


	std::cout << "---------------------------" << std::endl;
	std::cout << "  TEST: EXECUTE (SIMPLE)  " << std::endl;
	std::cout << "---------------------------" << std::endl;

	auto res = b.execute(strat);
	std::cout << "EDGES " << dg->numEdges() << std::endl;
	assert(dg->numEdges() == 2);
}

void testIsoLOrder() {
	auto host1 = graph::Graph::graphDFS("C-C");
	auto host2 = graph::Graph::graphDFS("C=C");
	auto host3 = graph::Graph::graphDFS("[C]1([C]([C](1)[Y])[Z])[X]");
	auto host4 = graph::Graph::graphDFS("[H][C]");

	auto r = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                   ruleID "AddEdge"
	                                   left [ ]
	                                   context [
	                                   node [ id 0 label "C" ] node [ id 1 label "C" ]
	                                   ]
	                                   right [
	                                   ]
	                                   ]
	                                   )", false);
	auto r2 = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                   ruleID "AddEdge"
	                                   left [ ]
	                                   context [
	                                       node [ id 0 label "C" ]
	                                       node [ id 1 label "C" ]
	                                       node [ id 2 label "C" ]
	                                   ]
	                                   right [
	                                   ]
	                                   ]
	                                   )", false);
	auto r3 = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                   ruleID "AddEdge"
	                                   left [ ]
	                                   context [
	                                   node [ id 0 label "H" ] node [ id 1 label "H" ]
	                                   node [ id 2 label "C" ] node [ id 3 label "C" ]
	                                   ]
	                                   right [
	                                   ]
	                                   ]
	                                   )", false);

	auto dg = mod::dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism},
	                            {host1, host2},
	                            IsomorphismPolicy::Check);
	auto b = dg->build();
	mod::getConfig().application.useCanonicalMatchesExhaustive.set(true);
	mod::getConfig().application.useCanonicalMatchesCombined.set(true);
	mod::getConfig().application.useIsoCompPruning.set(true);

	std::cout << "test to check host graph generation in the case that you have isomorphic comps in L" << std::endl;
	auto ders = b.apply_v2({host2, host1},  r);
	std::cout << "found ders " << ders.size() << std::endl;

	std::cout << "test to check host graph generation in the case that you have isomorphic comps in L" << std::endl;
	ders = b.apply_v2({host3},  r2);
	std::cout << "found ders " << ders.size() << std::endl;

	std::cout << "test to check iso L total order when x < y" << std::endl;
	ders = b.apply_v2({host4, host4},  r3);
	std::cout << "found ders " << ders.size() << std::endl;
}

void testIsoL() {
	auto hostGraph = graph::Graph::graphGMLString(R"(
	                                              graph [
	                                                  node [ id 0 label "C" ]
	                                                  node [ id 3 label "C" ]
	                                                  node [ id 2 label "C" ]
	                                                  node [ id 1 label "C" ]
	                                                  node [ id 4 label "F" ]
	                                                  node [ id 5 label "H" ]
	                                                  node [ id 6 label "H" ]
	                                                  node [ id 7 label "F" ]
	                                                  edge [ source 0 target 3 label "-" ]
	                                                  edge [ source 0 target 5 label "-" ]
	                                                  edge [ source 3 target 4 label "-" ]
	                                                  edge [ source 3 target 5 label "-" ]
	                                                  edge [ source 5 target 6 label "-" ]
	                                                  edge [ source 1 target 6 label "-" ]
	                                                  edge [ source 2 target 1 label "-" ]
	                                                  edge [ source 2 target 6 label "-" ]
	                                                  edge [ source 1 target 7 label "-" ]
	                                              ]
	                                              )");
	auto r = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                   ruleID "AddEdge"
	                                   left [ ]
	                                   context [
	                                   node [ id 0 label "C" ] node [ id 1 label "C" ]
	                                   edge [ source 0 target 1 label "-" ]
	                                   ]
	                                   right [
	                                   ]
	                                   ]
	                                   )", false);
	auto dg = mod::dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism},
	                            {hostGraph},
	                            IsomorphismPolicy::Check);
	auto b = dg->build();
	mod::getConfig().application.useCanonicalMatchesExhaustive.set(true);
	mod::getConfig().application.useCanonicalMatchesCombined.set(true);


	std::cout << "---------------------------" << std::endl;
	std::cout << "  TEST: APPLY V2 (RULE L ISOS)  " << std::endl;
	std::cout << "---------------------------" << std::endl;

	std::cout << "test: C + C -> C-C" << std::endl;
	auto ders = b.apply_v2({hostGraph},  r);
	std::cout << "found ders " << ders.size() << std::endl;
	assert(ders.size() == 1);

}

void testApplyV2(){
	auto path2 = graph::Graph::smiles("[C][C]");
	auto path3 = graph::Graph::smiles("[C][C][C]");
	auto singleton = graph::Graph::smiles("[C]");
	auto triangle = graph::Graph::graphGMLString(R"(
	                                             graph [
	                                             node [ id 0 label "C" ]
	                                             node [ id 1 label "C" ]
	                                             node [ id 2 label "C" ]
	                                             edge [ source 0 target 1 label "-" ]
	                                             edge [ source 0 target 2 label "-" ]
	                                             edge [ source 1 target 2 label "-" ]
	                                             ]
	                                             )");
	auto r = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                   ruleID "AddEdge"
	                                   left [ ]
	                                   context [ node [ id 0 label "C" ] node [ id 1 label "C" ] ]
	                                   right [ edge [ source 0 target 1 label "-" ] ]
	                                   ]
	                                   )", false);
	auto r2 = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                        ruleID "AddEdge"
	                                        left [ node [ id 0 label "C" ] ]
	                                        context [  node [ id 1 label "C" ] edge [ source 0 target 1 label "-" ] ]
	                                        right [ node [ id 0 label "A"] ]
	                                   ]
	                                   )", false);
	auto r3 = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                   ruleID "AddEdge"
	                                   left [ ]
	                                   context [
	                                    node [ id 0 label "C" ]
	                                    node [ id 1 label "C" ]
	                                    node [ id 2 label "C" ]
	                                    edge [ source 0 target 2 label "-" ]
	                                    edge [ source 1 target 2 label "-" ]
	                                   ]
	                                   right [
	                                    edge [ source 0 target 1 label "-" ]
	                                   ]
	                                   ]
	                                   )", false);
	auto r4 = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                        ruleID "AddEdge"
	                                        left [ node [ id 0 label "C" ] ]
	                                        context []
	                                        right []
	                                   ]
	                                   )", false);
	auto r5 = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                   ruleID "AddEdge"
	                                   left [ edge [ source 0 target 1 label "-" ] ]
	                                   context [ node [ id 0 label "C" ] node [ id 1 label "C" ] ]
	                                   right [ ]
	                                   ]
	                                   )", false);

	auto dg = mod::dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism},
	                            {singleton, path2, path3},
	                            IsomorphismPolicy::Check);
	auto b = dg->build();


	std::cout << "---------------------------" << std::endl;
	std::cout << "  TEST: APPLY V2 (SIMPLE)  " << std::endl;
	std::cout << "---------------------------" << std::endl;

	std::cout << "test: C + C -> C-C" << std::endl;
	auto ders = b.apply_v2({singleton, singleton},  r);
	std::cout << "  found: " << ders.size() << " derivations" << std::endl;
	assert(ders.size() == 1);

	std::cout << "-------\n";
	std::cout << "test: C-C -!> C-C" << std::endl;

	ders = b.apply_v2({path2},  r);
	std::cout << "  found: " << ders.size() << " derivations" << std::endl;
	assert(ders.size() == 0);

	std::cout << "-------\n";
	std::cout << "test: C-C-C with rule C-C -> C-A" << std::endl;
	ders = b.apply_v2({path3}, r2);
	std::cout << "  found: " << ders.size() << " derivations" << std::endl;
	auto der_set = std::set<mod::dg::DG::HyperEdge>(ders.begin(), ders.end());
	assert(der_set.size() == 2);

	std::cout << "-------\n";
	std::cout << "test: triangle with rule C-C-C -> triangle" << std::endl;
	ders = b.apply_v2({triangle}, r3);
	std::cout << "  found: " << ders.size() << " derivations" << std::endl;
	assert(ders.size() == 0);

	std::cout << "-------\n";
	std::cout << "test: C-C with rule C -> " << std::endl;
	ders = b.apply_v2({path2}, r4);
	std::cout << "  found: " << ders.size() << " derivations" << std::endl;
	assert(ders.size() == 0);

	std::cout << "-------\n";
	std::cout << "test: C-C with rule C-C -> C C " << std::endl;
	ders = b.apply_v2({path2}, r5);
	std::cout << "  found: " << ders.size() << " derivations" << std::endl;
	der_set = std::set<mod::dg::DG::HyperEdge>(ders.begin(), ders.end());
	assert(der_set.size() == 1);
}

void testApplyV2ReactionCenter() {
	auto g = graph::Graph::graphDFS("[C]1([N]([C](=[N][C]2=[C]([N]=[C]([C]([N](2)[H])([H])[H])[C]([C]([O][H])([C]([O][H])([H])[H])[H])([H])[O-])1)[N]([H])[H])[H])([O][C]([C]([C]([Amino{C, Glu, *, *}])([H])[H])([H])[H])=[O])[O][H]");

	auto r = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                       ruleID "r_{6169}"
	                                       left [
	                                           node [ id 4 label "O" ]
	                                           node [ id 6 label "N" ]
	                                           edge [ source 2 target 4 label "=" ]
	                                       ]
	                                       context [
	                                           node [ id 0 label "C" ]
	                                           node [ id 1 label "O" ]
	                                           node [ id 2 label "C" ]
	                                           node [ id 3 label "C" ]
	                                           node [ id 5 label "C" ]
	                                           edge [ source 0 target 1 label "-" ]
	                                           edge [ source 1 target 2 label "-" ]
	                                           edge [ source 2 target 3 label "-" ]
	                                           edge [ source 5 target 6 label "-" ]
	                                       ]
	                                       right [
	                                           node [ id 4 label "O-" ]
	                                           node [ id 6 label "N+" ]
	                                           edge [ source 2 target 4 label "-" ]
	                                           edge [ source 2 target 6 label "-" ]
	                                       ]
	                                   ]

	                                   )", false);

	auto dg = dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism},
	                       {g}, IsomorphismPolicy::Check);
	auto b = dg->build();

	auto ders = b.apply_v2({g}, r);
	auto der_set = std::set<mod::dg::DG::HyperEdge>(ders.begin(), ders.end());
	std::cout << "FOUND " << der_set.size() << "\n";
	assert(der_set.size() == 5);
}

void testMultipleDupsApplyV2() {
	auto gs = {graph::Graph::graphDFS("[H][O][H]"),
	           graph::Graph::graphDFS("[N]([C]([H])([H])[C]([H])([H])[C]([H])([H])[C]([Amino{C, Lys, *, *}])([H])[H])([H])[H]"),
	           graph::Graph::graphDFS("[Amino{C, Glu, *, *}][C]([H])([H])[C]([H])([H])[C]([O-])=[O]"),
	           graph::Graph::graphDFS("[C]1([C]2=[C]([N]=[C]([N](1)[H])[N]([H])[H])[N]([C]([C]([C]([C]([O][H])([C]([O][H])([H])[H])[H])([H])[O-])=[N+]([H])2)([H])[H])[H])=[O]")
	          };

	auto r = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                       ruleID "r_{1648}"
	                                       left [
	                                           node [ id 5 label "O-" ]
	                                           node [ id 7 label "N+" ]
	                                           edge [ source 0 target 12 label "-" ]
	                                           edge [ source 7 target 10 label "=" ]
	                                           edge [ source 13 target 14 label "-" ]
	                                       ]
	                                       context [
	                                           node [ id 0 label "O" ]
	                                           node [ id 1 label "Amino{C, Glu, *, *}" ]
	                                           node [ id 2 label "C" ]
	                                           node [ id 3 label "C" ]
	                                           node [ id 4 label "C" ]
	                                           node [ id 6 label "O" ]
	                                           node [ id 8 label "C" ]
	                                           node [ id 9 label "C" ]
	                                           node [ id 10 label "C" ]
	                                           node [ id 11 label "C" ]
	                                           node [ id 12 label "H" ]
	                                           node [ id 13 label "O" ]
	                                           node [ id 14 label "H" ]
	                                           edge [ source 1 target 2 label "-" ]
	                                           edge [ source 2 target 3 label "-" ]
	                                           edge [ source 3 target 4 label "-" ]
	                                           edge [ source 4 target 5 label "-" ]
	                                           edge [ source 4 target 6 label "=" ]
	                                           edge [ source 7 target 8 label "-" ]
	                                           edge [ source 9 target 10 label "-" ]
	                                           edge [ source 10 target 11 label "-" ]
	                                       ]
	                                       right [
	                                           node [ id 5 label "O" ]
	                                           node [ id 7 label "N" ]
	                                           edge [ source 0 target 14 label "-" ]
	                                           edge [ source 5 target 12 label "-" ]
	                                           edge [ source 7 target 10 label "-" ]
	                                           edge [ source 10 target 13 label "-" ]
	                                       ]
	                                   ]

	                                   )", false);

	auto dg = dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism},
	                       gs, IsomorphismPolicy::Check);
	auto b = dg->build();

	auto ders = b.apply_v2(gs, r, true);
	auto der_set = std::set<mod::dg::DG::HyperEdge>(ders.begin(), ders.end());

	std::cout << "FOUND " << der_set.size() << std::endl;
	assert(der_set.size() == 5);
}

void testFormoseReactions() {
	/*
	SUBSET:
	C(C(C(CO)O)O)=O
	C(C(CO)O)=O
	UNIVERSE:
	C(O)=CO
	C(C(C(CO)O)O)=O
	C(C(CO)O)=O
	C=O
	C(CO)=O
	*/

	auto s1 = graph::Graph::smiles("C(C(C(CO)O)O)=O");
	auto s2 = graph::Graph::smiles("C(C(CO)O)=O");
	auto u1 = graph::Graph::smiles("C(O)=CO");
	auto u2 = graph::Graph::smiles("C=O");
	auto u3 = graph::Graph::smiles("C(CO)=O");
	auto r = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                       ruleID "Keto-enol isomerization ->"
	                                       left [
	                                           edge [ source 0 target 3 label "-" ]
	                                           edge [ source 0 target 1 label "-" ]
	                                           edge [ source 1 target 2 label "=" ]
	                                       ]
	                                       context [
	                                           node [ id 0 label "C" ]
	                                           node [ id 1 label "C" ]
	                                           node [ id 2 label "O" ]
	                                           node [ id 3 label "H" ]
	                                       ]
	                                       right [
	                                           edge [ source 2 target 3 label "-" ]
	                                           edge [ source 2 target 1 label "-" ]
	                                           edge [ source 1 target 0 label "=" ]
	                                       ]
	                                       constrainAdj [
	                                           id 1
	                                           op "="
	                                           count 1
	                                           nodeLabels [
	                                               label "O"
	                                           ]
	                                       ]
	                                   ]
	                                   )", false);

	auto dg = dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism},
	    {s1,s2,u1,u2,u3}, IsomorphismPolicy::Check);
	auto policy = IsomorphismPolicy::Check;
	auto strat = dg::Strategy::makeSequence({
	        dg::Strategy::makeAdd(false, {s1,s2}, policy),
	        dg::Strategy::makeAdd(true, {u1,u2,u3}, policy),
	        dg::Strategy::makeRule(r)});
	auto b = dg->build();

//	auto ders = b.apply_v2({g}, r, true);
//	for (const auto& d : ders) {
//		for (const auto v : d.targets()) {
//			std::cout << "TAR: " << v.getGraph()->getSmiles() << std::endl;
//		}
//	}

//	std::cout << "FOUND " << ders.size() << std::endl;
	//assert(der_set.size() == 5);

	std::cout << "---------------------------" << std::endl;
	std::cout << "  TEST: EXECUTE (KETO)  " << std::endl;
	std::cout << "---------------------------" << std::endl;

	auto res = b.execute(strat);
	std::cout << "EDGES " << dg->numEdges() << std::endl;
	//assert(dg->numEdges() == 2);
	for (const auto& d : dg->edges()) {
		for (const auto v : d.sources()) {
			std::cout << "," << v.getGraph()->getSmiles();
		}
		std::cout << " -> ";
		for (const auto v : d.targets()) {
			std::cout << "," << v.getGraph()->getSmiles();
		}
		std::cout << std::endl;
	}
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
	testBuildRAII();
	testExecuteNull();
	testApplyV2();
	testApplyV2ReactionCenter();
	testMultipleDupsApplyV2();
	testExecute();
	testFormoseReactions();
	testIsoL();
	testIsoLOrder();

}
