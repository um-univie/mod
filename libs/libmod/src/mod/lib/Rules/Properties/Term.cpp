#include "Term.hpp"

#include <mod/Error.hpp>
#include <mod/lib/GraphMorphism/Constraints/AllVisitor.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/Term.hpp>
#include <mod/lib/Rules/Properties/String.hpp>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

namespace mod {
namespace lib {
namespace Rules {

PropTermCore::PropTermCore(const GraphType &core,
		const std::vector<ConstraintPtr> &leftMatchConstraints,
		const std::vector<ConstraintPtr> &rightMatchConstraints,
		const PropStringCore &label, const StringStore &stringStore) : Base(core) {
	// take every unique string and parse it
	std::unordered_map<std::string, std::size_t> labelToAddress;
	lib::Term::RawAppendStore varToAddr;
	const auto handleLabel = [&stringStore, &labelToAddress, &varToAddr, this](const std::string & label) {
		// if there is a * in the label, then we can not just use label caching
		if(label.find('*') == std::string::npos) {
			auto iter = labelToAddress.find(label);
			if(iter != end(labelToAddress)) {
				return iter->second;
			}
		}
		std::stringstream err;
		auto rawTerm = lib::IO::Term::Read::rawTerm(label, stringStore, err);
		if(!rawTerm) {
			parsingError = err.str();
			return std::numeric_limits<std::size_t>::max();
		}
		lib::Term::Address addr = lib::Term::append(machine, *rawTerm, varToAddr);
		labelToAddress[label] = addr.addr;
		return addr.addr;
	};
	vertexState.resize(num_vertices(core));
	for(Vertex v : asRange(vertices(core))) {
		auto vId = get(boost::vertex_index_t(), g, v);
		switch(core[v].membership) {
		case Membership::Left:
			vertexState[vId].left = handleLabel(label.getLeft()[v]);
			break;
		case Membership::Right:
			vertexState[vId].right = handleLabel(label.getRight()[v]);
			break;
		case Membership::Context:
			if(label.getLeft()[v] == label.getRight()[v]) {
				// no label-change, so use the same for both (e.g., "*" -> "*" should be the _same_ variable)
				vertexState[vId].left = vertexState[vId].right = handleLabel(label.getLeft()[v]);
			} else {
				vertexState[vId].left = handleLabel(label.getLeft()[v]);
				vertexState[vId].right = handleLabel(label.getRight()[v]);
			}
			break;
		}
	}
	edgeState.resize(num_edges(core));
	for(Edge e : asRange(edges(core))) {
		auto eId = get(boost::edge_index_t(), g, e);
		switch(core[e].membership) {
		case Membership::Left:
			edgeState[eId].left = handleLabel(label.getLeft()[e]);
			break;
		case Membership::Right:
			edgeState[eId].right = handleLabel(label.getRight()[e]);
			break;
		case Membership::Context:
			if(label.getLeft()[e] == label.getRight()[e]) {
				// no label-change, so use the same for both (e.g., "*" -> "*" should be the _same_ variable)
				auto convertedLabel = handleLabel(label.getLeft()[e]);
				edgeState[eId].left = convertedLabel;
				edgeState[eId].right = convertedLabel;
			} else {
				edgeState[eId].left = handleLabel(label.getLeft()[e]);
				edgeState[eId].right = handleLabel(label.getRight()[e]);
			}
			break;
		}
	}

	using HandlerType = decltype(handleLabel);

	struct Visitor : lib::GraphMorphism::Constraints::AllVisitorNonConst<SideGraphType> {

		Visitor(HandlerType &handleLabel) : handleLabel(handleLabel) { }

		virtual void operator()(lib::GraphMorphism::Constraints::VertexAdjacency<SideGraphType> &c) override {
			assert(c.vertexTerms.size() == 0);
			assert(c.edgeTerms.size() == 0);
			for(const std::string &s : c.vertexLabels)
				c.vertexTerms.insert(handleLabel(s));
			for(const std::string &s : c.edgeLabels)
				c.edgeTerms.insert(handleLabel(s));
		}

		virtual void operator()(lib::GraphMorphism::Constraints::ShortestPath<SideGraphType> &c) override { }
	private:
		HandlerType handleLabel;
	};

	const auto handleConstraints = [&](const auto &cs) {
		Visitor vis(handleLabel);
		for(auto &c : cs) c->accept(vis);
	};
	handleConstraints(leftMatchConstraints);
	handleConstraints(rightMatchConstraints);
	verify(&this->g);
}

PropTermCore::PropTermCore(const GraphType &core, lib::Term::Wam machine) : Base(core), machine(std::move(machine)) { }

bool isValid(const PropTermCore &core) {
	return !core.parsingError.is_initialized();
}

const std::string &PropTermCore::getParsingError() const {
	assert(!isValid(*this));
	return *parsingError;
}

lib::Term::Wam &getMachine(PropTermCore &core) {
	return core.machine;
}

const lib::Term::Wam &getMachine(const PropTermCore &core) {
	return core.machine;
}

} // namespace Rules
} // namespace lib
} // namespace mod