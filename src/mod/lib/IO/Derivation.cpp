#include "Derivation.h"

#include <mod/Config.h>
#include <mod/Rule.h>
#include <mod/lib/Algorithm.h>
#include <mod/lib/DG/Hyper.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/GraphMorphism/LabelledMorphism.h>
#include <mod/lib/GraphMorphism/VF2Finder.hpp>
#include <mod/lib/IO/FileHandle.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/MatchConstraint.h>
#include <mod/lib/IO/Rule.h>
#include <mod/lib/RC/ComposeRuleReal.h>
#include <mod/lib/RC/MatchMaker/Super.h>
#include <mod/lib/Rules/Real.h>

#include <jla_boost/graph/morphism/Predicates.hpp>

#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace IO {
namespace Derivation {
namespace Write {
namespace {
namespace GM = jla_boost::GraphMorphism;

std::vector<lib::Rules::Real*> findCompleteRules(const lib::DG::NonHyper &dg, lib::DG::HyperVertex v) {
	const lib::DG::HyperGraphType &dgGraph = dg.getHyper().getGraph();
	assert(v != dgGraph.null_vertex());
	using Vertex = lib::DG::HyperVertex;
	assert(dgGraph[v].kind == lib::DG::HyperVertexKind::Edge);
	assert(dgGraph[v].rules.size() == 1);
	const lib::Rules::Real &rReal = dgGraph[v].rules.front()->getAPIReference()->getRule();
	lib::Graph::Merge eductMerge, productMerge;
	for(Vertex vAdj : asRange(inv_adjacent_vertices(v, dgGraph)))
		eductMerge.mergeWith(*dgGraph[vAdj].graph);
	for(Vertex vAdj : asRange(adjacent_vertices(v, dgGraph)))
		productMerge.mergeWith(*dgGraph[vAdj].graph);
	eductMerge.lock();
	productMerge.lock();
	std::shared_ptr<mod::Rule> identifyL = lib::Rules::Real::createSide(eductMerge.getGraph(), eductMerge.getStringState(), lib::Rules::Membership::Context, "G");
	std::shared_ptr<mod::Rule> identifyR = lib::Rules::Real::createSide(productMerge.getGraph(), productMerge.getStringState(), lib::Rules::Membership::Context, "H");
	std::vector<lib::Rules::Real*> matchingLR;
	{
		std::vector<lib::Rules::Real*> matchingL;
		{
			//			IO::log() << "Derivation: compose identifyL -> rReal" << std::endl;
			auto reporter = [&matchingL, &dg] (std::unique_ptr<lib::Rules::Real> r) {
				auto *rPtr = r.release();
				auto p = findAndInsert(matchingL, rPtr, lib::Rules::makeIsomorphismPredicate());
				if(!p.second) delete rPtr;
			};
			lib::RC::Super mm(false, true);
			lib::RC::composeRuleRealByMatchMaker(identifyL->getRule(), rReal, mm, reporter);
		}
		for(auto *r : matchingL) {
			//			IO::log() << "Derivation: compose matchingL -> identifyR" << std::endl;
			auto reporter = [&matchingLR, &dg] (std::unique_ptr<lib::Rules::Real> r) {
				auto *rPtr = r.release();
				auto p = findAndInsert(matchingLR, rPtr, lib::Rules::makeIsomorphismPredicate());
				if(!p.second) delete rPtr;
			};
			assert(r);
			// TODO: we should do isomorphism here instead
			lib::RC::Super mm(false, true);
			lib::RC::composeRuleRealByMatchMaker(*r, identifyR->getRule(), mm, reporter);
			delete r;
		}
	}
	return matchingLR;
}

template<typename F>
struct MR {

	MR(F f, bool &derivationFound, const lib::Rules::Real *rLower)
	: f(f), derivationFound(derivationFound), rLower(rLower) { }

