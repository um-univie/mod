#ifndef GML_CONVERTER_EXPRESSIONS_HPP
#define GML_CONVERTER_EXPRESSIONS_HPP

#include <gml/converter_error.hpp>
#include <gml/value_type.hpp>

#include <boost/lexical_cast.hpp>

#include <ostream>

namespace gml::converter {
namespace detail {

struct ExpressionBase {
	bool checkKey(const std::string &key) const noexcept;
protected:
	ExpressionBase(const std::string &key) : key(key) {}
	// throws error
	[[noreturn]] void errorOnKey(const ast::KeyValue &kv) const;
	void checkAndErrorOnKey(const ast::KeyValue &kv) const;
	void checkAndErrorOnType(const ast::Value &value, ValueType expected) const;
protected:
	std::string key;
};

}  // namespace detail

template<typename AttrHandler>
struct Expression : detail::ExpressionBase {
protected:
	Expression(const std::string &key, AttrHandler attrHandler)
			: ExpressionBase(key), attrHandler(attrHandler) {}
protected:
	AttrHandler attrHandler;
};

#define MAKE_TERMINAL(Name, Type)                                                                \
    template<typename AttrHandler>                                                               \
    struct Name : Expression<AttrHandler> {                                                      \
        using Base = Expression<AttrHandler>;                                                    \
                                                                                                 \
        Name(const std::string &key, AttrHandler attrHandler) : Base(key, attrHandler) {}        \
                                                                                                 \
        template<typename ParentAttr>                                                            \
        void convert(const ast::KeyValue &kv, ParentAttr &parentAttr) const {                    \
            Base::checkAndErrorOnKey(kv);                                                        \
            Base::checkAndErrorOnType(kv.value, ValueType::Name);                                \
            Base::attrHandler(parentAttr, boost::get<Type>(kv.value));                           \
        }                                                                                        \
                                                                                                 \
        friend std::ostream &operator<<(std::ostream &s, const Name &expr) {                     \
            return s << #Name << "(" << expr.key << ")";                                         \
        }                                                                                        \
    };
MAKE_TERMINAL(Int, int)
MAKE_TERMINAL(Float, double)
MAKE_TERMINAL(String, std::string)
#undef MAKE_TERMINAL

struct Unused {
	template<typename T>
	Unused operator=(T &&) const {
		return Unused();
	}
};

struct Parent {
};

template<typename Expr>
struct ListElement {
	friend std::ostream &operator<<(std::ostream &s, const ListElement &el) {
		return s << "[" << el.lowerBound << ", " << el.upperBound << "](" << el.expr << ")";
	}
public:
	std::size_t lowerBound, upperBound;
	Expr expr;
};

template<std::size_t I, std::size_t N, typename ...Expr>
struct ListElementPrinter {
	static void print(std::ostream &s, const std::tuple<ListElement<Expr>...> &elems) {
		if(I > 0) s << ", ";
		s << std::get<I>(elems);
		ListElementPrinter<I + 1, N, Expr...>::print(s, elems);
	}
};

template<std::size_t N, typename ...Expr>
struct ListElementPrinter<N, N, Expr...> {
	static void print(std::ostream &s, const std::tuple<ListElement<Expr>...> &elems) {}
};

template<std::size_t I, std::size_t N, typename ...Expr>
struct ListElementHandler {
	template<typename ParentAttr>
	static void handle(const ast::KeyValue &kv, const std::tuple<ListElement<Expr>...> &elems,
	                   ParentAttr &parentAttr, std::array<std::size_t, N> &count) {
		auto &elem = std::get<I>(elems);
		if(!elem.expr.checkKey(kv.key))
			return ListElementHandler<I + 1, N, Expr...>::handle(kv, elems, parentAttr, count);
		elem.expr.convert(kv, parentAttr);
		++count[I];
		if(count[I] > elem.upperBound)
			throw error("Error at " + std::to_string(kv.line) + ":" + std::to_string(kv.column) + "."
			            + " Unexpected " + boost::lexical_cast<std::string>(elem.expr)
			            + ". Already got " + std::to_string(elem.upperBound) + " occurrences.");
	}
};

template<std::size_t N, typename ...Expr>
struct ListElementHandler<N, N, Expr...> {
	template<typename ParentAttr>
	static void handle(const ast::KeyValue &kv, const std::tuple<ListElement<Expr>...> &elems, ParentAttr &parentAttr,
	                   std::array<std::size_t, N> &count) {
		throw error("Error at " + std::to_string(kv.line) + ":" + std::to_string(kv.column) + "."
		            + " Unexpected list element with key '" + boost::lexical_cast<std::string>(kv.key) + "'.");
	}
};

template<std::size_t I, std::size_t N, typename ...Expr>
struct ListElementUpperBound {
	static void check(const std::tuple<ListElement<Expr>...> &elems, std::array<std::size_t, N> &count) {
		auto &elem = std::get<I>(elems);
		if(count[I] < elem.lowerBound)
			throw error(
					"Expected " + std::to_string(elem.lowerBound) + " of " + boost::lexical_cast<std::string>(elem.expr)
					+ ". Got only " + std::to_string(count[I]) + ".");
		return ListElementUpperBound<I + 1, N, Expr...>::check(elems, count);
	}
};

template<std::size_t N, typename ...Expr>
struct ListElementUpperBound<N, N, Expr...> {
	static void
	check(const std::tuple<ListElement<Expr>...> &elems, std::array<std::size_t, N> &count) {}
};

template<typename Type, typename AttrHandler, typename ParentAttr>
struct ListAttrHandler {
	ListAttrHandler(const AttrHandler &attrHandler, ParentAttr &parentAttr)
			: attrHandler(attrHandler), parentAttr(parentAttr) {}

