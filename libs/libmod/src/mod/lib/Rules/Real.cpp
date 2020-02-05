#include "Real.hpp"

#include <mod/rule/Rule.hpp>
#include <mod/lib/Chem/MoleculeUtil.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>
#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/GraphMorphism/LabelledMorphism.hpp>
#include <mod/lib/GraphMorphism/VF2Finder.hpp>
#include <mod/lib/LabelledGraph.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/Rule.hpp>
#include <mod/lib/Rules/Properties/Depiction.hpp>
#include <mod/lib/Rules/Properties/Molecule.hpp>
#include <mod/lib/Rules/Properties/Stereo.hpp>
#include <mod/lib/Rules/Properties/String.hpp>
#include <mod/lib/Rules/Properties/Term.hpp>
#include <mod/lib/Stereo/Inference.hpp>

#include <jla_boost/graph/morphism/callbacks/Limit.hpp>

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

Real::Real(LabelledRule &&rule, boost::optional<LabelType> labelType)
		: id(nextRuleNum++), name("r_{" + boost::lexical_cast<std::string>(id) + "}"), labelType(labelType),
		  dpoRule(std::move(rule)) {
	if(dpoRule.numLeftComponents == std::numeric_limits<std::size_t>::max()) dpoRule.initComponents();
	// only one of propString and propTerm should be defined
	assert(this->dpoRule.pString || this->dpoRule.pTerm);
	assert(!this->dpoRule.pString || !this->dpoRule.pTerm);
	if(!sanityChecks(getGraph(), getStringState(), IO::log())) {
		IO::log() << "Rule::sanityCheck\tfailed in rule '" << getName() << "'" << std::endl;
		MOD_ABORT;
	}
}

Real::~Real() = default;

std::size_t Real::getId() const {
	return id;
}

std::shared_ptr<rule::Rule> Real::getAPIReference() const {
	if(apiReference.use_count() > 0) return std::shared_ptr<rule::Rule>(apiReference);
	else std::abort();
}

void Real::setAPIReference(std::shared_ptr<rule::Rule> r) {
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

boost::optional<LabelType> Real::getLabelType() const {
	return labelType;
}

const LabelledRule &Real::getDPORule() const {
	return dpoRule;
}

const lib::Rules::GraphType &Real::getGraph() const {
	return get_graph(dpoRule);
}

DepictionDataCore &Real::getDepictionData() {
	if(!depictionData) depictionData.reset(new DepictionDataCore(getDPORule()));
	return *depictionData;
}

const DepictionDataCore &Real::getDepictionData() const {
	if(!depictionData) depictionData.reset(new DepictionDataCore(getDPORule()));
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

const PropStringCore &Real::getStringState() const {
	assert(dpoRule.pString || dpoRule.pTerm);
	if(!dpoRule.pString) {
		dpoRule.pString.reset(new PropStringCore(get_graph(dpoRule),
		                                         dpoRule.leftMatchConstraints, dpoRule.rightMatchConstraints,
		                                         getTermState(), lib::Term::getStrings()));
	}
	return *dpoRule.pString;
}

const PropTermCore &Real::getTermState() const {
	assert(dpoRule.pString || dpoRule.pTerm);
	if(!dpoRule.pTerm) {
		dpoRule.pTerm.reset(new PropTermCore(get_graph(dpoRule),
		                                     dpoRule.leftMatchConstraints, dpoRule.rightMatchConstraints,
		                                     getStringState(), lib::Term::getStrings()));
	}
	return *dpoRule.pTerm;
}

const PropMoleculeCore &Real::getMoleculeState() const {
	return get_molecule(getDPORule());
}

namespace {

template<typename Finder>
std::size_t morphism(const Real &gDomain,
                     const Real &gCodomain,
                     std::size_t maxNumMatches,
                     LabelSettings labelSettings,
                     Finder finder) {
	auto mr = jla_boost::GraphMorphism::makeLimit(maxNumMatches);
	lib::GraphMorphism::morphismSelectByLabelSettings(gDomain.getDPORule(), gCodomain.getDPORule(), labelSettings,
	                                                  finder, std::ref(mr), MembershipPredWrapper());
	return mr.getNumHits();
}

} // namespace

std::size_t Real::isomorphism(const Real &rDom,
                              const Real &rCodom,
                              std::size_t maxNumMatches,
                              LabelSettings labelSettings) {
	return morphism(rDom, rCodom, maxNumMatches, labelSettings, lib::GraphMorphism::VF2Isomorphism());
}

std::size_t Real::monomorphism(const Real &rDom,
                               const Real &rCodom,
                               std::size_t maxNumMatches,
                               LabelSettings labelSettings) {
	return morphism(rDom, rCodom, maxNumMatches, labelSettings, lib::GraphMorphism::VF2Monomorphism());
}

bool Real::isomorphicLeftRight(const Real &rDom, const Real &rCodom, LabelSettings labelSettings) {
	auto mrLeft = jla_boost::GraphMorphism::makeLimit(1);
	lib::GraphMorphism::morphismSelectByLabelSettings(
			get_labelled_left(rDom.getDPORule()),
			get_labelled_left(rCodom.getDPORule()),
			labelSettings,
			lib::GraphMorphism::VF2Isomorphism(), std::ref(mrLeft));
	if(mrLeft.getNumHits() == 0) return false;
	auto mrRight = jla_boost::GraphMorphism::makeLimit(1);
	lib::GraphMorphism::morphismSelectByLabelSettings(
			get_labelled_right(rDom.getDPORule()),
			get_labelled_right(rCodom.getDPORule()),
			labelSettings,
			lib::GraphMorphism::VF2Isomorphism(), std::ref(mrRight));
	return mrRight.getNumHits() == 1;
}

} // namespace Rules
} // namespace lib
} // namespace mod
