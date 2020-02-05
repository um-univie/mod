#include "IO.hpp"

#include <mod/Error.hpp>

#include <boost/lexical_cast.hpp>

#include <cmath>
#include <fstream>
#include <iostream>

namespace mod {
namespace lib {
namespace IO {
namespace {

const std::string prefix = "out/";

struct PostStream {

	PostStream() {
		std::string postFile = prefix + "post.sh";
		s.open(postFile.c_str());
		enabled = s.is_open();
	}

	~PostStream() {}

	std::ofstream &getStream() {
		if(enabled) return s;
		std::cerr << "ERROR: can not write to '" << prefix + "post.sh'" << std::endl;
		std::cerr << "Does '" << prefix << "' exist?" << std::endl;
		std::exit(1);
	}

private:
	bool enabled;
	std::ofstream s;
};

PostStream postStream;

} // namespace

std::string getUniqueFilePrefix() {
	static int count = 0;

	std::string strCount;
	int k = 4;
	int diff = std::max(0., k - log10(count + 1) - 1);
	if(count == 0) diff--;
	for(int i = 0; i < diff; i++) strCount += '0';
	strCount += boost::lexical_cast<std::string>(count);
	strCount += '_';
	count++;
	return prefix + strCount;
}

std::string escapeForLatex(const std::string &str) {
	std::string res;
	res.reserve(str.size());
	for(char c : str) {
		switch(c) {
		case '#':
		case '_':
		case '{':
		case '}': res += "\\";
			res += c;
			break;
		case '*': res += "\\ensuremath{*}";
			break;
		default: res += c;
		}
	}
	return res;
}

std::string asLatexMath(const std::string &str) {
	std::string res = "$\\mathrm{";
	for(char c : str) {
		switch(c) {
		case ' ': res += "\\";
			res += c;
			break;
		default: res += c;
		}
	}
	res += "}$";
	return res;
}

std::ostream &nullStream() {
	static std::ofstream s("/dev/null");
	return s;
}

std::ostream &post() {
	return postStream.getStream();
}

std::ostream &log() {
	std::cout.flush();
	bool log = true;
	return log ? std::cout : nullStream();
}

std::ostream &Logger::indent() const {
	assert(indentLevel >= 0);
	return s << std::string(indentLevel * 2, ' ');
}

std::ostream &Logger::sep(char c) const {
	assert(indentLevel >= 0);
	return s << std::string(std::max(10, 80 - indentLevel * 2), c) << '\n';
}

} // namespace IO
} // namespace lib
} // namespace mod