	Type &getAttr() {
		return attr;
	}

	void assignToParent() const {
		attrHandler(parentAttr, std::move(attr));
	}
private:
	Type attr;
	const AttrHandler &attrHandler;
	ParentAttr &parentAttr;
};

template<typename AttrHandler, typename ParentAttr>
struct ListAttrHandler<Unused, AttrHandler, ParentAttr> {
	ListAttrHandler(const AttrHandler &, const ParentAttr &) {}

	Unused &getAttr() {
		return unused;
	}

	void assignToParent() const {}
private:
	Unused unused;
};

template<typename AttrHandler, typename ParentAttr>
struct ListAttrHandler<Parent, AttrHandler, ParentAttr> {
	ListAttrHandler(const AttrHandler &attrHandler, ParentAttr &parentAttr)
			: attrHandler(attrHandler), parentAttr(parentAttr) {}

	ParentAttr &getAttr() {
		return parentAttr;
	}

	void assignToParent() const {}
private:
	const AttrHandler &attrHandler;
	ParentAttr &parentAttr;
};

template<typename Type, typename AttrHandler, typename ...Expr>
struct List : Expression<AttrHandler> {
	using Base = Expression<AttrHandler>;
	using Elems = std::tuple<ListElement<Expr>...>;

	List(const std::string &key, AttrHandler attrHandler, const Elems &elems)
			: Base(key, attrHandler), elems(elems) {}

	template<typename ParentAttr>
	bool convert(const ast::KeyValue &kv, ParentAttr &parentAttr) const {
		Base::checkAndErrorOnKey(kv);
		Base::checkAndErrorOnType(kv.value, ValueType::List);
		const ast::List &value = boost::get<x3::forward_ast<ast::List> >(kv.value);
		std::array<std::size_t, sizeof...(Expr)> count;
		count.fill(0);
		ListAttrHandler <Type, AttrHandler, ParentAttr> ourAttr(this->attrHandler, parentAttr);
		for(const ast::KeyValue &kvElem : value.list)
			ListElementHandler<0, sizeof...(Expr), Expr...>::handle(kvElem, elems, ourAttr.getAttr(), count);
		ListElementUpperBound<0, sizeof...(Expr), Expr...>::check(elems, count);
		ourAttr.assignToParent();
		return true;
	}

	friend std::ostream &operator<<(std::ostream &s, const List &expr) {
		s << "List(" << expr.key << ")[";
		ListElementPrinter<0, std::tuple_size<Elems>::value, Expr...>::print(s, expr.elems);
		return s << "]";
	}
private:
	Elems elems;
};

template<typename Expression>
Expression asConverter(const Expression &expr) {
	return expr;
}

} // namespace gml::converter

#endif /* GML_CONVERTER_EXPRESSIONS_HPP */
