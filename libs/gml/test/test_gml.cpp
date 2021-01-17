#include <gml/parser.hpp>
#include <gml/converter.hpp>
#include <gml/converter_edsl.hpp>

#include <iostream>
#include <sstream>

template<typename T>
std::ostream &operator<<(std::ostream &s, const std::vector<T> &v) {
	s << "[";
	if(!v.empty()) s << v.front();
	for(std::size_t i = 1; i < v.size(); ++i) s << ", " << v[i];
	return s << "]";
}

struct T {
	friend std::ostream &operator<<(std::ostream &s, const T &t) {
		return s << "i = " << t.i << ", d = " << t.d << ", s = '" << t.s << "'";
	}
public:
	int i = 0;
	double d = 0;
	std::string s = "t";
};

struct V {
	friend std::ostream &operator<<(std::ostream &s, const V &v) {
		return s << "vi = " << v.vi << ", vd = " << v.vd << ", vs = " << v.vs;
	}
public:
	std::vector<int> vi;
	std::vector<double> vd;
	std::vector<std::string> vs;
};

void print(std::ostream &) {}

template<typename Attr>
void print(std::ostream &s, const Attr &attr) {
	std::cout << "Attr: " << attr << std::endl;
}

template<typename Expression, typename ...Attr>
void test(std::string src, const Expression &expr, Attr &...attr) {
	std::cout << "Testing: '" << src << "' with '" << asConverter(expr) << "'" << std::endl << std::string(70, '-')
	          << std::endl;
	std::stringstream err;
	gml::ast::KeyValue ast;
	bool res = gml::parser::parse(src, ast, err);
	if(!res) {
		std::cout << err.str() << std::endl;
		std::cout << "Parsing failed." << std::endl;
		std::exit(1);
	}
	auto iterBegin = &ast;
	auto iterEnd = iterBegin + 1;
	res = gml::converter::convert(iterBegin, iterEnd, expr, err, attr...);
	if(!res) {
		std::cout << err.str() << std::endl << std::endl;
		std::cout << "Expected success." << std::endl;
		std::exit(1);
	} else {
		print(std::cout, attr...);
	}
}

template<typename Expression, typename ...Attr>
void fail(std::string src, const Expression &expr, Attr &...attr) {
	std::cout << "Testing for fail: '" << src << "' with '" << asConverter(expr) << "'" << std::endl
	          << std::string(70, '-') << std::endl;
	std::stringstream err;
	gml::ast::KeyValue ast;
	bool res = gml::parser::parse(src, ast, err);
	if(!res) {
		std::cout << err.str() << std::endl;
		std::cout << "Parsing failed." << std::endl;
		return;
	}
	auto iterBegin = &ast;
	auto iterEnd = iterBegin + 1;
	res = gml::converter::convert(iterBegin, iterEnd, expr, err, attr...);
	if(!res) {
		std::cout << err.str() << std::endl << std::endl;
	} else {
		print(std::cout, attr...);
		std::exit(1);
	}
}

int main() {
	using namespace gml::converter::edsl;
	fail("", int_("id"));
	fail("a 5 a 5", int_("a"));
	fail("a 5", int_("id"));
	fail("id \"a\"", int_("id"));
	test("w 4.5", float_("w"));
	test("label \"a\"", string("label"));
	test("node []", list("node"));
	fail("node [ id 0 ]", list("node"));
	fail("node []", list("node")(int_("id"), 1, 1));
	fail("node [ id 0 ]", list("node")(int_("id"), 0, 0));
	{
		int i = 0;
		double d = 0;
		std::string s = "t";
		test("id 42", int_("id"), i);
		test("w 3.14", float_("w"), d);
		test("s \"hest\"", string("s"), s);
	}
	{
		T t;
		test("id 42", int_("id", &T::i), t);
		test("w 3.14", float_("w", &T::d), t);
		test("s \"hest\"", string("s", &T::s), t);
	}
	{
		std::vector<int> vi;
		std::vector<double> vd;
		std::vector<std::string> vs;
		test("id 42", int_("id"), vi);
		test("w 3.14", float_("w"), vd);
		test("s \"hest\"", string("s"), vs);
	}
	{
		V v;
		test("id 42", int_("id", &V::vi), v);
		test("w 3.14", float_("w", &V::vd), v);
		test("s \"hest\"", string("s", &V::vs), v);
	}
	{
		T t;
		auto expr = list<Parent>("node")
				(int_("id", &T::i), 1, 1)
				(string("label", &T::s), 1, 1);
		test("node [ id 42 label \"hest\" ]", expr, t);
	}
	{
		T t;
		auto expr = list<T>("node")
				(int_("id", &T::i), 1, 1)
				(string("label", &T::s), 1, 1);
		test("node [ id 42 label \"hest\" ]", expr, t);
	}
	{
		std::vector<T> vt;
		auto expr = list<T>("node")
				(int_("id", &T::i), 1, 1)
				(string("label", &T::s), 1, 1);
		test("node [ id 42 label \"hest\" ]", expr, vt);
	}
}
