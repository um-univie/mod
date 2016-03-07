#include "Rule.h"

#include <mod/Graph.h>
#include <mod/Misc.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/Rule.h>
#include <mod/lib/Rule/Properties/Depiction.h>
#include <mod/lib/Rule/Real.h>
#include <mod/lib/Rule/Shallow.h>

#include <jla_boost/Memory.hpp>

#include <sstream>

namespace mod {

struct Rule::Pimpl {
	Pimpl(std::unique_ptr<lib::Rule::Real> r);
	Pimpl(std::unique_ptr<lib::Rule::Shallow> r);
	lib::Rule::Base &getBase();
public:
	const std::unique_ptr<lib::Rule::Real> real;
	const std::unique_ptr<lib::Rule::Shallow> shallow;
};

Rule::Rule(std::unique_ptr<lib::Rule::Real> r) : p(new Pimpl(std::move(r))) { }

Rule::Rule(std::unique_ptr<lib::Rule::Shallow> r) : p(new Pimpl(std::move(r))) { }

Rule::~Rule() { }

unsigned int Rule::getId() const {
	return p->getBase().getId();
}

std::ostream &operator<<(std::ostream &s, const Rule &r) {
	return s << "'" << r.p->getBase().getName() << "'";
}

const lib::Rule::Base &Rule::getBase() const {
	return p->getBase();
}

const lib::Rule::Real *Rule::getReal() const {
	return p->real.get();
}

const lib::Rule::Shallow *Rule::getShallow() const {
	return p->shallow.get();
}

std::pair<std::string, std::string> Rule::print() const {
	GraphPrinter first;
	GraphPrinter second;
	second.setReactionDefault();
	return print(first, second);
}

std::pair<std::string, std::string> Rule::print(const GraphPrinter &first, const GraphPrinter &second) const {
	if(p->real) {
		assert(!p->shallow);
		return lib::IO::Rule::Write::summary(*p->real, first.getOptions(), second.getOptions());
	} else {
		assert(p->shallow);
		lib::IO::log() << "Notice: the rule '" << p->shallow->getName() << "' is shallow and is not printed." << std::endl;
		return std::make_pair("", "");
	}
}

std::string Rule::getGMLString(bool withCoords) const {
	if(p->real) {
		if(withCoords && !p->real->getDepictionData().getHasCoordinates())
			throw LogicError("Coordinates are not available for this rule (" + getName() + ").");
		assert(!p->shallow);
		std::stringstream ss;
		lib::IO::Rule::Write::gml(*p->real, withCoords, ss);
		return ss.str();
	} else {
		assert(p->shallow);
		lib::IO::log() << "Notice: the rule '" << p->shallow->getName() << "' is shallow and is not printed as GML." << std::endl;
		return "NoGMLStringAvailable";
	}
}

std::string Rule::printGML(bool withCoords) const {
	if(p->real) {
		if(withCoords && !p->real->getDepictionData().getHasCoordinates())
			throw LogicError("Coordinates are not available for this rule (" + getName() + ").");
		assert(!p->shallow);
		return lib::IO::Rule::Write::gml(*p->real, withCoords);
	} else {
		assert(p->shallow);
		lib::IO::log() << "Notice: the rule '" << p->shallow->getName() << "' is shallow and is not printed as GML." << std::endl;
		return "NoGMLFilePrinted";
	}
}

const std::string &Rule::getName() const {
	return p->getBase().getName();
}

void Rule::setName(std::string name) {
	p->getBase().setName(name);
}

std::size_t Rule::getNumLeftComponents() const {
	if(!getReal()) throw LogicError("The the rule is fake.");
	return getReal()->getNumLeftComponents();
}

std::size_t Rule::getNumRightComponents() const {
	if(!getReal()) throw LogicError("The the rule is fake.");
	return getReal()->getNumRightComponents();
}

std::size_t Rule::isomorphism(std::shared_ptr<Rule> r, std::size_t maxNumMatches) const {
	if(!this->getReal()) throw LogicError("This rule may not be fake.");
	if(!r->getReal()) throw LogicError("The other rule may not be fake.");
	return lib::Rule::Real::isomorphism(*this->getReal(), *r->getReal(), maxNumMatches);
}

std::size_t Rule::monomorphism(std::shared_ptr<Rule> r, std::size_t maxNumMatches) const {
	if(!this->getReal()) throw LogicError("This rule may not be fake.");
	if(!r->getReal()) throw LogicError("The other rule may not be fake.");
	return lib::Rule::Real::monomorphism(*this->getReal(), *r->getReal(), maxNumMatches);
}

//------------------------------------------------------------------------------
// Static
//------------------------------------------------------------------------------

namespace {

std::shared_ptr<Rule> handleLoadedRule(lib::IO::Rule::Read::Data &&data, bool invert, const std::string &dataSource, std::ostringstream &err) {
	if(!data.rule) {
		err << "Could not load rule from " << dataSource << "." << std::endl;
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
		lib::Rule::Real::invert(get_graph(*data.rule), *data.rule->pString);
		if(data.name) data.name.get() += ", inverse";
	}
	auto libRes = make_unique<lib::Rule::Real>(std::move(*data.rule));
	if(data.name) libRes->setName(std::move(data.name.get()));
	return Rule::makeRule(std::move(libRes));
}

} // namespace 

std::shared_ptr<Rule> Rule::ruleGMLString(const std::string &data, bool invert) {
	std::istringstream ss(data);
	std::ostringstream err;
	return handleLoadedRule(lib::IO::Rule::Read::gml(ss, err), invert, "<inline GML string>", err);
}

std::shared_ptr<Rule> Rule::ruleGML(const std::string &file, bool invert) {
	std::string fullFilename = prefixFilename(file);
	std::ifstream ifs(fullFilename);
	if(!ifs) throw InputError("Could not open rule GML file '" + file + "' ('" + fullFilename + "').\n");
	std::ostringstream err;
	return handleLoadedRule(lib::IO::Rule::Read::gml(ifs, err), invert, "file '" + fullFilename + "'", err);
}

std::shared_ptr<Rule> Rule::makeRule(std::unique_ptr<lib::Rule::Real> r) {
	std::shared_ptr<Rule> rWrapped(new Rule(std::move(r)));
	rWrapped->p->getBase().setAPIReference(rWrapped);
	return rWrapped;
}

std::shared_ptr<Rule> Rule::makeRule(std::unique_ptr<lib::Rule::Shallow> r) {
	std::shared_ptr<Rule> rWrapped(new Rule(std::move(r)));
	rWrapped->p->getBase().setAPIReference(rWrapped);
	return rWrapped;
}

//------------------------------------------------------------------------------
// Pimpl impl
//------------------------------------------------------------------------------

Rule::Pimpl::Pimpl(std::unique_ptr<lib::Rule::Real> r) : real(std::move(r)) {
	assert(this->real);
}

Rule::Pimpl::Pimpl(std::unique_ptr<lib::Rule::Shallow> r) : shallow(std::move(r)) {
	assert(this->shallow);
}

lib::Rule::Base &Rule::Pimpl::getBase() {
	if(real) {
		assert(!shallow);
		return *real;
	} else {
		assert(shallow);
		return *shallow;
	}
}


} // namespace mod