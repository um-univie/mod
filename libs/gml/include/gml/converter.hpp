#ifndef GML_CONVERTER_HPP
#define GML_CONVERTER_HPP

#include <gml/converter_expressions.hpp>

namespace gml {
namespace converter {

template<typename IterBegin, typename IterEnd, typename Expression, typename Attr>
bool convert(IterBegin &iterBegin, const IterEnd &iterEnd, const Expression &expr, std::ostream &err, Attr &attr) {
	if(iterBegin == iterEnd) {
		err << "Expected root.";
		return false;
	}
	bool res = asConverter(expr).convert(*iterBegin, err, attr);
	if(!res) return false;
	++iterBegin;
	if(iterBegin != iterEnd) {
		err << "Unexpected second root.";
		return false;
	}
	return true;
}

template<typename IterBegin, typename IterEnd, typename Expression>
bool convert(IterBegin &iterBegin, const IterEnd &iterEnd, const Expression &expr, std::ostream &err) {
	Unused unused;
	return convert(iterBegin, iterEnd, expr, err, unused);
}

} // namespace converter
} // namespace gml

#endif /* GML_CONVERTER_HPP */