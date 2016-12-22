#ifndef GML_AST_HPP
#define GML_AST_HPP

#include <boost/fusion/adapted/struct.hpp>

#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>

#include <string>
#include <vector>

namespace gml {
namespace ast {

struct LocationInfo {
	std::size_t line, column;
};

struct List;
using ValueStore = boost::variant<int, double, std::string, boost::recursive_wrapper<List> >;

struct Value : LocationInfo {
	Value() = default;

	Value(ValueStore value) : value(value) { }
public:
	ValueStore value;
};

struct KeyValue : LocationInfo {
	std::string key;
	Value value;
};

struct List {
	List() = default;

	List(std::vector<KeyValue> list) : list(std::move(list)) { }
public:
	std::vector<KeyValue> list;
	int dummy;
};

} // namespace ast
} // namespace gml

BOOST_FUSION_ADAPT_STRUCT(gml::ast::Value,
		(gml::ast::ValueStore, value)
		(int, dummy)
		)
BOOST_FUSION_ADAPT_STRUCT(gml::ast::KeyValue,
		(std::string, key)
		(gml::ast::Value, value)
		)
BOOST_FUSION_ADAPT_STRUCT(gml::ast::List,
		(std::vector<gml::ast::KeyValue>, list)
		(int, dummy)
		)

#endif /* GML_AST_HPP */