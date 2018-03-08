#ifndef MOD_LIB_RULE_PROP_STEREO_H
#define MOD_LIB_RULE_PROP_STEREO_H

#include <mod/lib/Stereo/Configuration/Configuration.h>
#include <mod/lib/Stereo/EdgeCategory.h>
#include <mod/lib/Rules/Properties/Molecule.h>

#include <tuple>

namespace mod {
namespace lib {
namespace Rules {

struct PropStereoCore : private PropCore<PropStereoCore, GraphType, std::unique_ptr<const lib::Stereo::Configuration>, lib::Stereo::EdgeCategory> {
	// read-only of data
	using Base = PropCore<PropStereoCore, GraphType, std::unique_ptr<const lib::Stereo::Configuration>, lib::Stereo::EdgeCategory>;
	using Base::LeftVertexType;
	using Base::LeftEdgeType;
	using Base::RightVertexType;
	using Base::RightEdgeType;
	using Base::LeftType;
	using Base::RightType;

	struct ValueTypeVertex {
		boost::optional<const LeftVertexType&> left;
		boost::optional<const RightVertexType&> right;
		bool inContext;
	};

	struct ValueTypeEdge {
		boost::optional<lib::Stereo::EdgeCategory> left;
		boost::optional<lib::Stereo::EdgeCategory> right;
		bool inContext;
	};
public:

	template<typename InferenceLeft, typename InferenceRight, typename VertexInContext, typename EdgeInContext>
	PropStereoCore(const GraphType &g, InferenceLeft &&leftInference, InferenceRight &&rightInference,
			VertexInContext vCallback, EdgeInContext eCallback) : Base(g) {
		// The eCallback is only responsible for the information about being in context.
		// This function will ensure that it's valid to put in context as well.
		// However, the vCallback is also responsible for ensuring that it is valid to put it in context.
		// This function only checks what is easy, i.e., whether the vertex and its incident edges are in context.
		vertexState.reserve(num_vertices(g));
		vertexInContext.reserve(num_vertices(g));
		for(const auto v : asRange(vertices(g))) {
			assert(get(boost::vertex_index_t(), g, v) == vertexState.size());
			std::unique_ptr<const lib::Stereo::Configuration> l, r;
			if(g[v].membership != Membership::Right) l = leftInference.extractConfiguration(v);
			if(g[v].membership != Membership::Left) r = rightInference.extractConfiguration(v);
			{ // verify
				const auto verify = [&g, &v](const lib::Stereo::Configuration &conf, const auto m) {
					const auto oe = out_edges(v, g);
					const auto d = std::count_if(oe.first, oe.second, [&g, m](const auto &e) {
						return g[e].membership != m;
					});
					int dConf = 0;
					for(const auto &emb : conf) {
						if(emb.type != lib::Stereo::EmbeddingEdge::Type::Edge) {
							assert(emb.offset >= d);
						} else { // edge
							++dConf;
							assert(emb.offset < d);
						};
					}
					assert(dConf == d);
				};
				if(l) verify(*l, Membership::Right);
				if(r) verify(*r, Membership::Left);
				assert(bool(l) || bool(r));
			} // end verify
			vertexState.emplace_back(std::move(l), std::move(r));
			const bool inContext = [&]() {
				if(g[v].membership != Membership::Context) return false;
				if(!vCallback(v)) return false;
				for(const auto &eOut : asRange(out_edges(v, g))) {
					if(g[eOut].membership != Membership::Context) return false;
				}
				assert(bool(vertexState.back().left));
				assert(bool(vertexState.back().right));
				return true;
			}();
			vertexInContext.push_back(inContext);
		}
		edgeState.reserve(num_edges(g));
		edgeInContext.reserve(num_edges(g));
		for(const auto e : asRange(edges(g))) {
			assert(get(boost::edge_index_t(), g, e) == edgeState.size());
			lib::Stereo::EdgeCategory lCat, rCat;
			if(g[e].membership != Membership::Right) lCat = leftInference.getEdgeCategory(e);
			if(g[e].membership != Membership::Left) rCat = rightInference.getEdgeCategory(e);
			edgeState.emplace_back(lCat, rCat);
			const auto m = g[e].membership;
			const bool inContext =
					m == Membership::Context
					&& lCat == rCat
					&& eCallback(e);
			edgeInContext.push_back(inContext);
		}
		verify(&g);
	}

