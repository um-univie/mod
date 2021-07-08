#ifndef GML_CONVERTER_HPP
#define GML_CONVERTER_HPP

#include <gml/converter_expressions.hpp>

namespace gml::converter {

template<typename IterBegin, typename IterEnd, typename Expression, typename Attr>
void convert(IterBegin &iterBegin, const IterEnd &iterEnd, const Expression &expr, Attr &attr) {
	if(iterBegin == iterEnd)
		throw error("Expected root.");
	asConverter(expr).convert(*iterBegin, attr);
	++iterBegin;
	if(iterBegin != iterEnd)
		throw error("Unexpected second root.");
}

template<typename IterBegin, typename IterEnd, typename Expression>
void convert(IterBegin &iterBegin, const IterEnd &iterEnd, const Expression &expr) {
	Unused unused;
	convert(iterBegin, iterEnd, expr, unused);
}

} // namespace gml::converter

#endif /* GML_CONVERTER_HPP */