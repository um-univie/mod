#include <mod/Error.hpp>
#include <mod/dg/Printer.hpp>

#undef NDEBUG

#include <cassert>

using namespace mod;

template<typename F, typename ...Ts>
void test(F f, const Ts& ...ts) {
	dg::Printer p;
	try {
		(p.*f)({}, ts...);
		assert(false);
	} catch(const LogicError &e) {
		assert(e.what() == std::string("Can not push empty callback."));
	}
}

int main() {
	test(&dg::Printer::pushVertexVisible);
	test(&dg::Printer::pushEdgeVisible);
	test(&dg::Printer::pushVertexLabel);
	test(&dg::Printer::pushEdgeLabel);
	test(&dg::Printer::pushVertexColour, true);
	test(&dg::Printer::pushEdgeColour);
	test(&dg::Printer::setRotationOverwrite);
	test(&dg::Printer::setMirrorOverwrite);
}