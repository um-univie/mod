#ifndef MOD_LIB_LABELLEDUNIONGRAPH_HPP
#define MOD_LIB_LABELLEDUNIONGRAPH_HPP

#include <mod/lib/LabelledGraph.hpp>

#include <jla_boost/graph/UnionGraph.hpp>

namespace mod::lib {
namespace detail {

template<typename LGraph>
struct UnionPropBase {
	using InnerGraph = typename LabelledGraphTraits<LGraph>::GraphType;
public:
	using GraphType = jla_boost::union_graph<InnerGraph>;
	using Vertex = typename boost::graph_traits<GraphType>::vertex_descriptor;
	using Edge = typename boost::graph_traits<GraphType>::edge_descriptor;
public:
	UnionPropBase(const std::vector<const LGraph *> &lgs) : lgs(lgs) {}
protected:
	const std::vector<const LGraph *> &lgs;
};

#define MOD_MAKE_UNION_PROP(Name, Func)                                         \
   template<typename LGraph>                                                    \
   struct UnionProp ## Name : UnionPropBase<LGraph> {                           \
      using Base = UnionPropBase<LGraph>;                                       \
   public:                                                                      \
      using Base::Base;                                                         \
                                                                                \
      decltype(auto) operator[](const typename Base::Vertex v) const {          \
         return get_ ## Func(*this->lgs[v.gIdx])[v.v];                          \
      }                                                                         \
                                                                                \
      decltype(auto) operator[](const typename  Base::Edge e) const {           \
         return get_ ## Func(*this->lgs[e.gIdx])[e.e];                          \
      }                                                                         \
                                                                                \
      template<typename K>                                                      \
      friend decltype(auto) get(const UnionProp ## Name &up, const K &k) {      \
         return up[k];                                                          \
      }                                                                         \
   };
MOD_MAKE_UNION_PROP(String, string);
MOD_MAKE_UNION_PROP(Term, term);
MOD_MAKE_UNION_PROP(Stereo, stereo);
MOD_MAKE_UNION_PROP(Molecule, molecule);
#undef MOD_MAKE_UNION_PROP

} // namespace detail

template<typename LGraph>
struct LabelledUnionGraph {
	using InnerGraph = typename LabelledGraphTraits<LGraph>::GraphType;
public:
	using GraphType = jla_boost::union_graph<InnerGraph>;
	using PropString = detail::UnionPropString<LGraph>;
	using PropTerm = detail::UnionPropTerm<LGraph>;
	using PropStereo = detail::UnionPropStereo<LGraph>;
	using PropMolecule = detail::UnionPropMolecule<LGraph>;
public:
	void push_back(const LGraph *lg) {
		lgs.push_back(lg);
		ug.push_back(&get_graph(*lg));
	}
public:
	friend const GraphType &get_graph(const LabelledUnionGraph &lug) {
		return lug.ug;
	}

	friend PropString get_string(const LabelledUnionGraph &lug) {
		return PropString(lug.lgs);
	}

	friend PropTerm get_term(const LabelledUnionGraph &lug) {
		return PropTerm(lug.lgs);
	}

	friend PropStereo get_stereo(const LabelledUnionGraph &lug) {
		return PropStereo(lug.lgs);
	}

	friend bool has_stereo(const LabelledUnionGraph &lug) {
		return std::any_of(lug.lgs.begin(), lug.lgs.end(), [](const LGraph *lg) {
			return has_stereo(*lg);
		});
	}

	friend PropMolecule get_molecule(const LabelledUnionGraph &lug) {
		return PropMolecule(lug.lgs);
	}
private:
	std::vector<const LGraph *> lgs;
	GraphType ug;
};

} // namesapce mod::lib

#endif // MOD_LIB_LABELLEDUNIONGRAPH_HPP