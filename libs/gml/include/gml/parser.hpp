#ifndef GML_PARSER_HPP
#define GML_PARSER_HPP

#include <gml/ast.hpp>

#include <string_view>

namespace gml::parser {

bool parse(std::string_view src, ast::KeyValue &ast, std::ostream &err);

} // namespace gml::parser

#endif // GML_PARSER_HPP