#ifndef JLA_BOOST_FUNCTIONAL_H
#define	JLA_BOOST_FUNCTIONAL_H

namespace jla_boost {

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

#endif	/* JLA_BOOST_FUNCTIONAL_H */

