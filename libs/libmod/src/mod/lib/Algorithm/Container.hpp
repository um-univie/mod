#ifndef MOD_LIB_ALGORITHM_CONTAINER_HPP
#define MOD_LIB_ALGORITHM_CONTAINER_HPP

#include <algorithm>
#include <utility>

namespace mod::lib {

template<typename Container, typename T, typename Pred>
std::pair<T, bool> findAndInsert(Container &c, T &&t, Pred pred) {
	auto iter = std::find_if(begin(c), end(c), [&](const T &tCand) {
		return pred(tCand, t);
	});
	if(iter != end(c)) return {*iter, false};
	else {
		iter = c.insert(end(c), std::move(t));
		return {*iter, true};
	}
}

} // namesapce mod::lib

#endif // MOD_LIB_ALGORITHM_CONTAINER_HPP