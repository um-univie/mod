#ifndef MOD_LIB_IO_MORPHISMCONSTRAINTS_H
#define MOD_LIB_IO_MORPHISMCONSTRAINTS_H

#include <mod/Error.hpp>
#include <mod/lib/GraphMorphism/Constraints/AllVisitor.hpp>
#include <mod/lib/IO/IO.hpp>

namespace mod {
namespace lib {
namespace IO {
namespace MatchConstraint {
namespace Write {

template<typename Graph>
struct TexPrintVisitor : lib::GraphMorphism::Constraints::AllVisitor<Graph> {

	TexPrintVisitor(std::ostream &s, const Graph &g) : s(s), g(g) { }

	void printOp(lib::GraphMorphism::Constraints::Operator op) {
		using lib::GraphMorphism::Constraints::Operator;
		switch(op) {
		case Operator::EQ: s << "=";
			break;
		case Operator::LT: s << "<";
			break;
		case Operator::GT: s << ">";
			break;
		case Operator::LEQ: s << "\\leq";
			break;
		case Operator::GEQ: s << "\\geq";
			break;
		}
	}

	virtual void operator()(const lib::GraphMorphism::Constraints::VertexAdjacency<Graph> &c) override {
		s << "\\begin{align*}\n";
		s << "&|\\{e \\in \\mathrm{outEdges}(" << get(boost::vertex_index_t(), g, c.vConstrained) << ") ";
		if(!c.vertexLabels.empty() || !c.edgeLabels.empty()) s << " \\mid \\\\\n";
		if(!c.vertexLabels.empty()) {
			s << "&\\quad \\mathrm{label}(\\mathrm{target}(e)) \\in \\{";
			bool first = true;
			for(const std::string &str : c.vertexLabels) {
				if(!first) s << ", ";
				first = false;
				s << " \\text{`\\texttt{" << escapeForLatex(str) << "}'}";
			}
			s << " \\}\\\\\n";
		}
		if(!c.vertexLabels.empty() && !c.edgeLabels.empty()) s << "&\\quad \\wedge\\\\\n";
		if(!c.edgeLabels.empty()) {
			s << "&\\quad \\mathrm{label}(e) \\in \\{";
			bool first = true;
			for(const std::string &str : c.edgeLabels) {
				if(!first) s << ", ";
				first = false;
				s << " \\text{`\\texttt{" << escapeForLatex(str) << "}'}";
			}
			s << " \\}\\\\\n";
		}
		if(!c.vertexLabels.empty() || !c.edgeLabels.empty()) s << "&";
		s << "\\}| ";
		printOp(c.op);
		s << " " << c.count << "\n\\end{align*}\n";
	}

	virtual void operator()(const lib::GraphMorphism::Constraints::ShortestPath<Graph> &c) override {
		s << "$\\mathrm{shortestPath}(" << get(boost::vertex_index_t(), g, c.vSrc) << ", " << get(boost::vertex_index_t(), g, c.vTar) << ") ";
		printOp(c.op);
		s << " " << c.length << "$\n";
	}
private:
	std::ostream &s;
	const Graph &g;
};

template<typename Graph>
auto makeTexPrintVisitor(std::ostream &s, const Graph &g) {
	return TexPrintVisitor<Graph>(s, g);
}

template<typename Graph>
struct GMLPrintVisitor : lib::GraphMorphism::Constraints::AllVisitor<Graph> {

	GMLPrintVisitor(std::ostream &s, const Graph &g, std::string prefix) : s(s), g(g), prefix(std::move(prefix)) { }

	void printOp(lib::GraphMorphism::Constraints::Operator op) {
		using lib::GraphMorphism::Constraints::Operator;
		switch(op) {
		case Operator::EQ: s << "=";
			break;
		case Operator::LT: s << "<";
			break;
		case Operator::GT: s << ">";
			break;
		case Operator::LEQ: s << "<=";
			break;
		case Operator::GEQ: s << ">=";
			break;
		}
	}

	virtual void operator()(const lib::GraphMorphism::Constraints::VertexAdjacency<Graph> &c) {
		s << prefix << "constrainAdj [\n";
		s << prefix << "	id " << get(boost::vertex_index_t(), g, c.vConstrained) << "\n";
		s << prefix << "	op \"";
		printOp(c.op);
		s << "\"\n";
		s << prefix << "	count " << c.count << "\n";
		s << prefix << "	nodeLabels [";
		for(const auto &str : c.vertexLabels) s << " label \"" << str << "\"";
		s << " ]\n";
		s << prefix << "	edgeLabels [";
		for(const auto &str : c.edgeLabels) s << " label \"" << str << "\"";
		s << " ]\n";
		s << prefix << "]\n";
	}

	virtual void operator()(const lib::GraphMorphism::Constraints::ShortestPath<Graph> &c) {
		s << prefix << "constrainShortestPath [\n";
		s << prefix << "	source " << get(boost::vertex_index_t(), g, c.vSrc)
				<< " target " << get(boost::vertex_index_t(), g, c.vTar) << "\n";
		s << prefix << "	op \"";
		printOp(c.op);
		s << "\" length " << c.length << "\n";
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
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_MORPHISMCONSTRAINTS_H */