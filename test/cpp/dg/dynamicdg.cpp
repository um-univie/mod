#include <mod/Config.hpp>
#include <mod/Derivation.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/dg/Builder.hpp>
#include <mod/dg/DG.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/graph/Printer.hpp>
#include <mod/dg/Printer.hpp>
#include <mod/statespace/DynamicDG.hpp>

#include <iostream>
#include <set>

#undef NDEBUG

#include <cassert>

using namespace mod;

void testApply3() {
	auto r1 = rule::Rule::ruleGMLString(R"(
	                                    rule [
	                                        ruleID "r_{1645}"
	                                        left [
	                                            node [ id 6 label "O-" ]
	                                            node [ id 10 label "C" ]
	                                            edge [ source 0 target 1 label "-" ]
	                                            edge [ source 8 target 10 label "-" ]
	                                            edge [ source 8 target 13 label "-" ]
	                                            edge [ source 13 target 14 label "-" ]
	                                        ]
	                                        context [
	                                            node [ id 0 label "H" ]
	                                            node [ id 1 label "O" ]
	                                            node [ id 2 label "Amino{C, Glu, *, *}" ]
	                                            node [ id 3 label "C" ]
	                                            node [ id 4 label "C" ]
	                                            node [ id 5 label "C" ]
	                                            node [ id 7 label "O" ]
	                                            node [ id 8 label "C" ]
	                                            node [ id 9 label "C" ]
	                                            node [ id 11 label "C" ]
	                                            node [ id 12 label "O" ]
	                                            node [ id 13 label "O" ]
	                                            node [ id 14 label "H" ]
	                                            edge [ source 2 target 3 label "-" ]
	                                            edge [ source 3 target 4 label "-" ]
	                                            edge [ source 4 target 5 label "-" ]
	                                            edge [ source 5 target 6 label "-" ]
	                                            edge [ source 5 target 7 label "=" ]
	                                            edge [ source 8 target 9 label "-" ]
	                                            edge [ source 10 target 11 label "-" ]
	                                            edge [ source 10 target 12 label "-" ]
	                                        ]
	                                        right [
	                                            node [ id 6 label "O" ]
	                                            node [ id 10 label "C-" ]
	                                            edge [ source 0 target 6 label "-" ]
	                                            edge [ source 1 target 14 label "-" ]
	                                            edge [ source 8 target 13 label "=" ]
	                                        ]
	                                    ]
	                                   )", false);
	auto r2 = rule::Rule::ruleGMLString(R"(
	                                    rule [
	                                        ruleID "r_{2055}"
	                                        left [
	                                            edge [ source 1 target 5 label "-" ]
	                                            edge [ source 2 target 4 label "=" ]
	                                        ]
	                                        context [
	                                            node [ id 0 label "C" ]
	                                            node [ id 1 label "O" ]
	                                            node [ id 2 label "C" ]
	                                            node [ id 3 label "C" ]
	                                            node [ id 4 label "O" ]
	                                            node [ id 5 label "H" ]
	                                            edge [ source 0 target 1 label "-" ]
	                                            edge [ source 2 target 3 label "-" ]
	                                        ]
	                                        right [
	                                            edge [ source 1 target 2 label "-" ]
	                                            edge [ source 2 target 4 label "-" ]
	                                            edge [ source 4 target 5 label "-" ]
	                                        ]
	                                    ]
	                                   )", false);
	auto r3 = rule::Rule::ruleGMLString(R"(
	                                    rule [
	                                        ruleID "r_{3664}"
	                                        left [
	                                            edge [ source 2 target 3 label "-" ]
	                                            edge [ source 2 target 5 label "-" ]
	                                            edge [ source 3 target 4 label "=" ]
	                                        ]
	                                        context [
	                                            node [ id 0 label "C" ]
	                                            node [ id 1 label "C" ]
	                                            node [ id 2 label "N" ]
	                                            node [ id 3 label "C" ]
	                                            node [ id 4 label "O" ]
	                                            node [ id 5 label "H" ]
	                                            edge [ source 0 target 2 label "-" ]
	                                            edge [ source 1 target 3 label "-" ]
	                                        ]
	                                        right [
	                                            edge [ source 2 target 3 label "=" ]
	                                            edge [ source 3 target 4 label "-" ]
	                                            edge [ source 4 target 5 label "-" ]
	                                        ]
	                                    ]
	                                   )", false);

	auto g1 = graph::Graph::graphDFS("[H][O][H]");
	auto g2 = graph::Graph::graphDFS("[C]1([C]2=[C]([N]=[C]([N](1)[H])[N]([H])[H])[N]([C]([C](=[N]2)[C]([C]([O][H])([C]([O][H])([H])[H])[H])([O][H])[H])([H])[H])[H])=[O]");
	auto g3 = graph::Graph::graphDFS("[Amino{C, Glu, *, *}][C]([H])([H])[C]([H])([H])[C]([O-])=[O]");
	auto g4 = graph::Graph::graphDFS("[N]([C]([H])([H])[C]([H])([H])[C]([H])([H])[C]([Amino{C, Lys, *, *}])([H])[H])([H])[H]");

	auto g5 = graph::Graph::graphDFS("[C]([H])(=[O])[C]([H])([H])[O][H]");
	auto g6 = graph::Graph::graphDFS("[H][O][C]([C]([H])([H])[C]([H])([H])[Amino{C, Glu, *, *}])=[O]");
	auto g7 = graph::Graph::graphDFS("[C]1([C]2=[C]([N]=[C]([N](1)[H])[N]([H])[H])[N]([C]([C](=[N]2)[C-]([H])[O][H])([H])[H])[H])=[O]");

	auto g8 = graph::Graph::graphDFS("[C]([H])(=[O])[C]([H])([H])[O][C]([O][H])([C]([H])([H])[C]([H])([H])[Amino{C, Glu, *, *}])[O][H]");


	std::vector<std::shared_ptr<graph::Graph>> gs({g1,g2,g3,g4});
	std::vector<std::shared_ptr<graph::Graph>> gs1({g1,g4,g5,g6,g7});
	std::vector<std::shared_ptr<graph::Graph>> gs2({g1,g4,g7, g8});
	std::vector<std::shared_ptr<rule::Rule>> rs({r1,r2,r3});

	auto dg = dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism},
	                       {g1,g2,g3,g4,g5,g6,g7,g8}, IsomorphismPolicy::Check);
	auto builder = std::make_shared<dg::Builder>(dg->build());

	mod::statespace::DynamicDG ddg(builder, rs);


	auto ders = ddg.apply(gs);
	std::set<mod::dg::DG::HyperEdge> der_set(ders.begin(), ders.end());
	std::cout << "FOUND " << der_set.size() << "\n";
	assert(der_set.size() == 11);

	std::cout << "---------------------------------------" << std::endl;

	ders = ddg.apply(gs1);
	der_set = std::set<mod::dg::DG::HyperEdge>(ders.begin(), ders.end());
	std::cout << "FOUND " << der_set.size() << "\n";
	assert(der_set.size() == 5);

	std::cout << "---------------------------------------" << std::endl;

	ders = ddg.apply(gs2);
	der_set = std::set<mod::dg::DG::HyperEdge>(ders.begin(), ders.end());
	std::cout << "FOUND " << der_set.size() << "\n";
	assert(der_set.size() == 3);


}

