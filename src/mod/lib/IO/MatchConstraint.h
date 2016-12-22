#ifndef MOD_LIB_IO_MATCHCONSTRAINT_H
#define MOD_LIB_IO_MATCHCONSTRAINT_H

#include <mod/Error.h>
#include <mod/lib/GraphMorphism/MatchConstraint.h>

namespace mod {
namespace lib {
namespace GraphMorphism {
namespace MatchConstraint {
std::ostream &operator<<(std::ostream &s, Operator op);
} // namespace MatchConstraint
} // namespace GraphMorphism
namespace IO {
namespace MatchConstraint {
namespace Write {

template<typename Graph>
struct PrintVisitor : lib::GraphMorphism::MatchConstraint::Visitor<Graph, void> {

	PrintVisitor(std::ostream &s) : s(s) { }

	virtual void operator()(const lib::GraphMorphism::MatchConstraint::VertexAdjacency<Graph> &c) const override {
		s << "|{ e in outEdges(" << c.vConstrained << ") ";
		if(!c.vertexLabels.empty() || !c.edgeLabels.empty()) s << "| ";
		if(!c.vertexLabels.empty()) {
			s << "label(target(e)) in {";
			for(const std::string &str : c.vertexLabels) s << " '" << str << "'";
			s << " } ";
		}
		if(!c.vertexLabels.empty() && !c.edgeLabels.empty()) s << "AND ";
		if(!c.edgeLabels.empty()) {
			s << "label(e) in {";
			for(const std::string &str : c.edgeLabels) s << " '" << str << "'";
			s << " } ";
		}
		s << "}| " << c.op << " " << c.count;
	}

	virtual void operator()(const lib::GraphMorphism::MatchConstraint::ShortestPath<Graph> &c) const override {
		s << "shortestPath(" << c.vSrc << ", " << c.vTar << ") " << c.op << " " << c.length;
	}
private:
	std::ostream &s;
};

template<typename Graph>
struct GMLPrintVisitor : lib::GraphMorphism::MatchConstraint::Visitor<Graph, void> {

	GMLPrintVisitor(std::ostream &s, const Graph &g, std::string prefix) : s(s), g(g), prefix(std::move(prefix)) { }

	virtual void operator()(const lib::GraphMorphism::MatchConstraint::VertexAdjacency<Graph> &c) const override {
		s << prefix << "constrainAdj [\n";
		s << prefix << "	id " << get(boost::vertex_index_t(), g, c.vConstrained) << "\n";
		s << prefix << "	op \"" << c.op << "\"\n";
		s << prefix << "	count " << c.count << "\n";
		s << prefix << "	nodeLabels [";
		for(const auto &str : c.vertexLabels) s << " label \"" << str << "\"";
		s << " ]\n";
		s << prefix << "	edgeLabels [";
		for(const auto &str : c.edgeLabels) s << " label \"" << str << "\"";
		s << " ]\n";
		s << prefix << "]\n";
	}

	virtual void operator()(const lib::GraphMorphism::MatchConstraint::ShortestPath<Graph> &c) const override {
		s << prefix << "constrainShortestPath [\n";
		s << prefix << "	source " << get(boost::vertex_index_t(), g, c.vSrc)
				<< " target " << get(boost::vertex_index_t(), g, c.vTar) << "\n";
		s << prefix << "	op \"" << c.op << "\" length " << c.length << "\n";
		s << prefix << "]\n";
	}
private:
	std::ostream &s;
	const Graph &g;
	std::string prefix;
};

template<typename Graph>
auto makeGMLPrintVisitor(std::ostream &s, const Graph &g, std::string prefix) {
	return GMLPrintVisitor<Graph>(s, g, prefix);
}

} // namespace Write
} // namespace MatchConstraint
} // namespace IO
namespace GraphMorphism {
namespace MatchConstraint {

inline std::ostream &operator<<(std::ostream &s, Operator op) {
	switch(op) {
	case Operator::EQ: return s << "=";
	case Operator::LT: return s << "<";
	case Operator::GT: return s << ">";
	case Operator::LEQ: return s << "<=";
	case Operator::GEQ: return s << ">=";
	}
	MOD_ABORT;
}

template<typename Graph>
std::ostream &operator<<(std::ostream &s, const Constraint<Graph> &c) {
	lib::IO::MatchConstraint::Write::PrintVisitor<Graph> printer(s);
	c.visit(printer);
	return s;
}

} // namespace MatchConstraint
} // namespace GraphMorphism
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_MATCHCONSTRAINT_H */