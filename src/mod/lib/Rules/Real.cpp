#include "Real.h"

#include <mod/Rule.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/GraphMorphism/LabelledMorphism.h>
#include <mod/lib/GraphMorphism/VF2Finder.hpp>
#include <mod/lib/LabelledGraph.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/Rule.h>
#include <mod/lib/Rules/Properties/Depiction.h>
#include <mod/lib/Rules/Properties/String.h>
#include <mod/lib/Rules/Properties/Molecule.h>

#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace Rules {
BOOST_CONCEPT_ASSERT((LabelledGraphConcept<LabelledRule>));
BOOST_CONCEPT_ASSERT((LabelledGraphConcept<LabelledRule::LabelledLeftType>));
BOOST_CONCEPT_ASSERT((LabelledGraphConcept<LabelledRule::LabelledRightType>));

//------------------------------------------------------------------------------
// Main class
//------------------------------------------------------------------------------

void printEdge(Edge e, const GraphType &core, const PropStringCore &labelState, std::ostream &s) {
	Vertex vSrc = source(e, core), vTar = target(e, core);
	s << "\t";
	labelState.print(s, vSrc);
	s << " -- ";
	labelState.print(s, e);
	s << " -- ";
	labelState.print(s, vTar);
}

bool Real::sanityChecks(const GraphType &core, const PropStringCore &labelState, std::ostream &s) {
	// hmm, boost::edge_range is not supported for vecS, right? so we count in a rather stupid way

	for(Edge e : asRange(edges(core))) {
		Vertex vSrc = source(e, core), vTar = target(e, core);
		auto eContext = core[e].membership,
				srcContext = core[vSrc].membership,
				tarContext = core[vTar].membership;
		// check danglingness
		if(eContext != srcContext && srcContext != Membership::Context) {
			s << "Rule::sanityCheck\tdangling edge at source: " << std::endl;
			printEdge(e, core, labelState, s);
			return false;
		}
		if(eContext != tarContext && tarContext != Membership::Context) {
			s << "Rule::sanityCheck\tdangling edge at target: " << std::endl;
			printEdge(e, core, labelState, s);
			return false;
		}
		// check parallelness
		unsigned int count = 0;
		for(Edge eOut : asRange(out_edges(vSrc, core))) if(target(eOut, core) == vTar) count++;
		if(count > 1) {
			s << "Rule::sanityCheck\tcan't handle parallel edges in lib::Rules::GraphType" << std::endl;
			printEdge(e, core, labelState, s);
			return false;
		}
	}
	return true;
}

namespace {
std::size_t nextRuleNum = 0;
} // namespace 

Real::Real(LabelledRule &&rule) : id(nextRuleNum++), name("r_{" + boost::lexical_cast<std::string>(id) + "}"),
dpoRule(std::move(rule)) {
	if(dpoRule.numLeftComponents == std::numeric_limits<std::size_t>::max()) dpoRule.initComponents();
	assert(this->dpoRule.pString);
	if(!sanityChecks(getGraph(), getStringState(), IO::log())) {
		IO::log() << "Rule::sanityCheck\tfailed in rule '" << getName() << "'" << std::endl;
		MOD_ABORT;
	}
	getStringState().verify(&get_graph(this->dpoRule));
}

Real::~Real() { }

std::size_t Real::getId() const {
	return id;
}

std::shared_ptr<mod::Rule> Real::getAPIReference() const {
	if(apiReference.use_count() > 0) return std::shared_ptr<mod::Rule > (apiReference);
	else {
		MOD_ABORT;
	}
}

void Real::setAPIReference(std::shared_ptr<mod::Rule> r) {
	assert(apiReference.use_count() == 0);
	apiReference = r;
#ifndef NDEBUG
	assert(&r->getRule() == this);
#endif
}

const std::string &Real::getName() const {
	return name;
}

void Real::setName(std::string name) {
	this->name = name;
}

const LabelledRule &Real::getDPORule() const {
	return dpoRule;
}

const lib::Rules::GraphType &Real::getGraph() const {
	return get_graph(dpoRule);
}

const PropStringCore &Real::getStringState() const {
	assert(dpoRule.pString);
	return *dpoRule.pString;
}

