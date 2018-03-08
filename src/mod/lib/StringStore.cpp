#include "StringStore.h"

#include <cassert>
#include <iostream>

namespace mod {
namespace lib {

bool StringStore::hasString(const std::string &s) const {
	return index.find(s) != end(index);
}

std::size_t StringStore::getIndex(const std::string &s) const {

	struct DoPrint {

		DoPrint(const StringStore &store, const std::string &s) : store(store), s(s) { }

		~DoPrint() {
			std::cout << "StringStore getIndex(" << s << ")" << std::endl;
			std::cout << "================================================" << std::endl;
			for(const auto &p : store.index) {
				assert(p.second < store.strings.size());
				assert(store.strings[p.second] == p.first);
				std::cout << "\t" << p.second << " -> " << p.first << std::endl;
			}
		}
	private:
		const StringStore &store;
		const std::string &s;
	} /*doPrint(*this, s)*/;
	auto pIter = index.emplace(s, strings.size());
	if(pIter.second) strings.push_back(s);
	return pIter.first->second;
}

const std::string &StringStore::getString(std::size_t index) const {
	assert(index < strings.size());
	return strings[index];
}

} // namespace lib
} // namespace mod