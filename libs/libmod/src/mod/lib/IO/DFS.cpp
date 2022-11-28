#include "DFS.hpp"

#include <mod/lib/Chem/MoleculeUtil.hpp>
#include <mod/lib/Chem/Smiles.hpp>
#include <mod/lib/IO/Parsing.hpp>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/home/x3/auxiliary/attr.hpp>
#include <boost/spirit/home/x3/char/char.hpp>
#include <boost/spirit/home/x3/char/char_class.hpp>
#include <boost/spirit/home/x3/directive/lexeme.hpp>
#include <boost/spirit/home/x3/numeric/uint.hpp>
#include <boost/spirit/home/x3/operator/alternative.hpp>
#include <boost/spirit/home/x3/operator/difference.hpp>
#include <boost/spirit/home/x3/operator/kleene.hpp>
#include <boost/spirit/home/x3/operator/optional.hpp>
#include <boost/spirit/home/x3/operator/plus.hpp>
#include <boost/spirit/home/x3/string/literal_string.hpp>
#include <boost/spirit/home/x3/string/symbols.hpp>

#include <ostream>

namespace mod::lib::IO::DFS {

void escapeLabel(std::ostream &s, const std::string &label, char escChar) {
	for(int i = 0; i != label.size(); i++) {
		char c = label[i];
		if(c == escChar) s << "\\" << escChar;
		else if(c == '\t') s << "\\t";
		else if(c == '\\' && i + 1 != label.size()) {
			char next = label[i + 1];
			if(next == '\\' || next == 't') s << "\\\\";
			else s << '\\';
		} else s << c;
	}
}

std::ostream &operator<<(std::ostream &s, const LabelVertex &lv) {
	if(lv.implicit) s << lv.label;
	else {
		s << '[';
		escapeLabel(s, lv.label, ']');
		s << ']';
	}
	if(lv.id) s << *lv.id;
	return s;
}

std::ostream &operator<<(std::ostream &s, const RingClosure &rc) {
	return s << rc.id;
}

std::ostream &operator<<(std::ostream &s, const Vertex &v) {
	s << v.vertex;
	for(const auto &b: v.branches) s << b;
	return s;
}

std::ostream &operator<<(std::ostream &s, const Edge &e) {
	if(e.label.size() > 1) {
		s << '{';
		escapeLabel(s, e.label, '}');
		return s << '}';
	}
	switch(e.label.front()) {
	case '-':
		return s;
	case '=':
	case '#':
	case ':':
		return s << e.label;
	default:
		return s << "{" << e.label << "}";
	}
}

std::ostream &operator<<(std::ostream &s, const Chain &c) {
	s << c.head;
	for(const auto &ev: c.tail) s << ev.first << ev.second;
	return s;
}

std::ostream &operator<<(std::ostream &s, const Branch &b) {
	for(const auto &ev: b.tail) s << '(' << ev.first << ev.second << ')';
	return s;
}

std::ostream &operator<<(std::ostream &s, const Rule &r) {
	if(r.left) s << *r.left;
	s << "<<";
	if(r.right) s << *r.right;
	return s;
}

namespace Read {
namespace {
namespace parser {

struct SpecialVertexLabels : x3::symbols<const std::string> {
	SpecialVertexLabels() {
		name("specialVertexLabels");
		for(auto atomId: lib::Chem::getSmilesOrganicSubset())
			add(lib::Chem::symbolFromAtomId(atomId), lib::Chem::symbolFromAtomId(atomId));
	}
} specialVertexLabels;

struct SpecialEdgeLabels : x3::symbols<const std::string> {
	SpecialEdgeLabels() {
		name("specialEdgeLabels");
		add("-", "-");
		add(":", ":");
		add("=", "=");
		add("#", "#");
		add(".", "");
	}
} specialEdgeLabelSymbols;

// no recursion
const auto implicitBackslash = x3::attr('\\');
const auto explicitBackslash = '\\' >> x3::attr('\\');
const auto tab = 't' >> x3::attr('\t');
const auto plainBrace /*  */ = (x3::char_ - x3::char_('}'));
const auto plainBracket /**/ = (x3::char_ - x3::char_(']'));
const auto escapedBrace /*  */ = '\\' >> (x3::char_('}') | tab | explicitBackslash | implicitBackslash);
const auto escapedBracket /**/ = '\\' >> (x3::char_(']') | tab | explicitBackslash | implicitBackslash);
const auto escapedStringBrace /*  */ = x3::lexeme[x3::lit('{') > *(escapedBrace /*  */ | plainBrace /*  */) >
                                                  x3::lit('}')];
const auto escapedStringBracket /**/ = x3::lexeme[x3::lit('[') > *(escapedBracket /**/ | plainBracket /**/) >
                                                  x3::lit(']')];

const auto specialEdgeLabels = specialEdgeLabelSymbols | x3::attr(std::string(1, '-'));
const auto edge = x3::rule<struct edge, Edge>("edge") = escapedStringBrace | specialEdgeLabels;
const auto defRingId = x3::uint_;
const auto ringClosure = x3::rule<struct ringClosure, RingClosure>("ringClosure") = x3::uint_;
const auto specialLabelVertex = x3::rule<struct specialLabelVertex, LabelVertex>("specialLabelVertex")
/*                         */ = specialVertexLabels >> x3::attr(true) >> -defRingId;
const auto explicitLabelVertex = x3::rule<struct specialLabelVertex, LabelVertex>("explicitLabelVertex")
/*                          */ = escapedStringBracket >> x3::attr(false) >> -defRingId;
const auto labelVertex = explicitLabelVertex | specialLabelVertex;
// part of recursion
const x3::rule<struct vertex, Vertex> vertex = "vertex";
const auto evPair = x3::rule<struct evPair, EVPair>("edgeVertexPair") = edge >> vertex;
const auto branch = x3::rule<struct branch, Branch>("branch") = '(' > +evPair > ')';
const auto branches = *branch;
const auto vertex_def = (labelVertex | ringClosure) >> branches;
BOOST_SPIRIT_DEFINE(vertex)
// no recursion
const auto chain = vertex > *evPair;
const auto graphDFS = x3::rule<struct graphDFS, Chain>("graphDFS")
/*               */ = chain;
const auto ruleDFS = x3::rule<struct ruleDFS, Rule>("ruleDFS")
/*              */ = -graphDFS > x3::lit(">>") > -graphDFS;

} // namespace parser

struct Phase1 {
	// - Set connectedComponentIDs
	// - Record vertex IDs
	using Map = std::map<int, LabelVertex *>;
	// - Resolve rings
public:
	Phase1(std::string errorVariation) : errorVariation(std::move(errorVariation)) {}
	lib::IO::Result<std::pair<int, Map>> operator()(Chain &chain) {
		(*this)(chain.head);
		for(EVPair &ev: chain.tail)
			(*this)(ev.second);
		if(error.empty())
			return std::pair(nextID, std::move(vertexFromId));
		else
			return lib::IO::Result<>::Error(std::move(error));
	}

