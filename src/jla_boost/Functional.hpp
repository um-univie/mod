#ifndef JLA_BOOST_FUNCTIONAL_H
#define JLA_BOOST_FUNCTIONAL_H

#include <functional> // so we can act as replacement

namespace jla_boost {

template<typename Ret = void>
struct Nop {

	template<typename ...Args>
	Ret operator()(Args&&...) const {
		return Ret();
	}
};

struct Identity {

	template<typename T>
	T operator()(T &&t) const {
		return std::forward<T>(t);
	}
};

struct AlwaysTrue {

	template<typename ...Args>
	bool operator()(Args&&...) const {
		return true;
	}
};

struct AlwaysFalse {

	template<typename ...Args>
	bool operator()(Args&&...) const {
		return false;
	}
};

} // namespace jla_boost

#endif /* JLA_BOOST_FUNCTIONAL_H */

