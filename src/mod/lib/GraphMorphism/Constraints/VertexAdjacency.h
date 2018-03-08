#ifndef MOD_LIB_GRAPHMORPHISM_ADJACENCY_H
#define MOD_LIB_GRAPHMORPHISM_ADJACENCY_H

#include <mod/lib/GraphMorphism/LabelledMorphism.h>
#include <mod/lib/GraphMorphism/TermVertexMap.h>
#include <mod/lib/GraphMorphism/Constraints/Constraint.h>
#include <mod/lib/Term/WAM.h>

#include <jla_boost/graph/morphism/VertexMap.hpp>
#include <jla_boost/graph/morphism/models/PropertyVertexMap.hpp>

namespace mod {
namespace lib {
namespace GraphMorphism {
namespace Constraints {

template<typename Graph>
struct VertexAdjacency : Constraint<Graph> {
	MOD_VISITABLE();
	using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
public:

	VertexAdjacency(Vertex vConstrained, Operator op, int count)
	: vConstrained(vConstrained), op(op), count(count), vertexLabels(1), edgeLabels(1) { }

	virtual std::unique_ptr<Constraint<Graph> > clone() const override {
		auto c = std::make_unique<VertexAdjacency>(vConstrained, op, count);
		c->vertexLabels = vertexLabels;
		c->edgeLabels = edgeLabels;
		return c;
	}

	virtual std::string name() const override {
		return "VertexAdjacency";
	}

	virtual bool supportsTerm() const override {
		return false;
	}
private:

	template<typename Visitor, typename LabelledGraphCodom, typename VertexMap>
	int matchesImpl(Visitor &vis, const Graph &gDom, const LabelledGraphCodom &lgCodom, VertexMap &m, const LabelSettings ls, std::false_type) const {
		assert(ls.type == LabelType::String); // otherwise someone forgot to add the TermData prop
		using GraphCodom = typename LabelledGraphTraits<LabelledGraphCodom>::GraphType;
		const GraphCodom &gCodom = get_graph(lgCodom);
		const auto vCodom = get(m, gDom, gCodom, vConstrained);
		int count = 0;
		const auto &string = get_string(lgCodom);
		for(const auto eOutCodom : asRange(out_edges(vCodom, gCodom))) {
			if(!edgeLabels.empty()) {
				const auto iter = edgeLabels.find(string[eOutCodom]);
				if(iter == edgeLabels.end()) continue;
			}
			if(!vertexLabels.empty()) {
				const auto iter = vertexLabels.find(string[target(eOutCodom, gCodom)]);
				if(iter == vertexLabels.end()) continue;
			}
			++count;
		}
		return count;
	}

	template<typename Visitor, typename LabelledGraphCodom, typename VertexMap>
	int matchesImpl(Visitor &vis, const Graph &gDom, const LabelledGraphCodom &lgCodom, VertexMap &m, const LabelSettings ls, std::true_type) const {
		assert(ls.type == LabelType::Term); // otherwise someone did something very strange
		using GraphCodom = typename LabelledGraphTraits<LabelledGraphCodom>::GraphType;
		const GraphCodom &gCodom = get_graph(lgCodom);
		const auto vCodom = get(m, gDom, gCodom, vConstrained);
		int count = 0;
		const auto &term = get_term(lgCodom);
		lib::Term::Wam &machine = get_prop(TermDataT(), m).machine;
		const auto countPerVertexTerms = [&](const auto h) {
			if(vertexTerms.empty()) {
				++count;
			} else {
				for(const auto t : vertexTerms) {
					lib::Term::MGU mgu = machine.unifyHeapTemp(h, t);
					if(mgu.status == lib::Term::MGU::Status::Exists) {
						++count;
					}
					machine.revert(mgu);
				}
			}
		};
		const auto countPerEdgeTerms = [&](const auto hEdge, const auto hVertex) {
			if(edgeTerms.empty()) {
				countPerVertexTerms(hVertex);
			} else {
				for(const auto t : edgeTerms) {
					lib::Term::MGU mgu = machine.unifyHeapTemp(hEdge, t);
					if(mgu.status == lib::Term::MGU::Status::Exists) {
						countPerVertexTerms(hVertex);
					}
					machine.revert(mgu);
				}
			}
		};
		for(const auto eOutCodom : asRange(out_edges(vCodom, gCodom))) {
			const auto vOut = target(eOutCodom, gCodom);
			countPerEdgeTerms(term[eOutCodom], term[vOut]);
		}
		return count;
	}
public:

	template<typename Visitor, typename LabelledGraphCodom, typename VertexMap>
	bool matches(Visitor &vis, const Graph &gDom, const LabelledGraphCodom &lgCodom, VertexMap &m, const LabelSettings ls) const {
		using GraphCodom = typename LabelledGraphTraits<LabelledGraphCodom>::GraphType;
		static_assert(std::is_same<Graph, typename jla_boost::GraphMorphism::VertexMapTraits<VertexMap>::GraphDom>::value, "");
		static_assert(std::is_same<GraphCodom, typename jla_boost::GraphMorphism::VertexMapTraits<VertexMap>::GraphCodom>::value, "");
		{ // verify
#ifndef NDEBUG
			const auto vs = vertices(gDom);
			assert(std::find(vs.first, vs.second, vConstrained) != vs.second);
#endif
		}
		const GraphCodom &gCodom = get_graph(lgCodom);
		const auto vCodom = get(m, gDom, gCodom, vConstrained);
		if(vCodom == boost::graph_traits<GraphCodom>::null_vertex())
			return true; // vertex is not even mapped by the vertex map

		using HasTerm = GraphMorphism::HasTermData<VertexMap>;
		const int count = matchesImpl(vis, gDom, lgCodom, m, ls, HasTerm());
		switch(op) {
		case Operator::EQ: return count == this->count;
		case Operator::LT: return count < this->count;
		case Operator::GT: return count > this->count;
		case Operator::LEQ: return count <= this->count;
		case Operator::GEQ: return count >= this->count;
		}
		assert(false);
		std::abort();
	}
public:
	Vertex vConstrained;
	Operator op;
	int count;
	std::unordered_set<std::string> vertexLabels, edgeLabels;
	std::unordered_set<std::size_t> vertexTerms, edgeTerms;
};

} // namespace Constraints
} // namespace GraphMorphism
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPHMORPHISM_ADJACENCY_H */