#ifndef GML_VALUE_TYPE_HPP
#define GML_VALUE_TYPE_HPP

#include <boost/variant/static_visitor.hpp>

#include <gml/ast.hpp>

#include <cassert>
#include <iosfwd>

namespace gml {

enum class ValueType {
	Int, Float, String, List
};

std::ostream &operator<<(std::ostream &s, ValueType vt);

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