	void operator()(Vertex &vertex) {
		boost::apply_visitor(*this, vertex.vertex);
		for(Branch &branch: vertex.branches)
			for(EVPair &ev: branch.tail)
				(*this)(ev.second);
	}

	void operator()(LabelVertex &vertex) {
		vertex.connectedComponentID = nextID;
		++nextID;

		if(vertex.id) {
			const auto iter = vertexFromId.find(*vertex.id);
			if(iter == vertexFromId.end()) {
				// use the id as definition
				vertexFromId[*vertex.id] = &vertex;
			} else {
				// use the id as ring closure
				vertex.ringClosure = iter->second;
			}
		}
	}

	void operator()(RingClosure &vertex) {
		const auto iter = vertexFromId.find(vertex.id);
		if(iter != vertexFromId.end()) {
			vertex.other = iter->second;
		} else {
			// there may have been a previous error
			if(error.empty())
				error = "Ring closure ID " + std::to_string(vertex.id) + " not found" + errorVariation + ".";
		}
	}
public:
	int nextID = 0;
	Map vertexFromId;
	std::string error;
	std::string errorVariation;
};

struct CheckLoop {
	// - Check for loop edges, by ring resolution to previous vertex
	CheckLoop(std::string errorVariation) : errorVariation(std::move(errorVariation)) {}

