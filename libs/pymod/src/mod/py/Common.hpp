#ifndef MOD_PY_COMMON_H
#define MOD_PY_COMMON_H

#include <boost/optional.hpp>
// TODO: https://github.com/boostorg/python/pull/296
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/python.hpp>
#undef BOOST_BIND_GLOBAL_PLACEHOLDERS

namespace py = boost::python;
namespace mod::Py {

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

// https://stackoverflow.com/questions/36485840/wrap-boostoptional-using-boostpython
template<typename T>
struct ToPythonOptionalValue {
	static PyObject *convert(boost::optional<T> obj) {
		return obj
		       ? py::incref(py::object(*obj).ptr())
		       : py::incref(py::object().ptr());
	}
};

} // namespace mod::Py

#endif /* MOD_PY_COMMON_H */