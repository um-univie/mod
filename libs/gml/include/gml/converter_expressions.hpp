#ifndef GML_CONVERTER_EXPRESSIONS_HPP
#define GML_CONVERTER_EXPRESSIONS_HPP

#include <gml/value_type.hpp>

#include <boost/variant/get.hpp>

#include <ostream>

namespace gml {
namespace converter {
namespace detail {

struct ExpressionBase {
	bool checkKey(const std::string &key) const;
protected:
	ExpressionBase(const std::string &key) : key(key) {}
	void errorOnKey(const ast::KeyValue &kv, std::ostream &err) const;
	bool checkAndErrorOnKey(const ast::KeyValue &kv, std::ostream &err) const;
	bool checkAndErrorOnType(const ast::Value &value, std::ostream &err, ValueType expected) const;
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
        Name(const std::string &key, AttrHandler attrHandler)                                    \
        : Base(key, attrHandler) { }                                                             \
                                                                                                 \
        template<typename ParentAttr>                                                            \
        bool convert(const ast::KeyValue &kv, std::ostream &err, ParentAttr &parentAttr) const { \
            bool res = Base::checkAndErrorOnKey(kv, err)                                         \
                       && Base::checkAndErrorOnType(kv.value, err, ValueType::Name);             \
            if(!res) return res;                                                                 \
            Base::attrHandler(parentAttr, boost::get<Type>(kv.value));                           \
            return true;                                                                         \
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
	static bool handle(const ast::KeyValue &kv, const std::tuple<ListElement<Expr>...> &elems,
	                   std::ostream &err, ParentAttr &parentAttr, std::array<std::size_t, N> &count) {
		auto &elem = std::get<I>(elems);
		if(!elem.expr.checkKey(kv.key)) {
			return ListElementHandler<I + 1, N, Expr...>::handle(kv, elems, err, parentAttr, count);
		} else if(elem.expr.convert(kv, err, parentAttr)) {
			++count[I];
			if(count[I] > elem.upperBound) {
				err << "Error at " << kv.line << ":" << kv.column << ".";
				err << " Unexpected " << elem.expr << ". Already got " << elem.upperBound << " occurrences.";
				return false;
			}
			return true;
		} else {
			return false;
		}
	}
};

template<std::size_t N, typename ...Expr>
struct ListElementHandler<N, N, Expr...> {
	template<typename ParentAttr>
	static bool handle(const ast::KeyValue &kv, const std::tuple<ListElement<Expr>...> &elems,
	                   std::ostream &err, ParentAttr &parentAttr, std::array<std::size_t, N> &count) {
		err << "Error at " << kv.line << ":" << kv.column << ".";
		err << " Unexpected list element with key '" << kv.key << "'.";
		return false;
	}
};

template<std::size_t I, std::size_t N, typename ...Expr>
struct ListElementUpperBound {
	static bool
	check(const std::tuple<ListElement<Expr>...> &elems, std::ostream &err, std::array<std::size_t, N> &count) {
		auto &elem = std::get<I>(elems);
		if(count[I] < elem.lowerBound) {
			err << "Expected " << elem.lowerBound << " of " << elem.expr << ". Got only " << count[I] << ".";
			return false;
		}
		return ListElementUpperBound<I + 1, N, Expr...>::check(elems, err, count);
	}
};

template<std::size_t N, typename ...Expr>
struct ListElementUpperBound<N, N, Expr...> {
	static bool
	check(const std::tuple<ListElement<Expr>...> &elems, std::ostream &err, std::array<std::size_t, N> &count) {
		return true;
	}
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
	bool convert(const ast::KeyValue &kv, std::ostream &err, ParentAttr &parentAttr) const {
		bool res = Base::checkAndErrorOnKey(kv, err)
		           && Base::checkAndErrorOnType(kv.value, err, ValueType::List);
		if(!res) return res;
		const ast::List &value = boost::get<x3::forward_ast<ast::List> >(kv.value);
		std::array<std::size_t, sizeof...(Expr)> count;
		count.fill(0);
		ListAttrHandler <Type, AttrHandler, ParentAttr> ourAttr(this->attrHandler, parentAttr);
		for(const ast::KeyValue &kv : value.list) {
			bool res = ListElementHandler<0, sizeof...(Expr), Expr...>::handle(kv, elems, err, ourAttr.getAttr(),
			                                                                   count);
			if(!res) return false;
		}
		res = ListElementUpperBound<0, sizeof...(Expr), Expr...>::check(elems, err, count);
		if(!res) return false;
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

} // namespace converter
} // namespace gml

#endif /* GML_CONVERTER_EXPRESSIONS_HPP */