	lib::IO::Result<> operator()(const Chain &chain) {
		const LabelVertex *prevVertex = (*this)(nullptr, nullptr, chain.head);
		for(const EVPair &ev: chain.tail) {
			prevVertex = (*this)(prevVertex, &ev.first, ev.second);
			if(!error.empty()) break;
		}

		if(error.empty())
			return {};
		else
			return lib::IO::Result<>::Error(std::move(error));
	}

	const LabelVertex *operator()(const LabelVertex *pVertex, const Edge *edge, const Vertex &vertex) {
		assert((pVertex == nullptr) == (edge == nullptr));
		auto [thisVertex, ringId] = boost::apply_visitor(*this, vertex.vertex);
		assert(thisVertex);
		if(ringId >= 0 && thisVertex == pVertex && !edge->label.empty()) {
			assert(error.empty());
			error = "Loop edge in DFS on vertex" + errorVariation + " with ID " + std::to_string(ringId) + ".";
			return pVertex;
		}

		const LabelVertex *baseVertex = ringId < 0 ? thisVertex : pVertex;

		for(const Branch &branch: vertex.branches) {
			const LabelVertex *pBranchVertex = baseVertex;
			for(const EVPair &ev: branch.tail) {
				pBranchVertex = (*this)(pBranchVertex, &ev.first, ev.second);
				if(!error.empty()) break;
			}
		}

		return baseVertex;
	}

	std::pair<const LabelVertex *, int> operator()(const LabelVertex &vertex) {
		return {&vertex, -1};
	}

	std::pair<const LabelVertex *, int> operator()(const RingClosure &vertex) {
		return {vertex.other, vertex.id};
	}
public:
	std::string error;
	std::string errorVariation;
};

struct CheckParallel {
	// - Check for parallel edges, by ring-closure to just previous vertex
	CheckParallel(std::string errorVariation) : errorVariation(std::move(errorVariation)) {}

	lib::IO::Result<> operator()(const Chain &chain) {
		auto [ppVertex, pVertex] = (*this)(nullptr, nullptr, nullptr, chain.head);
		for(const EVPair &ev: chain.tail) {
			std::tie(ppVertex, pVertex) = (*this)(ppVertex, pVertex, &ev.first, ev.second);
			if(!error.empty()) break;
		}

		if(error.empty())
			return {};
		else
			return lib::IO::Result<>::Error(std::move(error));
	}

	std::pair<const LabelVertex *, const LabelVertex *>
	operator()(const LabelVertex *ppVertex, const LabelVertex *pVertex, const Edge *edge, const Vertex &vertex) {
		assert((pVertex == nullptr) == (edge == nullptr));
		auto [thisVertex, ringId] = boost::apply_visitor(*this, vertex.vertex);
		assert(thisVertex);
		if(edge && !edge->label.empty()) {
			// not [A].[B] with (edge, vertex) being (., [B])
			if(ringId >= 0 && thisVertex == ppVertex) {
				// [A]1[B]-1 with (edge, vertex) being (-, 1)
				assert(error.empty());
				error = "Parallel edge in DFS" + errorVariation + ". Back-edge is to vertex with ID " +
				        std::to_string(ringId) + ".";
				return {nullptr, nullptr};
			}
			if(ringId < 0 && thisVertex->ringClosure && thisVertex->ringClosure == pVertex) {
				// [A]1[B]1 with (edge, vertex) being ('-', [B]1)
				assert(error.empty());
				error = "Parallel edge in DFS" + errorVariation + ". Back-edge is to vertex with ID " +
				        std::to_string(*thisVertex->id) + ".";
				return {nullptr, nullptr};
			}
		}

		if(ringId < 0) {
			// thisVertex is an actual vertex
			if(!edge || !edge->label.empty()) {
				// [ppVertex][pVertex][thisVertex]
				ppVertex = pVertex;
				pVertex = thisVertex;
			} else {
				// [ppVertex][pVertex].[thisVertex]
				// so semi-reset
				ppVertex = nullptr;
				pVertex = thisVertex;
			}
		} else {
			// thisVertex is just a ringClosure, i.e., an implicit branch
			// keep ppVertex and pVertex
		}
		for(const Branch &branch: vertex.branches) {
			const LabelVertex *ppBranchVertex = ppVertex;
			const LabelVertex *pBranchVertex = pVertex;
			for(const EVPair &ev: branch.tail) {
				std::tie(ppBranchVertex, pBranchVertex) = (*this)(ppBranchVertex, pBranchVertex, &ev.first, ev.second);
				if(!error.empty()) break;
			}
		}

		return {ppVertex, pVertex};
	}

