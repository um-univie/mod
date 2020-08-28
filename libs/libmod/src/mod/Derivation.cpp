#include "Derivation.hpp"

#include <mod/graph/Graph.hpp>
#include <mod/rule/Rule.hpp>

#include <ostream>

namespace mod {

Derivation::~Derivation() = default;

std::ostream &operator<<(std::ostream &s, const Derivation &d) {
	s << "{";
	for(const auto &g : d.left) {
		if(g) s << " '" << g->getName() << "'";
		else s << " null";
	}
	s << " }";
	if(d.r) s << ", '" << d.r->getName() << "'";
	s << ", {";
	for(const auto &g : d.right) {
		if(g) s << " '" << g->getName() << "'";
		else s << " null";
	}
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
	for(const auto &g : d.left) {
		if(g) s << " '" << g->getName() << "'";
		else s << " null";
	}
	s << " } <";
	for(const auto &r : d.rules) {
		if(r) s << " '" << r->getName() << "'";
		else s << " null";
	}
	s << " > {";
	for(const auto &g : d.right) {
		if(g) s << " '" << g->getName() << "'";
		else s << " null";
	}
	s << " }";
	return s;
}

} // namespace mod