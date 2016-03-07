#include "Base.h"

#include <mod/Error.h>
#include <mod/Rule.h>

#include <boost/lexical_cast.hpp>

#include <cassert>
#include <ostream>

namespace mod {
namespace lib {
namespace Rule {

namespace {
std::size_t nextRuleNum = 0;
} // namespace 

Base::Base() : id(nextRuleNum++), name("r_{" + boost::lexical_cast<std::string>(id) + "}") { }

Base::~Base() { }

unsigned int Base::getId() const {
	return id;
}

std::shared_ptr<mod::Rule> Base::getAPIReference() const {
	if(apiReference.use_count() > 0) return std::shared_ptr<mod::Rule > (apiReference);
	else {
		MOD_ABORT;
	}
}

void Base::setAPIReference(std::shared_ptr<mod::Rule> r) {
	assert(apiReference.use_count() == 0);
	apiReference = r;
#ifndef NDEBUG
	assert(&r->getBase() == this);
#endif
}

const std::string &Base::getName() const {
	return name;
}

void Base::setName(std::string name) {
	this->name = name;
}

} // namespace Rule
} // namespace lib
} // namespace mod