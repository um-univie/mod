#ifndef GML_VALUE_TYPE_HPP
#define GML_VALUE_TYPE_HPP

#include <cassert>
#include <iosfwd>

namespace gml {

enum class ValueType {
	Int, Float, String, List
};

inline std::ostream &operator<<(std::ostream &s, ValueType vt) {
	switch(vt) {
	case ValueType::Int: return s << "Int";
	case ValueType::Float: return s << "Float";
	case ValueType::String: return s << "String";
	case ValueType::List: return s << "List";
	}
	assert(false);
	std::abort();
}

struct ValueTypeVisitor : boost::static_visitor<ValueType> {

	ValueType operator()(const int&) const {
		return ValueType::Int;
	}

	ValueType operator()(const double&) const {
		return ValueType::Float;
	}

	ValueType operator()(const std::string&) const {
		return ValueType::String;
	}

	ValueType operator()(const gml::ast::List&) const {
		return ValueType::List;
	}
};

} // namespace gml

#endif /* GML_VALUE_TYPE_HPP */