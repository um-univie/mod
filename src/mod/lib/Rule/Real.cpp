#include "Real.h"

#include <mod/Rule.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/GraphMorphism/LabelledMorphism.h>
#include <mod/lib/LabelledGraph.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/Rule.h>
#include <mod/lib/Rule/Properties/Depiction.h>
#include <mod/lib/Rule/Properties/String.h>
#include <mod/lib/Rule/Properties/Molecule.h>

#include <jla_boost/graph/morphism/VF2Finder.hpp>
#include <jla_boost/Memory.hpp>

#include <boost/graph/connected_components.hpp>

namespace mod {
namespace lib {
namespace Rule {
BOOST_CONCEPT_ASSERT((LabelledGraphConcept<LabelledRule>));
BOOST_CONCEPT_ASSERT((LabelledGraphConcept<LabelledRule::LabelledLeftType>));
BOOST_CONCEPT_ASSERT((LabelledGraphConcept<LabelledRule::LabelledRightType>));

//------------------------------------------------------------------------------
// Component storage
//------------------------------------------------------------------------------

struct Real::ComponentStorage {

	ComponentStorage(const Real &r)
	: left(get_labelled_left(r.getDPORule())), right(get_labelled_right(r.getDPORule())) {
		for(std::size_t i = 0; i < get_num_connected_components(left); ++i)
			leftComponents.push_back(get_component_graph(i, left));
		for(std::size_t i = 0; i < get_num_connected_components(right); ++i)
			rightComponents.push_back(get_component_graph(i, right));
	}
public:
	LabelledRule::LabelledLeftType left;
	LabelledRule::LabelledRightType right;
	std::vector<ComponentGraph> leftComponents, rightComponents;
};

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
			s << "Rule::sanityCheck\tcan't handle parallel edges in lib::Rule::GraphType" << std::endl;
			printEdge(e, core, labelState, s);
			return false;
		}
	}
	return true;
}

Real::Real(LabelledRule &&rule) : dpoRule(std::move(rule)),
left(getGraph(), Membership::Left),
context(getGraph(), Membership::Context),
right(getGraph(), Membership::Right) {
	dpoRule.leftComponents.resize(num_vertices(get_graph(dpoRule)), -1);
	dpoRule.rightComponents.resize(num_vertices(get_graph(dpoRule)), -1);
	dpoRule.numLeftComponents = boost::connected_components(get_graph(get_labelled_left(dpoRule)), dpoRule.leftComponents.data());
	dpoRule.numRightComponents = boost::connected_components(get_graph(get_labelled_right(dpoRule)), dpoRule.rightComponents.data());
	assert(this->dpoRule.pString);
	if(!sanityChecks(getGraph(), getStringState(), IO::log())) {
		IO::log() << "Rule::sanityCheck\tfailed in rule '" << getName() << "'" << std::endl;
		MOD_ABORT;
	}
	getStringState().verify(&get_graph(this->dpoRule));
}

LabelledRule Real::clone() const {
	const auto &g = getGraph();
	const auto &pString = getStringState();
	LabelledRule dpoCopy;
	dpoCopy.pString = make_unique<lib::Rule::PropStringCore>(get_graph(dpoCopy));
	auto &gCopy = get_graph(dpoCopy);
	auto &pStringCopy = *dpoCopy.pString;
	for(Vertex v : asRange(vertices(g))) {
		Vertex vCopy = add_vertex(gCopy);
		gCopy[vCopy].membership = g[v].membership;
		switch(g[v].membership) {
		case Membership::Left:
			pStringCopy.add(vCopy, pString.getLeft()[v], "");
			break;
		case Membership::Right:
			pStringCopy.add(vCopy, "", pString.getRight()[v]);
			break;
		case Membership::Context:
			pStringCopy.add(vCopy, pString.getRight()[v], pString.getLeft()[v]);
			break;
		}
	}
	for(Edge e : asRange(edges(g))) {
		Edge eCopy = add_edge(source(e, g), target(e, g), gCopy).first;
		gCopy[eCopy].membership = g[e].membership;
		switch(g[e].membership) {
		case Membership::Left:
			pStringCopy.add(eCopy, pString.getLeft()[e], "");
			break;
		case Membership::Right:
			pStringCopy.add(eCopy, "", pString.getRight()[e]);
			break;
		case Membership::Context:
			pStringCopy.add(eCopy, pString.getLeft()[e], pString.getRight()[e]);
			break;
		}
	}
	return dpoCopy;
}

