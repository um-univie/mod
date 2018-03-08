#ifndef MOD_FUNCTION_H
#define	MOD_FUNCTION_H

// rst: The template class ``Function`` is used throughout the library to represent
// rst: functors.

#include <boost/type_traits/function_traits.hpp>

#include <functional>
#include <iosfwd>
#include <memory>
#include <string>

namespace mod {

// rst-class: template<typename R, typename ...Args> Function<R(Args...)>
// rst:
// rst:		Abstract base class template for fuctors used in the library.
// rst:
// rst-class-start:

template<typename Sig>
struct Function {
};

template<typename R, typename ...Args>
struct Function<R(Args...)> {

	virtual ~Function() { };
	// rst: .. function:: virtual std::shared_ptr<Function<R(Args...)> > clone() const = 0
	// rst:
	// rst:		Cloning function used when a copy of a given functor is needed.
	// rst:
	virtual std::shared_ptr<Function<R(Args...)> > clone() const = 0;
	// rst: .. function:: virtual void print(std::ostream &s) const = 0
	// rst:
	// rst:		Printing function used for describing the function in output from algorithms.
	// rst:
	virtual void print(std::ostream &s) const = 0;
	// rst: .. function:: virtual R operator()(Args ...args) const = 0
	// rst: 
	// rst:		The actual function implementation of the function.
	// rst:
	virtual R operator()(Args ...args) const = 0;
};
// rst-class-end:

namespace detail {

template<typename Sig>
struct StdFunctionWrapper {
};

template<typename R, typename ...Args>
struct StdFunctionWrapper<R(Args...)> : public Function<R(Args...)> {

	StdFunctionWrapper(std::string name, std::function<R(Args...) > f) : name(name), f(f) { }

	StdFunctionWrapper(std::function<R(Args...) > f) : name("<C++ lambda>"), f(f) { }

	std::shared_ptr < Function < R(Args...)> > clone() const {
		return std::unique_ptr < Function < R(Args...)> >(new StdFunctionWrapper(*this));
	}

	void print(std::ostream &s) const {
		s << name;
	}

	R operator()(Args ...args) const {
		return f(args...);
	}
private:
	const std::string name;
	std::function < R(Args...) > f;
};

template<typename Sig>
struct ToStdFunctionHelper {
};

template<typename R, typename ...Args>
struct ToStdFunctionHelper<R(Args...)> {

	static std::function < R(Args...) > get(std::shared_ptr<Function<R(Args...)> > fMod) {
		return [fMod] (Args ...args) -> R {
			return (*fMod)(args...);
		};
	}
};

} // namespace detail

template<typename Sig>
std::unique_ptr<Function<Sig> > fromStdFunction(std::function<Sig> f) {
	return std::unique_ptr<Function<Sig> >(new detail::StdFunctionWrapper<Sig>(f));
}

template<typename Sig>
std::unique_ptr<Function<Sig> > fromStdFunction(std::string name, std::function<Sig> f) {
	return std::unique_ptr<Function<Sig> >(new detail::StdFunctionWrapper<Sig>(name, f));
}

template<typename Sig>
std::function<Sig> toStdFunction(std::shared_ptr<Function<Sig> > fMod) {
	return detail::ToStdFunctionHelper<Sig>::get(fMod);
}

} // namespace mod

#endif	/* MOD_FUNCTION_H */