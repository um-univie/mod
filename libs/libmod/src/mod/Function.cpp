#include "Function.hpp"

namespace mod {
namespace detail_function {

void dont_call_only_for_test() {
	std::function<void() > f = []() {
	};
	auto a = fromStdFunction(f);
	auto b = fromStdFunction("test", f);
}

} // namespace detail_function
} // namespace mod