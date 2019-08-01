#ifndef GML_PARSER_HPP
#define GML_PARSER_HPP

#include <gml/ast.hpp>

namespace gml {
namespace parser {

bool parse(std::istream &s, ast::KeyValue &ast, std::ostream &err);

} // namespace parser
} // namespace gml

#endif /* GML_PARSER_HPP */