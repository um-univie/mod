#ifndef GML_PARSER_HPP
#define GML_PARSER_HPP

#include <gml/ast.hpp>

#include <string_view>

namespace gml::parser {

struct error : std::exception {
	error(std::string msg) : msg(std::move(msg)) {}
	virtual const char *what() const noexcept { return msg.c_str(); }
private:
	std::string msg;
};

ast::KeyValue parse(std::string_view src);

} // namespace gml::parser

#endif // GML_PARSER_HPP