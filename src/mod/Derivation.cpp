#include "Derivation.h"

#include <mod/graph/Graph.h>
#include <mod/rule/Rule.h>

#include <ostream>

namespace mod {

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

} // namespace mod