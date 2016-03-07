#ifndef MOD_PY_FUNCTION_H
#define	MOD_PY_FUNCTION_H

// The wrapping and haxing of reference counts could probably be done simpler.
// The arg wrapping should also be looked into.
// Use exportFunc to export the wrapper class for a given signature.

#include <mod/Function.h>

#include <iostream>

namespace mod {
namespace Py {
namespace py = boost::python;
namespace detail {

template<typename T>
struct ArgWrap {

	static T wrap(T t) {
		return t;
	}
};

template<typename T>
struct ArgWrap<T&> {
	static auto wrap(T &t) -> decltype(boost::ref(t)) {
		return boost::ref(t);
	}
};

template<typename R>
struct Returner {

	static R doReturn(decltype((*static_cast<py::override*> (nullptr))()) r) {
		return r;
	}
};

template<>
struct Returner<void> {

	static void doReturn(decltype((*static_cast<py::override*> (nullptr))())) { }
};

template<typename F>
struct FunctionWrapper {
};

template<>
template<typename R, typename ...Args>
struct FunctionWrapper<R(Args...)> : mod::Function<R(Args...)>, py::wrapper<FunctionWrapper<R(Args...)> > {

	std::shared_ptr < mod::Function < R(Args...)> > clone() const {
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

// sharedToStd* is adapted from http://stackoverflow.com/questions/6326757/conversion-from-boostshared-ptr-to-stdshared-ptr

template<typename T>
void sharedToStd_doRelease(const typename boost::shared_ptr<T> &, T*) { }

template<typename T>
std::shared_ptr<T> sharedToStd(boost::shared_ptr<T> p) {
	return std::shared_ptr<T>(p.get(), std::bind(&sharedToStd_doRelease<T>, p, std::placeholders::_1));
}

} // namespace detail

template<typename Sig>
void exportFunc(const char *name) {
	// TODO: change the held type to std::shared_ptr instead of boost::shared_ptr at some point
	// currently boost::shared_ptr is copied into C++ with a custom deleter which holds a handle<>
	// to the PyObject*, while the PyObject* holds a shared_ptr without this deleter.
	// std::shared_ptr is _not_ handled this way, meaning object slicing might happen
	// sharedToStd holds the boost::shared_ptr in the deleter of the std::shared_ptr
	using Func = mod::Function<Sig>;
	using Wrap = detail::FunctionWrapper<Sig>;
	py::class_<Wrap, boost::shared_ptr<Wrap>, boost::noncopyable>(name)
			.def("clone", py::pure_virtual(&Func::clone))
			.def("__str__", py::pure_virtual(&Func::print))
			.def("__call__", py::pure_virtual(&Func::operator()))
			;
	py::implicitly_convertible<boost::shared_ptr<Wrap>, boost::shared_ptr<Func> >();
	py::register_ptr_to_python<std::shared_ptr<Func> >();
	py::def("_sharedToStd", &detail::sharedToStd<Func>);
}

} // namespace Py
} // namespace mod

#endif	/* MOD_PY_FUNCTION_H */