void testReactionCenter() {
	auto g = graph::Graph::smiles("[C][C][C]");

	auto r = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                        ruleID "AddEdge"
	                                        left [ node [ id 0 label "C" ] ]
	                                        context [  node [ id 1 label "C" ] edge [ source 0 target 1 label "-" ] ]
	                                        right [ node [ id 0 label "A"] ]
	                                   ]
	                                   )", false);

	auto dg = dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism},
	                       {g}, IsomorphismPolicy::Check);
	auto builder = std::make_shared<dg::Builder>(dg->build());

	mod::statespace::DynamicDG ddg(builder, {r});

	auto ders = ddg.apply({g});
	auto der_set = std::set<mod::dg::DG::HyperEdge>(ders.begin(), ders.end());
	std::cout << "FOUND " << der_set.size() << "\n";
	assert(der_set.size() == 2);
}

void testReactionCenter2() {
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
	auto builder = std::make_shared<dg::Builder>(dg->build());

	mod::statespace::DynamicDG ddg(builder, {r});

	auto ders = ddg.apply({g});
	auto der_set = std::set<mod::dg::DG::HyperEdge>(ders.begin(), ders.end());
	std::cout << "FOUND " << der_set.size() << "\n";
	assert(der_set.size() == 5);
}

void testMultipleDups() {
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
	auto builder = std::make_shared<dg::Builder>(dg->build());

	mod::statespace::DynamicDG ddg(builder, {r});

	auto ders = ddg.apply(gs);
	auto der_set = std::set<mod::dg::DG::HyperEdge>(ders.begin(), ders.end());
	std::cout << "FOUND " << der_set.size() << "\n";
	assert(der_set.size() == 5);
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

void testDDG() {
	auto gPath1 = graph::Graph::smiles("[C]");
	auto gPath2 = graph::Graph::smiles("[C][C]");
	auto gPath3 = graph::Graph::smiles("[C][C][C]");

	auto r = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                        ruleID "AddEdge"
	                                        left [ ]
	                                        context [ node [ id 0 label "C" ] node [ id 1 label "C" ] ]
	                                        right [ edge [ source 0 target 1 label "-" ] ]
	                                   ]
	                                   )", false);

	std::vector<std::shared_ptr<graph::Graph>> gs;
	std::vector<std::shared_ptr<rule::Rule>> rs;

	rs.push_back(r);
	gs.push_back(gPath2);