Real::~Real() { }

bool Real::isReal() const {
	return true;
}

const LabelledRule &Real::getDPORule() const {
	return dpoRule;
}

const lib::Rule::GraphType &Real::getGraph() const {
	return get_graph(dpoRule);
}

const DPOProjection &Real::getLeft() const {
	return left;
}

const DPOProjection &Real::getContext() const {
	return context;
}

const DPOProjection &Real::getRight() const {
	return right;
}

RuleSideComponentRange Real::getLeftComponents() const {
	return getComponentStore().leftComponents;
}

RuleSideComponentRange Real::getRightComponents() const {
	return getComponentStore().rightComponents;
}

const std::vector<std::unique_ptr<LabelledRule::MatchConstraint> > &Real::getLeftConstraints() const {
	return getDPORule().leftComponentMatchConstraints;
}

const std::vector<std::size_t> &Real::getCompMapLeft() const {
	return dpoRule.leftComponents;
}

const std::vector<std::size_t> &Real::getCompMapRight() const {
	return dpoRule.rightComponents;
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

std::size_t Real::getNumLeftComponents() const {
	return dpoRule.numLeftComponents;
}

std::size_t Real::getNumRightComponents() const {
	return dpoRule.numRightComponents;
}

Real::ComponentStorage &Real::getComponentStore() {
	if(!componentStore) componentStore.reset(new ComponentStorage(*this));
	return *componentStore;
}

const Real::ComponentStorage &Real::getComponentStore() const {
	if(!componentStore) componentStore.reset(new ComponentStorage(*this));
	return *componentStore;
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
	return morphism(rDom, rCodom, maxNumMatches, jla_boost::GraphMorphism::VF2Isomorphism());
}

std::size_t Real::monomorphism(const Real &rDom, const Real &rCodom, std::size_t maxNumMatches) {
	return morphism(rDom, rCodom, maxNumMatches, jla_boost::GraphMorphism::VF2Monomorphism());
}

std::shared_ptr<mod::Rule> Real::createSide(const lib::Graph::GraphType &g, const lib::Graph::PropStringType &pStringGraph, Membership membership, const std::string &name) {
	assert(membership == Membership::Left || membership == Membership::Context || membership == Membership::Right);
	lib::Rule::LabelledRule rule;
	auto &core = get_graph(rule);
	rule.pString = make_unique<PropStringCore>(core);
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
	auto ret = mod::Rule::makeRule(make_unique<Real>(std::move(rule)));
	ret->setName(completeName);
	return ret;
}

void Real::invert(GraphType &g, PropStringCore &pString) {
	for(Vertex v : asRange(vertices(g))) {
		auto membership = g[v].membership;
		switch(membership) {
		case Membership::Left:
		{
			auto label = pString.getLeft()[v];
			g[v].membership = Membership::Right;
			pString.setRight(v, label);
		}
			break;
		case Membership::Right:
		{
			auto label = pString.getRight()[v];
			g[v].membership = Membership::Left;
			pString.setLeft(v, label);
		}
			break;
		case Membership::Context:
		{
			auto left = pString.getLeft()[v];
			auto right = pString.getRight()[v];
			pString.setLeft(v, right);
			pString.setRight(v, left);
		}
			break;
		}
	}

	for(Edge e : asRange(edges(g))) {
		auto membership = g[e].membership;
		switch(membership) {
		case Membership::Left:
		{
			auto label = pString.getLeft()[e];
			g[e].membership = Membership::Right;
			pString.setRight(e, label);
		}
			break;
		case Membership::Right:
		{
			auto label = pString.getRight()[e];
			g[e].membership = Membership::Left;
			pString.setLeft(e, label);
		}
			break;
		case Membership::Context:
		{
			auto left = pString.getLeft()[e];
			auto right = pString.getRight()[e];
			pString.setLeft(e, right);
			pString.setRight(e, left);
		}
			break;
		}
	}
}

} // namespace Rule
} // namespace lib
} // namespace mod
