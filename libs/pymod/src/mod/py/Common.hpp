#ifndef MOD_PY_COMMON_H
#define MOD_PY_COMMON_H

#include <boost/python.hpp>

namespace py = boost::python;
namespace mod {
namespace Py {

struct AttributeIsNotReadable {
};

inline void noGet(AttributeIsNotReadable) {}

// From https://wiki.python.org/moin/boost.python/HowTo#Dynamic_template_to-python_converters.

template<class T1, class T2>
struct PairToTupleConverter {

	static PyObject *convert(const std::pair<T1, T2> &pair) {
		return py::incref(py::make_tuple(pair.first, pair.second).ptr());
	}
};

} // namespace Py
} // namespace mod

#endif /* MOD_PY_COMMON_H */