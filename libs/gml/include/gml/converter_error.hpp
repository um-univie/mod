#ifndef MOD_CONVERTER_ERROR_HPP
#define MOD_CONVERTER_ERROR_HPP

#include <exception>
#include <string>

namespace gml::converter {

struct error : std::exception {
	error(std::string msg) : msg(std::move(msg)) {}
	virtual const char *what() const noexcept { return msg.c_str(); }
private:
	std::string msg;
};

} // namespace gml::converter

#endif // MOD_CONVERTER_ERROR_HPP