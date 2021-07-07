#ifndef MOD_LIB_IO_RESULT_HPP
#define MOD_LIB_IO_RESULT_HPP

#include <optional>
#include <string>
#include <vector>

namespace mod::lib::IO {

struct Warnings {
	Warnings() = default;
	Warnings(const Warnings &) = delete;
	Warnings &operator=(const Warnings &) = delete;
	Warnings(Warnings &&) = default;
	Warnings &operator=(Warnings &&) = default;
public:
	// will be prefixed with "WARNING: " when printed
	void add(std::string msg) { add(std::move(msg), true); }
	void add(std::string msg, bool print) { warnings.emplace_back(std::move(msg), print); }
	void addFrom(Warnings &&other, bool print) {
		warnings.reserve(warnings.size() + other.warnings.size());
		for(auto &[msg, oldPrint] : other.warnings)
			warnings.emplace_back(std::move(msg), print);
	}
public:
	friend std::ostream &operator<<(std::ostream &s, const Warnings &ws);
public:
	std::vector<std::pair<std::string, bool>> extractWarnings() { return std::move(warnings); }
private:
	std::vector<std::pair<std::string, bool>> warnings; // msg, print
};

template<typename T = void>
struct Result;

template<>
struct [[nodiscard]] Result<void> {
	Result() = default;
	Result(const Result &) = delete;
	Result &operator=(const Result &) = delete;
	Result(Result &&) = default;
	Result &operator=(Result &&) = default;
	explicit operator bool() const { return !error.has_value(); }
public:
	void setError(std::string msg) { error = std::move(msg); }
public:
	std::string extractError() { return std::move(*error); }
public:
	template<typename E>
	void throwIfError() {
		if(error) throw E(std::move(*error));
	}
public:
	static Result Error(std::string msg) {
		Result r;
		r.setError(std::move(msg));
		return r;
	}
protected:
	std::optional<std::string> error;
};

template<typename T>
struct [[nodiscard]] Result : Result<void> {
	Result(T value) : value(std::move(value)) {}
	// TODO: change to by-value when C++20/P1825R0 is available
	Result(Result<void> &&other) : Result<void>(std::move(other)) {}
	T &operator*() { return *value; }
private:
	Result() = default;
private:
	std::optional<T> value;
public:
	static Result Error(std::string msg) {
		return Result<>::Error(std::move(msg));
	}
};

} // namespace mod::lib::IO

#endif // MOD_LIB_IO_RESULT_HPP
