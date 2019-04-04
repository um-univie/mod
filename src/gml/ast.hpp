#ifndef GML_AST_HPP
#define GML_AST_HPP

#include <boost/fusion/adapted/struct.hpp>

#include <boost/spirit/home/x3/support/ast/variant.hpp>

#include <string>
#include <vector>

namespace spirit = boost::spirit;
namespace x3 = boost::spirit::x3;

namespace gml {
namespace ast {

struct LocationInfo {
	std::size_t line, column;
};

struct List;

struct Value : x3::variant<int, double, std::string, x3::forward_ast<List> >, LocationInfo {
	using base_type::base_type;
	using base_type::operator=;
};

struct KeyValue : LocationInfo {
	std::string key;
	Value value;
};

struct List {
	std::vector<KeyValue> list;
};

} // namespace ast
} // namespace gml

BOOST_FUSION_ADAPT_STRUCT(gml::ast::KeyValue,
		(std::string, key)
		(gml::ast::Value, value)
		)
BOOST_FUSION_ADAPT_STRUCT(gml::ast::List,
		(std::vector<gml::ast::KeyValue>, list)
		)

#endif /* GML_AST_HPP */