	template<typename Morphism>
	bool operator()(Morphism &&m, const lib::Rules::GraphType &gUpper, const lib::Rules::GraphType &gLower) {
		std::vector<bool> notInRule(num_vertices(gLower), true);
		std::map<lib::Rules::Vertex, lib::Rules::Vertex> inverseMap;
		for(auto vUpper : asRange(vertices(gUpper))) {
			auto vLower = get(m, gUpper, gLower, vUpper);
			std::size_t id = get(boost::vertex_index_t(), gLower, vLower);
			notInRule[id] = false;
			inverseMap[vLower] = vUpper;
		}
		derivationFound = true;
		std::string strMatch = boost::lexical_cast<std::string>(matchCount);
		matchCount++;
		f(*rLower, gUpper, gLower, notInRule, inverseMap, strMatch);
		return true;
	}
public:
	std::size_t matchCount = 0;
	F f;
	bool &derivationFound;
	const lib::Rules::Real *rLower;
};

template<typename F>
void forEachMatch(const lib::DG::NonHyper &dg, lib::DG::HyperVertex v, F f) {
	const lib::DG::HyperGraphType &dgGraph = dg.getHyper().getGraph();
	assert(dgGraph[v].rules.size() == 1);
	const lib::Rules::Real &rReal = dgGraph[v].rules.front()->getAPIReference()->getRule();
	bool derivationFound = false;
	std::vector<lib::Rules::Real*> matchingLR = findCompleteRules(dg, v);
	for(const lib::Rules::Real *rLower : matchingLR) {
		MR<F> mr(f, derivationFound, rLower);
		lib::GraphMorphism::morphismSelectByLabelSettings(rReal.getDPORule(), rLower->getDPORule(),
				lib::GraphMorphism::VF2Monomorphism(), std::ref(mr), lib::Rules::MembershipPredWrapper());
		delete rLower;
	}
	if(!derivationFound) IO::log() << "WARNING: derivation " << get(boost::vertex_index_t(), dgGraph, v) << " does not exist." << std::endl;
}

} // namespace

void summary(const lib::DG::NonHyper &dg, lib::DG::HyperVertex v, const IO::Graph::Write::Options &options, const std::string &matchColour) {
	const lib::DG::HyperGraphType &dgGraph = dg.getHyper().getGraph();
	assert(dgGraph[v].rules.size() == 1);
	const lib::Rules::Real &rReal = dgGraph[v].rules.front()->getAPIReference()->getRule();

	if(!getConfig().dg.dryDerivationPrinting.get()) {
		IO::post() << "summarySectionNoEscape \"Derivation " << get(boost::vertex_index_t(), dgGraph, v) << ", \\{";
		bool first = true;
		for(auto vIn : asRange(inv_adjacent_vertices(v, dgGraph))) {
			if(!first) IO::post() << ", ";
			else first = false;
			IO::post() << "\\texttt{\\ensuremath{\\mathrm{" << dgGraph[vIn].graph->getName() << "}}}";
		}
		IO::post() << "\\} \\texttt{->} \\{";
		first = true;
		for(auto vOut : asRange(adjacent_vertices(v, dgGraph))) {
			if(!first) IO::post() << ", ";
			else first = false;
			IO::post() << "\\texttt{\\ensuremath{\\mathrm{" << dgGraph[vOut].graph->getName() << "}}}";
		}
		IO::post() << "\\}\"" << std::endl;
		{
			std::string file = getUniqueFilePrefix() + "der_constraints.tex";
			FileHandle s(file);
			s << "\\begin{verbatim}" << std::endl;
			for(const auto &c : rReal.getDPORule().leftComponentMatchConstraints)
				s << *c << '\n';
			s << "\\end{verbatim}" << std::endl;
			IO::post() << "summaryInput \"" << file << "\"" << std::endl;
		}
	}
	auto f = [&options, &matchColour](const lib::Rules::Real &rLower, const lib::Rules::GraphType &gUpper, const lib::Rules::GraphType &gLower,
			const std::vector<bool> &notInRule, const std::map<lib::Rules::Vertex, lib::Rules::Vertex> &inverseMap, const std::string & strMatch) {
		if(!getConfig().dg.dryDerivationPrinting.get()) {
			auto visibleRule = [&notInRule, &gLower](lib::Rules::Vertex v) {
				return !notInRule[get(boost::vertex_index_t(), gLower, v)];
			};
			auto visibleInstantiation = [](lib::Rules::Vertex v) {
				return true;
			};
			auto vColourRule = [&matchColour](lib::Rules::Vertex) -> std::string {
				return matchColour;
			};
			auto eColourRule = [&matchColour](lib::Rules::Edge) -> std::string {
				return matchColour;
			};
			auto vColourInstantiation = [&matchColour, &visibleRule] (lib::Rules::Vertex v) -> std::string {
				return visibleRule(v) ? matchColour : "";
			};
			auto eColourInstantiation = [&matchColour, &visibleRule, &gUpper, &gLower, &inverseMap] (lib::Rules::Edge e) -> std::string {
				unsigned int vSrcId = get(boost::vertex_index_t(), gLower, source(e, gLower));
				unsigned int vTarId = get(boost::vertex_index_t(), gLower, target(e, gLower));
				auto iterSrc = inverseMap.find(vSrcId);
				auto iterTar = inverseMap.find(vTarId);
				if(iterSrc == end(inverseMap)) return "";
				if(iterTar == end(inverseMap)) return "";
				lib::Graph::Vertex vSrc = vertex(iterSrc->second, gUpper), vTar = vertex(iterTar->second, gUpper);
				auto p = edge(vSrc, vTar, gUpper);
				return p.second ? matchColour : "";
			};
			std::string fileNoExt1 = IO::Rules::Write::pdf(rLower, options, strMatch + "_derL", strMatch + "_derK", strMatch + "_derR", visibleRule, vColourRule, eColourRule);
			std::string fileNoExt2 = IO::Rules::Write::pdf(rLower, options, strMatch + "_derG", strMatch + "_derD", strMatch + "_derH", visibleInstantiation, vColourInstantiation, eColourInstantiation);
			IO::post() << "summaryDerivation \"" << fileNoExt1 << "_" << strMatch << "\" \"" << fileNoExt2 << "_" << strMatch << "\"" << std::endl;
		}
		IO::Rules::Write::gml(rLower, false);
	};
	forEachMatch(dg, v, f);
}

} // namespace Write
} // namespace Derivation
} // namespace IO
} // namespace lib
} // namespace mod
