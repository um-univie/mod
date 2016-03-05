#ifndef JLA_BOOST_MEMORY_HPP
#define	JLA_BOOST_MEMORY_HPP

#include <memory>

namespace jla_boost {

// copied from http://stackoverflow.com/questions/7038357/make-unique-and-perfect-forwarding

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace jla_boost

using jla_boost::make_unique;

#endif	/* JLA_BOOST_MEMORY_HPP */