#ifndef MOD_LIB_ALGORITHM_POINT_HPP
#define MOD_LIB_ALGORITHM_POINT_HPP

#include <cmath>
#include <utility>

namespace mod::lib {

constexpr double pi = 3.14159265358979323846;

inline std::pair<double, double> pointRotation(double xRaw, double yRaw, int rotation) {
	double angle = rotation * pi / 180;
	auto s = std::sin(angle);
	auto c = std::cos(angle);
	double x = xRaw * c - yRaw * s;
	double y = xRaw * s + yRaw * c;
	return std::make_pair(x, y);
}

inline std::pair<double, double> pointTransform(double xRaw, double yRaw, int rotation, bool mirror) {
	if(mirror) xRaw *= -1;
	return pointRotation(xRaw, yRaw, rotation);
}

} // namespace mod::lib

#endif // MOD_LIB_ALGORITHM_POINT_HPP