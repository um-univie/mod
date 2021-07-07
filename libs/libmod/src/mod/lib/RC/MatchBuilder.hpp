#ifndef MOD_LIB_RC_MATCHBUILDER_HPP
#define MOD_LIB_RC_MATCHBUILDER_HPP

#include <mod/lib/Rules/Real.hpp>
#include <jla_boost/graph/morphism/finders/CommonSubgraph.hpp>
#include <jla_boost/graph/morphism/models/Vector.hpp>

namespace mod::lib::RC {

struct MatchBuilder {
	explicit MatchBuilder(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, LabelSettings labelSettings);
	lib::Rules::Vertex getSecondFromFirst(lib::Rules::Vertex v) const;
	lib::Rules::Vertex getFirstFromSecond(lib::Rules::Vertex v) const;
	std::size_t size() const;
	bool push(lib::Rules::Vertex vFirst, lib::Rules::Vertex vSecond);
	void pop();
	std::unique_ptr<lib::Rules::Real> compose(bool verbose) const;
	std::vector<std::unique_ptr<lib::Rules::Real>> composeAll(bool maximum, bool verbose) const;
private:
	struct VertexPred {
		bool operator()(lib::Rules::Vertex vSecond, lib::Rules::Vertex vFirst);
	public:
		const lib::Rules::Real &rFirst;
		const lib::Rules::Real &rSecond;
		const LabelSettings labelSettings;
	};
	struct EdgePred {
		bool operator()(lib::Rules::Edge eSecond, lib::Rules::Edge eFirst);
	public:
		const lib::Rules::Real &rFirst;
		const lib::Rules::Real &rSecond;
		const LabelSettings labelSettings;
	};
public:
	const lib::Rules::Real &rFirst;
	const lib::Rules::Real &rSecond;
	const LabelSettings labelSettings;
private:
	// note: the match has rSecond as domain and rFirst as codomain, i.e., <-,
	//       so everything todo with match is somewhat "backwards"
	jla_boost::GraphMorphism::CommonSubgraphEnumerator<false, lib::Rules::SideGraphType, lib::Rules::SideGraphType,
			EdgePred, VertexPred> match;
};

} // namespace mod::lib::RC

#endif // MOD_LIB_RC_MATCHBUILDER_HPP