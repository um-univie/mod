#include "Derivation.h"

#include <mod/Graph.h>
#include <mod/Rule.h>

#include <ostream>

namespace mod {

std::ostream &operator<<(std::ostream &s, const Derivation &d) {
	s << "{";
	for(auto g : d.left) s << " '" << g->getName() << "'";
	s << " }";
	if(d.rule) s << ", '" << d.rule->getName() << "'";
	s << ", {";
	for(auto g : d.right) s << " '" << g->getName() << "'";
	s << " }";
	return s;
}

} // namespace mod