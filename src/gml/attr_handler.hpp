#ifndef GML_ATTR_HANDLER_HPP
#define GML_ATTR_HANDLER_HPP

#include <gml/converter_expressions.hpp>

namespace gml {
namespace converter {

template<typename Data>
struct AttrHandler {
};

template<>
struct AttrHandler<Unused> {

	AttrHandler() { }

	AttrHandler(Unused) { }

	template<typename Res, typename Attr>
	auto operator()(Res &res, Attr &&attr) const
	-> decltype(res = std::forward<Attr>(attr), void()) {
		res = std::forward<Attr>(attr);
	}

	template<typename Res, typename Attr>
	auto operator()(Res &res, Attr &&attr) const
	-> decltype(res.push_back(std::forward<Attr>(attr)), void()) {
		res.push_back(std::forward<Attr>(attr));
	}
};

template<> // member object pointer
template<typename C, typename T>
struct AttrHandler<T C::*> {

	AttrHandler(T C::*m) : m(m) { }

	template<typename Res, typename Attr>
	void operator()(Res &res, Attr &&attr) const {
		AttrHandler<Unused>()(res.*m, std::forward<Attr>(attr));
	}
private:
	T C::*m;
};

} // namespace converter
} // namespace gml

#endif /* ATTR_HANDLER_HPP */