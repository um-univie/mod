#include <mod/Error.hpp>
#include <mod/dg/Printer.hpp>

#undef NDEBUG

#include <cassert>

using namespace mod;


template<typename F, typename ...Ts>
void test(F f, const std::string &msg, const Ts &...ts) {
	dg::Printer p;
	try {
		(p.*f)({}, ts...);
		assert(false);
	} catch(const LogicError &e) {
		assert(e.what() == msg);
	}
}

template<typename F, typename ...Ts>
void testPush(F f, const Ts &...ts) {
	test(f, "Can not push empty callback.", ts...);
}

template<typename F, typename ...Ts>
void testSet(F f, const Ts &...ts) {
	test(f, "Can not set empty callback.", ts...);
}

int main() {
	testPush(&dg::Printer::pushVertexVisible);
	testPush(&dg::Printer::pushEdgeVisible);
	testPush(&dg::Printer::pushVertexLabel);
	testPush(&dg::Printer::pushEdgeLabel);
	testPush(&dg::Printer::pushVertexColour, true);
	testPush(&dg::Printer::pushEdgeColour);
	testSet(&dg::Printer::setRotationOverwrite);
	testSet(&dg::Printer::setMirrorOverwrite);
}