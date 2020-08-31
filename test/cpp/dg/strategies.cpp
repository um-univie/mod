#include <mod/Error.hpp>
#include <mod/Function.hpp>
#include <mod/dg/Strategies.hpp>

#undef NDEBUG

#include <cassert>

using namespace mod;

void testFilterNull() {
	try {
		dg::Strategy::makeFilter(false, nullptr);
		std::exit(1);
	} catch(const LogicError &e) {
		assert(e.what() == std::string("The predicate is a null pointer."));
	}
}

void testExecuteNull() {
	try {
		dg::Strategy::makeExecute(nullptr);
		std::exit(1);
	} catch(const LogicError &e) {
		assert(e.what() == std::string("The callback is a null pointer."));
	}
}

void testLeftPredicateNull() {
	try {
		dg::Strategy::makeLeftPredicate(nullptr, nullptr);
		std::exit(1);
	} catch(const LogicError &e) {
		assert(e.what() == std::string("The predicate is a null pointer."));
	}
	try {
		std::function<bool(const Derivation &)> p = [](const Derivation &) { return false; };
		dg::Strategy::makeLeftPredicate(fromStdFunction(p), nullptr);
		std::exit(1);
	} catch(const LogicError &e) {
		assert(e.what() == std::string("The substrategy is a null pointer."));
	}
}

void testRightPredicateNull() {
	try {
		dg::Strategy::makeRightPredicate(nullptr, nullptr);
		std::exit(1);
	} catch(const LogicError &e) {
		assert(e.what() == std::string("The predicate is a null pointer."));
	}
	try {
		std::function<bool(const Derivation &)> p = [](const Derivation &) { return false; };
		dg::Strategy::makeRightPredicate(fromStdFunction(p), nullptr);
		std::exit(1);
	} catch(const LogicError &e) {
		assert(e.what() == std::string("The substrategy is a null pointer."));
	}
}

int main() {
	testFilterNull();
	testExecuteNull();
	testLeftPredicateNull();
	testRightPredicateNull();
}