//	mod::statespace::DynamicDG ddg({gPath1, gPath2, gPath3}, rs,
//	                               LabelSettings{LabelType::String, LabelRelation::Isomorphism});
	auto dg = dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism},
	                       {gPath1, gPath2, gPath3}, IsomorphismPolicy::Check);
	auto builder = std::make_shared<dg::Builder>(dg->build());
	mod::statespace::DynamicDG ddg(builder, rs);


	auto ders = ddg.apply(gs);
	std::cout << "FOUND " << ders.size() << "\n";
	assert(ders.size() == 0);

	gs.clear();
	gs.push_back(gPath1);
	gs.push_back(gPath1);
	ders = ddg.apply(gs);
	std::cout << "FOUND " << ders.size() << "\n";
	assert(ders.size() == 1);

	std::cout << "---------" << std::endl;
	gs.clear();
	gs.push_back(gPath1);
	gs.push_back(gPath3);
	ders = ddg.apply(gs);
	std::cout << "FOUND " << ders.size() << "\n";
	assert(ders.size() == 3);

	std::cout << "---------" << std::endl;
	gs.clear();
	gs.push_back(gPath1);
	gs.push_back(gPath1);
	gs.push_back(gPath2);
	ders = ddg.apply(gs);
	std::cout << "FOUND " << ders.size() << "\n";
	assert(ders.size() == 2);
}

void testError() {
	auto gPath1 = graph::Graph::smiles("[C]");
	auto gPath2 = graph::Graph::smiles("[C][C]");
	auto gPath3 = graph::Graph::smiles("[C][C][C]");

	auto r = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                        ruleID "AddEdge"
	                                        left [ ]
	                                        context [ node [ id 0 label "C" ] node [ id 1 label "C" ] ]
	                                        right [ edge [ source 0 target 1 label "-" ] ]
	                                   ]
	                                   )", false);

	std::vector<std::shared_ptr<graph::Graph>> gs;
	std::vector<std::shared_ptr<rule::Rule>> rs;

	rs.push_back(r);
	gs.push_back(gPath2);

//	mod::statespace::DynamicDG ddg({gPath1, gPath2, gPath3}, rs,
//	                               LabelSettings{LabelType::String, LabelRelation::Isomorphism});
	auto dg = dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism},
	                       {gPath1, gPath2, gPath3}, IsomorphismPolicy::Check);
	auto builder = std::make_shared<dg::Builder>(dg->build());
	mod::statespace::DynamicDG ddg(builder, rs);


	gs = {gPath1, gPath1, gPath1, gPath1};
	auto ders = ddg.apply(gs);
	std::cout << "FOUND " << ders.size() << "\n";
	assert(ders.size() == 1);

	std::cout << "---------" << std::endl;
	gs = {gPath1, gPath1, gPath2};
	ders = ddg.apply(gs);
	std::cout << "FOUND " << ders.size() << "\n";
	assert(ders.size() == 2);
}

void testDDGCache() {
	auto gPath1 = graph::Graph::smiles("[C]");
	auto gPath2 = graph::Graph::smiles("[C][C]");
	auto gPath3 = graph::Graph::smiles("[C][C][C]");

	auto r = rule::Rule::ruleGMLString(R"(
	                                   rule [
	                                        ruleID "AddEdge"
	                                        left [ ]
	                                        context [ node [ id 0 label "C" ] node [ id 1 label "C" ] ]
	                                        right [ edge [ source 0 target 1 label "-" ] ]
	                                   ]
	                                   )", false);

	std::vector<std::shared_ptr<graph::Graph>> gs;
	std::vector<std::shared_ptr<rule::Rule>> rs;

	rs.push_back(r);
	gs.push_back(gPath2);

//	mod::statespace::DynamicDG ddg({gPath1, gPath2, gPath3}, rs,
//	                               LabelSettings{LabelType::String, LabelRelation::Isomorphism});
	auto dg = dg::DG::make(LabelSettings{LabelType::String, LabelRelation::Isomorphism},
	                       {gPath1, gPath2, gPath3}, IsomorphismPolicy::Check);
	auto builder = std::make_shared<dg::Builder>(dg->build());
	mod::statespace::DynamicDG ddg(builder, rs);

	auto ders = ddg.applyAndCache(gs);
	std::cout << "FOUND " << ders.size() << "\n";
	assert(ders.size() == 0);

	gs.clear();
	gs.push_back(gPath1);
	gs.push_back(gPath1);
	ders = ddg.applyAndCache(gs);
	std::cout << "FOUND " << ders.size() << "\n";
	assert(ders.size() == 2);

	std::cout << "---------" << std::endl;
	gs.clear();
	gs.push_back(gPath1);
	gs.push_back(gPath3);
	ders = ddg.applyAndCache(gs);
	std::cout << "FOUND " << ders.size() << "\n";
	assert(ders.size() == 8);

	std::cout << "------------------" << std::endl;
	gs.clear();
	gs.push_back(gPath1);
	gs.push_back(gPath1);
	gs.push_back(gPath1);
	ders = ddg.applyAndCache(gs);
	std::cout << "FOUND " << ders.size() << "\n";

	std::cout << "------------------" << std::endl;
	gs.clear();
	gs.push_back(gPath2);
	gs.push_back(gPath1);
	ders = ddg.applyAndCache(gs);
	std::cout << "FOUND " << ders.size() << "\n";
}

int main() {
//	testApply2();
	testDDG();
	//testDDGCache();
	testApply3();

	testError();
	testReactionCenter();
	testReactionCenter2();
	testMultipleDups();
}
