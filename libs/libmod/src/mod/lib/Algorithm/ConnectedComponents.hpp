#ifndef MOD_LIB_ALGORITHM_CONNECTEDCOMPONENTS_HPP
#define MOD_LIB_ALGORITHM_CONNECTEDCOMPONENTS_HPP

#include <cassert>
#include <vector>

namespace mod::lib {

struct ConnectedComponents {
	ConnectedComponents(int n) : components(n) {
		for(int i = 0; i != n; ++i)
			components[i] = i;
	}

	int findRoot(int i) {
		while(components[i] != i)
			i = components[i];
		return i;
	}

	void join(int a, int b) {
		a = findRoot(a);
		b = findRoot(b);
		// always make the smallest the root
		if(a < b) components[b] = a;
		else if(b < a) components[a] = b;
	}
public:
	int finalize() {
		int next = 0;
		for(int i = 0; i != components.size(); ++i) {
			// everything below i has been changed to component numbers
			// and joining always goes downward, so just do a single root-finding step
			const int root = components[i];
			if(root == i) {
				components[i] = next;
				++next;
			} else {
				assert(root < i);
				components[i] = components[root];
			}
		}
		return next;
	}
public:
	int operator[](int i) const {
		return components[i];
	}
private:
	std::vector<int> components;
};

} // namespace mod::lib

#endif // MOD_LIB_ALGORITHM_CONNECTEDCOMPONENTS_HPP