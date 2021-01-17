#include "String.hpp"

#include <mod/lib/GraphMorphism/Constraints/AllVisitor.hpp>
#include <mod/lib/IO/Term.hpp>
#include <mod/lib/Rules/Properties/Term.hpp>
#include <mod/lib/StringStore.hpp>

namespace mod {
namespace lib {
namespace Rules {

PropStringCore::PropStringCore(const GraphType &g) : PropCore(g) {
	verify(&g);
}

PropStringCore::PropStringCore(const GraphType &g,
                               const std::vector<ConstraintPtr> &leftMatchConstraints,
                               const std::vector<ConstraintPtr> &rightMatchConstraints,
                               const PropTermCore &term, const StringStore &strings) : PropCore(g) {
	std::stringstream ss;
	const auto termToString = [&ss, &term, &strings](std::size_t addr) -> std::string {
		ss.str(std::string());

		lib::IO::Term::Write::term(getMachine(term), {
				lib::Term::AddressType::Heap, addr
		}, strings, ss);
		return ss.str();
	};
	vertexState.resize(num_vertices(g));
	for(auto v : asRange(vertices(g))) {
		auto vId = get(boost::vertex_index_t(), g, v);
		switch(g[v].membership) {
		case Membership::Left:
			vertexState[vId].left = termToString(term.getLeft()[v]);
			break;
		case Membership::Right:
			vertexState[vId].right = termToString(term.getRight()[v]);
			break;
		case Membership::Context:
			vertexState[vId].left = termToString(term.getLeft()[v]);
			vertexState[vId].right = termToString(term.getRight()[v]);
			break;
		}
	}
	edgeState.resize(num_edges(g));
	for(auto e : asRange(edges(g))) {
		auto eId = get(boost::edge_index_t(), g, e);
		switch(g[e].membership) {
		case Membership::Left:
			edgeState[eId].left = termToString(term.getLeft()[e]);
			break;
		case Membership::Right:
			edgeState[eId].right = termToString(term.getRight()[e]);
			break;
		case Membership::Context:
			edgeState[eId].left = termToString(term.getLeft()[e]);
			edgeState[eId].right = termToString(term.getRight()[e]);
			break;
		}
	}

	using HandlerType = decltype(termToString);

	struct Visitor : lib::GraphMorphism::Constraints::AllVisitorNonConst<SideGraphType> {
		Visitor(HandlerType &termToString) : termToString(termToString) {}

		virtual void operator()(lib::GraphMorphism::Constraints::VertexAdjacency<SideGraphType> &c) override {
			assert(c.vertexLabels.size() == 0);
			assert(c.edgeLabels.size() == 0);
			for(const auto &t : c.vertexTerms)
				c.vertexLabels.insert(termToString(t));
			for(const auto &t : c.edgeTerms)
				c.edgeLabels.insert(termToString(t));
		}

		virtual void operator()(lib::GraphMorphism::Constraints::ShortestPath<SideGraphType> &c) override {}
	private:
		HandlerType termToString;
	};

	const auto handleConstraints = [&](const auto &cs) {
		Visitor vis(termToString);
		for(auto &c : cs) c->accept(vis);
	};
	handleConstraints(leftMatchConstraints);
	handleConstraints(rightMatchConstraints);
}

} // namespace Rules
} // namespace lib
} // namespace mod