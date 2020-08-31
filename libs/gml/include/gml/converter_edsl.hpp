#ifndef GML_CONVERTER_EDSL_HPP
#define GML_CONVERTER_EDSL_HPP

#include <gml/attr_handler.hpp>
#include <gml/converter_expressions.hpp>

namespace gml {
namespace converter {
inline namespace edsl {
// such that 'using namespace gml::converter::edsl;' gives Parent as well
using gml::converter::Parent;

#define MAKE_CREATOR(Name, VarName)                                                    \
    struct Name ## Creator {                                                           \
        Name<AttrHandler<Unused> > operator()(const std::string &key) const {          \
            return Name<AttrHandler<Unused> >(key, AttrHandler<Unused>());             \
        }                                                                              \
                                                                                       \
      template<typename Data>                                                          \
        Name<AttrHandler<Data> > operator()(const std::string &key, Data data) const { \
            return Name<AttrHandler<Data> >(key, AttrHandler<Data>(data));             \
        }                                                                              \
    };                                                                                 \
    const Name ## Creator VarName;
MAKE_CREATOR(Int, int_)
MAKE_CREATOR(Float, float_)
MAKE_CREATOR(String, string)
#undef MAKE_CREATOR

template<typename Type, typename AttrHandler, typename ...Expr>
struct ListWithKey {
	using Elems = std::tuple<ListElement<Expr>...>;

	ListWithKey(const std::string &key, AttrHandler attrHandler)
			: key(key), attrHandler(attrHandler) {}

	ListWithKey(const std::string &key, AttrHandler attrHandler, Elems elems)
			: key(key), attrHandler(attrHandler), elems(elems) {}

	template<typename ExprT>
	auto operator()(ExprT &&expr, std::size_t lowerBound, std::size_t upperBound) const
	-> ListWithKey<Type, AttrHandler, Expr..., decltype(asConverter(expr))> {
		using AsConverterExpr = decltype(asConverter(expr));
		ListElement<AsConverterExpr> newElem{lowerBound, upperBound, asConverter(expr)};
		auto newElems = std::tuple_cat(elems, std::tuple<ListElement<AsConverterExpr> >(newElem));
		return ListWithKey<Type, AttrHandler, Expr..., AsConverterExpr>(key, attrHandler, newElems);
	}

	template<typename ExprT>
	auto operator()(ExprT &&expr, std::size_t lowerBound) const
	-> ListWithKey<Type, AttrHandler, Expr..., decltype(asConverter(expr))> {
		return (*this)(std::forward<ExprT>(expr), lowerBound, std::numeric_limits<std::size_t>::max());
	}

	template<typename ExprT>
	auto operator()(ExprT &&expr) const
	-> ListWithKey<Type, AttrHandler, Expr..., decltype(asConverter(expr))> {
		return (*this)(std::forward<ExprT>(expr), 0, std::numeric_limits<std::size_t>::max());
	}
private:
	std::string key;
	AttrHandler attrHandler;
	Elems elems;
public:

	friend List<Type, AttrHandler, Expr...> asConverter(const ListWithKey<Type, AttrHandler, Expr...> &lwk) {
		return List<Type, AttrHandler, Expr...>(lwk.key, lwk.attrHandler, lwk.elems);
	}
};

template<typename Type = Unused, typename AttrHandlerData = Unused>
ListWithKey<Type, AttrHandler<AttrHandlerData> >
list(const std::string &key, AttrHandlerData attrHandlerData = Unused()) {
	return ListWithKey<Type, AttrHandler<AttrHandlerData> >(key, AttrHandler<AttrHandlerData>(attrHandlerData));
}

} // inline namespace edsl
} // namespace converter
} // namespace gml

#endif /* GML_CONVERTER_EDSL_HPP */