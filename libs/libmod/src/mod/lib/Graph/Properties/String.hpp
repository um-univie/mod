#ifndef MOD_LIB_GRAPH_PROP_LABEL_H
#define MOD_LIB_GRAPH_PROP_LABEL_H

#include <mod/lib/Graph/Properties/Property.hpp>

namespace mod {
namespace lib {
namespace Graph {

struct PropString : Prop<PropString, std::string, std::string> {
	using Base = Prop<PropString, std::string, std::string>;
public:

	explicit PropString(const GraphType &g) : Base(g) {
		Base::verify(&g);
	}

	PropString(const PropString &other, const GraphType &g) : Base(other, g) {
		Base::verify(&g);
	}
};

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_PROP_LABEL_H */