const PropMoleculeCore &Real::getMoleculeState() const {
	if(!moleculeState) {
		moleculeState.reset(new PropMoleculeCore(getGraph(), getStringState()));
	}
	return *moleculeState;
}

DepictionDataCore &Real::getDepictionData() {
	if(!depictionData) depictionData.reset(new DepictionDataCore(getGraph(), getStringState(), getMoleculeState()));
	return *depictionData;
}

const DepictionDataCore &Real::getDepictionData() const {
	if(!depictionData) depictionData.reset(new DepictionDataCore(getGraph(), getStringState(), getMoleculeState()));
	return *depictionData;
}

bool Real::isChemical() const {
	// TODO: actually implement it
	return true;
}

bool Real::isOnlySide(Membership membership) const {
	const GraphType &core = getGraph();
	for(Vertex v : asRange(vertices(core)))
		if(core[v].membership != membership) return false;
	for(Edge e : asRange(edges(core)))
		if(core[e].membership != membership) return false;
	return true;
}

bool Real::isOnlyRightSide() const {
	return isOnlySide(Membership::Right);
}

namespace {

template<typename Finder>
std::size_t morphism(const Real &gDomain, const Real &gCodomain, std::size_t maxNumMatches, Finder finder) {
	auto mr = jla_boost::GraphMorphism::makeLimit(maxNumMatches);
	lib::GraphMorphism::morphismSelectByLabelSettings(gDomain.getDPORule(), gCodomain.getDPORule(), finder, std::ref(mr), MembershipPredWrapper());
	return mr.getNumHits();
}

} // namespace

std::size_t Real::isomorphism(const Real &rDom, const Real &rCodom, std::size_t maxNumMatches) {
	return morphism(rDom, rCodom, maxNumMatches, lib::GraphMorphism::VF2Isomorphism());
}

std::size_t Real::monomorphism(const Real &rDom, const Real &rCodom, std::size_t maxNumMatches) {
	return morphism(rDom, rCodom, maxNumMatches, lib::GraphMorphism::VF2Monomorphism());
}

std::shared_ptr<mod::Rule> Real::createSide(const lib::Graph::GraphType &g, const lib::Graph::PropString &pStringGraph, Membership membership, const std::string &name) {
	assert(membership == Membership::Left || membership == Membership::Context || membership == Membership::Right);
	lib::Rules::LabelledRule rule;
	auto &core = get_graph(rule);
	rule.pString = std::make_unique<PropStringCore>(core);
	auto &pString = *rule.pString;
	std::map<Vertex, lib::Graph::Vertex> gToR;

	for(lib::Graph::Vertex v : asRange(vertices(g))) {
		Vertex vCore = add_vertex(core);
		gToR[v] = vCore;
		core[vCore].membership = membership;
		const auto &label = pStringGraph[v];
		switch(membership) {
		case Membership::Left:
			pString.add(vCore, label, "");
			break;
		case Membership::Right:
			pString.add(vCore, "", label);
			break;
		case Membership::Context:
			pString.add(vCore, label, label);
			break;
		}
	}

	for(lib::Graph::Edge e : asRange(edges(g))) {
		Edge eCore = add_edge(gToR[source(e, g)], gToR[target(e, g)], core).first;
		core[eCore].membership = membership;
		const auto &label = pStringGraph[e];
		switch(membership) {
		case Membership::Left:
			pString.add(eCore, label, "");
			break;
		case Membership::Right:
			pString.add(eCore, "", label);
			break;
		case Membership::Context:
			pString.add(eCore, label, label);
			break;
		}
	}
	rule.initComponents();
	std::string completeName;
	switch(membership) {
	case Membership::Left:
		completeName += "unbind";
		break;
	case Membership::Context:
		completeName += "id";
		break;
	case Membership::Right:
		completeName += "bind";
		break;
	default:
		MOD_ABORT;
		break;
	}
	completeName += "<";
	completeName += name;
	completeName += ">";
	auto ret = mod::Rule::makeRule(std::make_unique<Real>(std::move(rule)));
	ret->setName(completeName);
	return ret;
}

} // namespace Rules
} // namespace lib
} // namespace mod
