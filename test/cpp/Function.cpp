#include <mod/Function.hpp>

int main() {
	std::function<void()> f = []() {
	};
	auto a = mod::fromStdFunction(f);
	auto b = mod::fromStdFunction("test", f);
}