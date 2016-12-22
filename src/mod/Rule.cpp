#include "Rule.h"

#include <mod/GraphPrinter.h>
#include <mod/Misc.h>
#include <mod/RuleGraphInterface.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/Rule.h>
#include <mod/lib/Rules/Properties/Depiction.h>
#include <mod/lib/Rules/Real.h>

#include <sstream>

namespace mod {

struct Rule::Pimpl {

	Pimpl(std::unique_ptr<lib::Rules::Real> r) : r(std::move(r)) {
		assert(this->r);
	}
public:
	const std::unique_ptr<lib::Rules::Real> r;
};

Rule::Rule(std::unique_ptr<lib::Rules::Real> r) : p(new Pimpl(std::move(r))) { }

Rule::~Rule() { }

unsigned int Rule::getId() const {
	return getRule().getId();
}

std::ostream &operator<<(std::ostream &s, const Rule &r) {
	return s << "'" << r.getName() << "'";
}

const lib::Rules::Real &Rule::getRule() const {
	return *p->r.get();
}

//------------------------------------------------------------------------------

std::size_t Rule::numVertices() const {
	return num_vertices(p->r->getGraph());
}

Rule::VertexRange Rule::vertices() const {
	return VertexRange(p->r->getAPIReference());
}

std::size_t Rule::numEdges() const {
	return num_edges(p->r->getGraph());
}

Rule::EdgeRange Rule::edges() const {
	return EdgeRange(p->r->getAPIReference());
}

Rule::LeftGraph Rule::getLeft() const {
	return LeftGraph(p->r->getAPIReference());
}

Rule::ContextGraph Rule::getContext() const {
	return ContextGraph(p->r->getAPIReference());
}

Rule::RightGraph Rule::getRight() const {
	return RightGraph(p->r->getAPIReference());
}

//------------------------------------------------------------------------------

std::shared_ptr<Rule> Rule::makeInverse() const {
	lib::Rules::LabelledRule dpoRule(getRule().getDPORule(), true);
	if(getConfig().rule.ignoreConstraintsDuringInversion.get()) {
		if(dpoRule.leftComponentMatchConstraints.size() > 0
				|| dpoRule.rightComponentMatchConstraints.size() > 0) {
			lib::IO::log() << "WARNING: inversion of rule strips constraints.\n";
		}
	} else {
		if(dpoRule.leftComponentMatchConstraints.size() > 0) {
			throw LogicError("Can not invert rule with left-side component constraints.");
		}
		if(dpoRule.rightComponentMatchConstraints.size() > 0) {
			throw LogicError("Can not invert rule with right-side component constraints.");
		}
	}
	dpoRule.invert();
	bool ignore = getConfig().rule.ignoreConstraintsDuringInversion.get();
	if(ignore) dpoRule.rightComponentMatchConstraints.clear();
	auto rInner = std::make_unique<lib::Rules::Real>(std::move(dpoRule));
	return makeRule(std::move(rInner));
}

std::pair<std::string, std::string> Rule::print() const {
	GraphPrinter first;
	GraphPrinter second;
	second.setReactionDefault();
	return print(first, second);
}

std::pair<std::string, std::string> Rule::print(const GraphPrinter &first, const GraphPrinter &second) const {
	return lib::IO::Rules::Write::summary(getRule(), first.getOptions(), second.getOptions());
}

std::string Rule::getGMLString(bool withCoords) const {
	if(withCoords && !getRule().getDepictionData().getHasCoordinates())
		throw LogicError("Coordinates are not available for this rule (" + getName() + ").");
	std::stringstream ss;
	lib::IO::Rules::Write::gml(getRule(), withCoords, ss);
	return ss.str();
}

std::string Rule::printGML(bool withCoords) const {
	if(withCoords && !getRule().getDepictionData().getHasCoordinates())
		throw LogicError("Coordinates are not available for this rule (" + getName() + ").");
	return lib::IO::Rules::Write::gml(*p->r, withCoords);
}

const std::string &Rule::getName() const {
	return getRule().getName();
}

void Rule::setName(std::string name) {
	p->r->setName(name);
}

std::size_t Rule::getNumLeftComponents() const {
	return getRule().getDPORule().numLeftComponents;
}

std::size_t Rule::getNumRightComponents() const {
	return getRule().getDPORule().numRightComponents;
}

std::size_t Rule::isomorphism(std::shared_ptr<Rule> r, std::size_t maxNumMatches) const {
	return lib::Rules::Real::isomorphism(this->getRule(), r->getRule(), maxNumMatches);
}

std::size_t Rule::monomorphism(std::shared_ptr<Rule> r, std::size_t maxNumMatches) const {
	return lib::Rules::Real::monomorphism(this->getRule(), r->getRule(), maxNumMatches);
}

//------------------------------------------------------------------------------
// Static
//------------------------------------------------------------------------------

namespace {

std::shared_ptr<Rule> handleLoadedRule(lib::IO::Rules::Read::Data &&data, bool invert, const std::string &dataSource, std::ostringstream &err) {
	if(!data.rule) {
		err << "\nCould not load rule from " << dataSource << "." << std::endl;
		throw InputError(err.str());
	}
	assert(data.rule->pString);
	if(invert) {
		if(!data.rule->leftComponentMatchConstraints.empty()) {
			if(getConfig().rule.ignoreConstraintsDuringInversion.get()) lib::IO::log() << "WARNING: ";
			bool ignore = getConfig().rule.ignoreConstraintsDuringInversion.get();
			std::ostream &stream = ignore ? lib::IO::log() : err;
			stream << "The rule '";
			if(data.name) stream << data.name.get();
			else stream << "anon";
			stream << "' from " << dataSource << " has matching constraints ";
			if(!ignore) {
				stream << "and can not be reversed. Use " << getConfig().rule.ignoreConstraintsDuringInversion.getName() << " == true to strip constraints." << std::endl;
				throw InputError(err.str());
			} else {
				stream << "and these will be stripped from the reversed rule." << std::endl;
				data.rule->leftComponentMatchConstraints.clear();
			}
		}
		data.rule->invert();
		if(data.name) data.name.get() += ", inverse";
	}
	auto libRes = std::make_unique<lib::Rules::Real>(std::move(*data.rule));
	if(data.name) libRes->setName(std::move(data.name.get()));
	return Rule::makeRule(std::move(libRes));
}

} // namespace 

std::shared_ptr<Rule> Rule::ruleGMLString(const std::string &data, bool invert) {
	std::istringstream ss(data);
	std::ostringstream err;
	return handleLoadedRule(lib::IO::Rules::Read::gml(ss, err), invert, "<inline GML string>", err);
}

std::shared_ptr<Rule> Rule::ruleGML(const std::string &file, bool invert) {
	std::string fullFilename = prefixFilename(file);
	std::ifstream ifs(fullFilename);
	if(!ifs) throw InputError("Could not open rule GML file '" + file + "' ('" + fullFilename + "').\n");
	std::ostringstream err;
	return handleLoadedRule(lib::IO::Rules::Read::gml(ifs, err), invert, "file '" + fullFilename + "'", err);
}

std::shared_ptr<Rule> Rule::makeRule(std::unique_ptr<lib::Rules::Real> r) {
	std::shared_ptr<Rule> rWrapped(new Rule(std::move(r)));
	rWrapped->p->r->setAPIReference(rWrapped);
	return rWrapped;
}

} // namespace mod