#ifndef MOD_PY_FUNCTION_H
#define MOD_PY_FUNCTION_H

// The wrapping and haxing of reference counts could probably be done simpler.
// The arg wrapping should also be looked into.
// Use exportFunc to export the wrapper class for a given signature.

#include <mod/Function.hpp>

#include <boost/python.hpp>

#include <iostream>

namespace mod::Py {
namespace py = boost::python;
namespace detail {

template<typename T>
struct ArgWrap {
	static T wrap(T t) {
		return t;
	}
};

template<typename T>
struct ArgWrap<T &> {
	static auto wrap(T &t) -> decltype(boost::ref(t)) {
		return boost::ref(t);
	}
};

template<typename R>
struct Returner {
	static R doReturn(decltype(std::declval<py::override>()()) r) {
		return r;
	}
};

template<>
struct Returner<void> {
	static void doReturn(decltype(std::declval<py::override>()())) {}
};

template<typename F>
struct FunctionWrapper {
};

template<typename R, typename ...Args>
struct FunctionWrapper<R(Args...)> : mod::Function<R(Args...)>, py::wrapper<FunctionWrapper<R(Args...)> > {
	std::shared_ptr<mod::Function<R(Args...)> > clone() const {
		if(py::override f = this->get_override("clone")) {
			return f();
		} else {
			print(std::cerr << "ERROR: override of 'clone' not found in Function\n");
			std::cerr << std::endl;
			std::exit(1);
		}
	}

	void print(std::ostream &s) const {
		if(py::override f = this->get_override("__str__")) {
			std::string str = f();
			s << str;
		} else {
			std::cerr << "ERROR: override of '__str__' not found in Function" << std::endl;
			std::exit(1);
		}
	}

	R operator()(Args ...args) const {
		if(py::override f = this->get_override("__call__")) {
			return Returner<R>::doReturn(f(ArgWrap<Args>::wrap(args)...));
		} else {
			print(std::cerr << "ERROR: override of '__call__' not found in Function\n\t");
			std::cerr << std::endl;
			std::exit(1);
		}
	}
};

// It is used in the __init__ file so just make it an identity function.
template<typename T>
std::shared_ptr<T> sharedToStd(std::shared_ptr<T> p) {
	return p;
}

} // namespace detail

template<typename Sig>
void exportFunc(const char *name) {
	using Func = mod::Function<Sig>;
	using Wrap = detail::FunctionWrapper<Sig>;
	using SharedPtr = std::shared_ptr<Func>;
	using StoragePtr = std::shared_ptr<Wrap>;
	py::class_<Wrap, StoragePtr, boost::noncopyable>(name)
			.def("clone", py::pure_virtual(&Func::clone))
			.def("__str__", py::pure_virtual(&Func::print))
			.def("__call__", py::pure_virtual(&Func::operator()));
	py::implicitly_convertible<StoragePtr, SharedPtr>();
	py::def("_sharedToStd", &detail::sharedToStd<Func>);
}

} // namespace mod::Py

#endif /* MOD_PY_FUNCTION_H */
