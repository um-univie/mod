#include "Random.hpp"

#include <mod/lib/IO/IO.hpp>

#include <algorithm>

namespace mod {
namespace lib {

Random::Random() {
	reseed(std::random_device()());
}

std::mt19937 &Random::getEngine() {
	return engine;
}

void Random::reseed(unsigned int seed) {
	this->seed = seed;
	engine.seed(this->seed);
	IO::log() << "RNG seed: " << this->seed << std::endl;
}

Random &Random::getInstance() {
	static Random rnd;
	return rnd;
}

Random::Engine &getRng() {
	return Random::getInstance().getEngine();
}

std::vector<std::size_t> makePermutation(std::size_t n) {
	std::vector<std::size_t> p;
	p.reserve(n);
	for(std::size_t i = 0; i < n; i++) p.push_back(i);
	std::shuffle(begin(p), end(p), getRng());
	return p;
}

std::vector<size_t> invertPermutation(const std::vector<size_t> &p) {
	std::vector<size_t> pInverted(p.size());
	for(unsigned int i = 0; i < p.size(); i++) {
		assert(p[i] < p.size());
		pInverted[p[i]] = i;
	}
	return pInverted;
}

} // namespace lib
} // namespace mod
