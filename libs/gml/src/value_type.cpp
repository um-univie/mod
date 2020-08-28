#include <gml/value_type.hpp>

#include <ostream>

namespace gml {

std::ostream &operator<<(std::ostream &s, ValueType vt) {
	switch(vt) {
	case ValueType::Int: return s << "Int";
	case ValueType::Float: return s << "Float";
	case ValueType::String: return s << "String";
	case ValueType::List: return s << "List";
	}
	assert(false);
	std::abort();
}

} // namespace gml