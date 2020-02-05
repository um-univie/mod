#include "Derivation.hpp"

#include <mod/graph/Graph.hpp>
#include <mod/rule/Rule.hpp>

#include <ostream>

namespace mod {

Derivation::~Derivation() = default;

std::ostream &operator<<(std::ostream &s, const Derivation &d) {
	s << "{";
	for(auto g : d.left) s << " '" << g->getName() << "'";
	s << " }";
	if(d.r) s << ", '" << d.r->getName() << "'";
	s << ", {";
	for(auto g : d.right) s << " '" << g->getName() << "'";
	s << " }";
	return s;
}


Derivation::operator Derivations() const {
	if(r) return {left, {r}, {right}};
	else return {left, {}, right};
}

Derivations::~Derivations() = default;

std::ostream &operator<<(std::ostream &s, const Derivations &d) {
	s << "{";
	for(auto g : d.left) s << " '" << g->getName() << "'";
	s << " } <";
	for(auto r : d.rules) s << " '" << r->getName() << "'";
	s << " > {";
	for(auto g : d.right) s << " '" << g->getName() << "'";
	s << " }";
	return s;
}

} // namespace mod