	std::pair<const LabelVertex *, int> operator()(const LabelVertex &vertex) {
		return {&vertex, -1};
	}

	std::pair<const LabelVertex *, int> operator()(const RingClosure &vertex) {
		return {vertex.other, vertex.id};
	}
public:
	std::string error;
	std::string errorVariation;
};

} // namespace

Result<GraphResult> graph(std::string_view data) {
	auto ast = std::make_unique<Chain>();
	try {
		lib::IO::parse(data.begin(), data.end(), parser::graphDFS, *ast, true, x3::ascii::space);
	} catch(const lib::IO::ParsingError &e) {
		return lib::IO::Result<>::Error(e.msg);
	}

	auto resPhase1 = Phase1("")(*ast);
	if(!resPhase1) return lib::IO::Result<>::Error(resPhase1.extractError());
	auto [numVertices, vertexFromId] = *resPhase1;
	if(auto res = CheckLoop("")(*ast); !res) return res;
	if(auto res = CheckParallel("")(*ast); !res) return res;
	return GraphResult{std::move(ast), numVertices, std::move(vertexFromId)};
}

Result<RuleResult> rule(std::string_view data) {
	Rule ast;
	try {
		lib::IO::parse(data.begin(), data.end(), parser::ruleDFS, ast, true, x3::ascii::space);
	} catch(const lib::IO::ParsingError &e) {
		return lib::IO::Result<>::Error(e.msg);
	}
	RuleResult res;
	if(ast.left) {
		res.left.ast = std::make_unique<Chain>(std::move(*ast.left));
		auto resPhase1 = Phase1(", in the left side")(*res.left.ast);
		if(!resPhase1) return lib::IO::Result<>::Error(resPhase1.extractError());
		std::tie(res.left.numVertices, res.left.vertexFromId) = *resPhase1;
		if(auto resLoop = CheckLoop(" in the left side")(*res.left.ast); !resLoop) return resLoop;
		if(auto resPar = CheckParallel(" in the left side")(*res.left.ast); !resPar) return resPar;
	}
	if(ast.right) {
		res.right.ast = std::make_unique<Chain>(std::move(*ast.right));
		auto resPhase1 = Phase1(", in the right side")(*res.right.ast);
		if(!resPhase1) return lib::IO::Result<>::Error(resPhase1.extractError());
		std::tie(res.right.numVertices, res.right.vertexFromId) = *resPhase1;
		if(auto resLoop = CheckLoop(" in the right side")(*res.right.ast); !resLoop) return resLoop;
		if(auto resPar = CheckParallel(" in the right side")(*res.right.ast); !resPar) return resPar;
	}
	return std::move(res); // TODO: remove std::move when C++20/P1825R0 is available
}

} // namespace Read
} // namespace mod::lib::IO::DFS

BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::DFS::LabelVertex,
                          (std::string, label)
		                          (bool, implicit)
		                          (std::optional<int>, id))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::DFS::RingClosure,
                          (unsigned int, id))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::DFS::Vertex,
                          (mod::lib::IO::DFS::BaseVertex, vertex)
		                          (std::vector<mod::lib::IO::DFS::Branch>, branches))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::DFS::Edge,
                          (std::string, label))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::DFS::Chain,
                          (mod::lib::IO::DFS::Vertex, head)
		                          (std::vector<mod::lib::IO::DFS::EVPair>, tail))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::DFS::Branch,
                          (std::vector<mod::lib::IO::DFS::EVPair>, tail))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::DFS::Rule,
                          (std::optional<mod::lib::IO::DFS::Chain>, left)
		                          (std::optional<mod::lib::IO::DFS::Chain>, right))