	ValueTypeVertex operator[](Vertex v) const {
		assert(v != boost::graph_traits<GraphType>::null_vertex());
		ValueTypeVertex res;
		if(g[v].membership != Membership::Right) res.left = getLeft()[v];
		if(g[v].membership != Membership::Left) res.right = getRight()[v];
		res.inContext = inContext(v);
		return res;
	}

	ValueTypeEdge operator[](Edge e) const {
		ValueTypeEdge res;
		if(g[e].membership != Membership::Right) res.left = getLeft()[e];
		if(g[e].membership != Membership::Left) res.right = getRight()[e];
		res.inContext = inContext(e);
		return res;
	}
public:
	using Base::verify;
	using Base::getLeft;
	using Base::getRight;
	using Base::print;

	struct Handler {

		template<typename VEProp, typename LabGraphDom, typename LabGraphCodom, typename F, typename ...Args>
		static auto fmap2(const VEProp &l, const VEProp &r, const LabGraphDom &gDom, const LabGraphCodom &gCodom, F &&f, Args&&... args) {
			assert(l.left.is_initialized() || l.right.is_initialized());
			if(l.inContext) {
				assert(l.left.is_initialized());
				assert(l.right.is_initialized());
			}
			assert(r.left.is_initialized() || r.right.is_initialized());
			if(r.inContext) {
				assert(r.left.is_initialized());
				assert(r.right.is_initialized());
			}
			assert(l.left.is_initialized() == r.left.is_initialized());
			assert(l.right.is_initialized() == r.right.is_initialized());
			using Left = decltype(f(*l.left, *r.left, get_labelled_left(gDom), get_labelled_left(gCodom), args...));
			using Right = decltype(f(*l.right, *r.right, get_labelled_right(gDom), get_labelled_right(gCodom), args...));
			using InContext = decltype(f(l.inContext, r.inContext, get_labelled_left(gDom), get_labelled_left(gCodom), args...));
			return std::tuple<boost::optional<Left>, boost::optional<Right>, InContext>(
					l.left.is_initialized() ? f(*l.left, *r.left, get_labelled_left(gDom), get_labelled_left(gCodom), args...) : boost::optional<Left>(),
					l.right.is_initialized() ? f(*l.right, *r.right, get_labelled_right(gDom), get_labelled_right(gCodom), args...) : boost::optional<Right>(),
					f(l.inContext, r.inContext, args...)
					)
					;
		}

		template<typename Op, typename Val>
		static auto reduce(Op &&op, Val &&val) {
			const auto &left = std::get<0>(val);
			const auto &right = std::get<1>(val);
			const auto &inContext = std::get<2>(val);
			assert(left.is_initialized() || right.is_initialized());
			if(!left.is_initialized()) return op(*right, inContext);
			if(!right.is_initialized()) return op(*left, inContext);
			return op(op(*left, *right), inContext);
		}
	};
public:

	bool inContext(Vertex v) const {
		return vertexInContext[get(boost::vertex_index_t(), g, v)];
	}

	bool inContext(Edge e) const {
		return edgeInContext[get(boost::edge_index_t(), g, e)];
	}
private:
	std::vector<bool> vertexInContext, edgeInContext;
};

template<typename VertexOrEdge>
auto get(const PropStereoCore &p, const VertexOrEdge &ve) -> decltype(p[ve]) {
	return p[ve];
}

} // namespace Rules
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RULE_PROP_STEREO_H */