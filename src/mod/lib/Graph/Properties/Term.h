#ifndef MOD_LIB_GRAPH_PROP_TERM_H
#define	MOD_LIB_GRAPH_PROP_TERM_H

#include <mod/lib/Graph/Properties/Property.h>
#include <mod/lib/Term/WAM.h>

#include <boost/optional.hpp>

#include <string>

namespace mod {
namespace lib {
namespace Graph {
struct PropString;

struct PropTerm : Prop<PropTerm, std::size_t, std::size_t> {
	using Base = Prop<PropTerm, std::size_t, std::size_t>;
public:
	PropTerm(const GraphType &g, const PropString &pString, const StringStore &stringStore); // parse-construct
	const std::string &getParsingError() const; // requires !isValid
	friend bool isValid(const PropTerm &p);
	friend const lib::Term::Wam &getMachine(const PropTerm &p);
private:
	boost::optional<std::string> parsingError;
	lib::Term::Wam machine;
};

} // namespace Graph
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_GRAPH_PROP_TERM_H */
