#include "DFSEncoding.h"

#include <mod/Error.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Chem/Smiles.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/Graph/Properties/Stereo.h>
#include <mod/lib/IO/ParsingUtil.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/home/x3/auxiliary/attr.hpp>
#include <boost/spirit/home/x3/auxiliary/eps.hpp>
#include <boost/spirit/home/x3/char/char.hpp>
#include <boost/spirit/home/x3/directive/lexeme.hpp>
#include <boost/spirit/home/x3/numeric/uint.hpp>
#include <boost/spirit/home/x3/operator/alternative.hpp>
#include <boost/spirit/home/x3/operator/difference.hpp>
#include <boost/spirit/home/x3/operator/kleene.hpp>
#include <boost/spirit/home/x3/operator/optional.hpp>
#include <boost/spirit/home/x3/operator/plus.hpp>
#include <boost/spirit/home/x3/string/literal_string.hpp>
#include <boost/spirit/home/x3/string/symbols.hpp>

#include <boost/variant/variant.hpp>

#include <limits>
#include <map>
#include <ostream>

namespace mod {
namespace lib {
namespace Graph {
namespace DFSEncoding {
namespace {

void escapeLabel(std::ostream &s, const std::string &label, char escChar) {
	for(unsigned int i = 0; i < label.size(); i++) {
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

} // namespace
namespace detail {

const std::string invalidLabelString("");

class LabelVertex;
class RingClosure;
class Branch;

using BaseVertex = boost::variant<LabelVertex, RingClosure>;

using GVertex = lib::Graph::Vertex;
using GEdge = lib::Graph::Edge;

struct LabelVertex {

	LabelVertex() : label(invalidLabelString), implicit(false) { }
public:
	std::string label;
	bool implicit;
	boost::optional<unsigned int> id;
public:
	GVertex gVertex;
public:

	friend std::ostream &operator<<(std::ostream &s, const LabelVertex &lv) {
		if(lv.implicit) s << lv.label;
		else {
			s << '[';
			escapeLabel(s, lv.label, ']');
			s << ']';
		}
		if(lv.id) s << *lv.id;
		return s;
	}
};

struct RingClosure {

	RingClosure() : id(std::numeric_limits<unsigned int>::max()) { }
public:
	unsigned int id;
public:

	friend std::ostream &operator<<(std::ostream &s, const RingClosure &rc) {
		return s << rc.id;
	}
};

struct Vertex {
	BaseVertex vertex;
	std::vector<Branch> branches;
public:
	friend std::ostream &operator<<(std::ostream &s, const Vertex &v);
};

struct Edge {

	Edge() : label(invalidLabelString) { }
public:
	std::string label;
public:

	friend std::ostream &operator<<(std::ostream &s, const Edge &e) {
		if(e.label.size() > 1) {
			s << '{';
			escapeLabel(s, e.label, '}');
			return s << '}';
		}
		switch(e.label.front()) {
		case '-': return s;
		case '=':
		case '#':
		case ':': return s << e.label;
		default: return s << "{" << e.label << "}";
		}
	}
};

using EVPair = std::pair<Edge, Vertex>;

struct Chain {
	Vertex head;
	std::vector<EVPair> tail;
	bool hasNonSmilesRingClosure; // only set when creating GraphDFS from a graph
public:

	friend std::ostream &operator<<(std::ostream &s, const Chain &c) {
		s << c.head;
		for(const auto &ev : c.tail) s << ev.first << ev.second;
		return s;
	}
};

struct Branch {
	std::vector<EVPair> tail;
public:

	friend std::ostream &operator<<(std::ostream &s, const Branch &b) {
		for(const auto &ev : b.tail) s << ev.first << ev.second;
		return s;
	}
};

std::ostream &operator<<(std::ostream &s, const Vertex &v) {
	s << v.vertex;
	for(const auto &b : v.branches) s << b;
	return s;
}

namespace {
namespace parser {

struct SpecialVertexLabels : x3::symbols<const std::string> {

	SpecialVertexLabels() {
		name("specialVertexLabels");
		for(auto atomId : lib::Chem::getSmilesOrganicSubset())
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
const auto escapedStringBrace /*  */ = x3::lexeme[x3::lit('{') > *(escapedBrace /*  */ | plainBrace /*  */) > x3::lit('}')];
const auto escapedStringBracket /**/ = x3::lexeme[x3::lit('[') > *(escapedBracket /**/ | plainBracket /**/) > x3::lit(']')];

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
BOOST_SPIRIT_DEFINE(vertex);
// no recursion
const auto chain = vertex > *evPair;
const auto graphDFS = x3::rule<struct graphDFS, Chain>("graphDFS")
/*               */ = chain;

} // namespace parser
} // namespace

struct Converter : public boost::static_visitor<std::pair<GVertex, bool> > {

	Converter(GraphType &g, PropString &pString, std::ostream &s) : g(g), pString(pString), s(s), error(false) { }

	bool convert(Chain &chain) {
		std::pair<GVertex, bool> res = convert(chain.head, g.null_vertex());
		if(error) return false;
		GVertex vPrev = res.first;
		assert(!res.second);
		assert(vPrev != g.null_vertex());
		for(EVPair &ev : chain.tail) {
			vPrev = attach(ev, vPrev);
			if(error) break;
			assert(vPrev != g.null_vertex());
		}
		return !error;
	}

	std::pair<GVertex, bool> convert(Vertex &vertex, GVertex prev) {
		GVertex next;
		bool isRingClosure;
		boost::tie(next, isRingClosure) = boost::apply_visitor(*this, vertex.vertex);
		if(error) return std::make_pair(next, isRingClosure);
		assert(!(isRingClosure && prev == g.null_vertex()));
		GVertex branchRoot = isRingClosure ? prev : next;
		for(Branch &branch : vertex.branches) {
			GVertex prev = branchRoot;
			for(EVPair &ev : branch.tail) {
				prev = attach(ev, prev);
				if(error) break;
				assert(prev != g.null_vertex());
			}
			if(error) break;
		}
		return std::make_pair(next, isRingClosure);
	}

	void makeRingClosureBond(GVertex vSrc, GVertex vTar, const std::string &label) {
		std::pair<GEdge, bool> e = add_edge(vSrc, vTar, g);
		assert(e.second);
		pString.addEdge(e.first, label);
	}

	GVertex attach(EVPair &ev, GVertex prev) {
		GVertex next;
		bool isRingClosure;
		boost::tie(next, isRingClosure) = convert(ev.second, prev);
		if(!error) makeRingClosureBond(prev, next, ev.first.label);
		if(isRingClosure) return prev;
		else return next;
	}

	std::pair<GVertex, bool> operator()(LabelVertex &vertex) {
		GVertex v = add_vertex(g);
		pString.addVertex(v, vertex.label);
		if(vertex.id) {
			const auto iter = idVertexMap.find(vertex.id.get());
			if(iter == idVertexMap.end()) {
				// use the id as definition
				idVertexMap[vertex.id.get()] = v;
			} else {
				// use the id as ring closure
				makeRingClosureBond(v, iter->second, "-");
			}
		}
		vertex.gVertex = v;
		return std::make_pair(v, false);
	}

	std::pair<GVertex, bool> operator()(const RingClosure &vertex) {
		const auto iter = idVertexMap.find(vertex.id);
		if(iter == idVertexMap.end()) {
			s << "Ring closure id " << vertex.id << " not found." << std::endl;
			error = true;
			return std::make_pair(g.null_vertex(), true);
		}
		return std::make_pair(iter->second, true);
	}
private:
	GraphType &g;
	PropString &pString;
	std::ostream &s;
	bool error;
public:
	std::map<unsigned int, GVertex> idVertexMap;
};

struct ImplicitHydrogenAdder : public boost::static_visitor<void> {

	ImplicitHydrogenAdder(GraphType &g, PropString &pString) : g(g), pString(pString) { }

	void operator()(const Chain &chain) {
		(*this)(chain.head);
		for(const auto &ev : chain.tail) (*this)(ev.second);
	}

	void operator()(const Vertex &vertex) {
		boost::apply_visitor(*this, vertex.vertex);
		for(const auto &b : vertex.branches)
			for(const auto &ev : b.tail) (*this)(ev.second);
	}

	void operator()(const RingClosure&) { }

	void operator()(const LabelVertex &vertex) {
		if(vertex.implicit) {
			// we can only add hydrogens if all incident edges are valid bonds
			for(auto eOut : asRange(out_edges(vertex.gVertex, g))) {
				if(lib::Chem::decodeEdgeLabel(pString[eOut]) == BondType::Invalid)
					return;
			}
			auto atomId = lib::Chem::atomIdFromSymbol(vertex.label);
			if(std::find(begin(lib::Chem::getSmilesOrganicSubset()), end(lib::Chem::getSmilesOrganicSubset()), atomId)
					== end(lib::Chem::getSmilesOrganicSubset())) MOD_ABORT;
			auto hydrogenAdder = [](lib::Graph::GraphType &g, lib::Graph::PropString &pString, lib::Graph::Vertex p) {
				GVertex v = add_vertex(g);
				pString.addVertex(v, "H");
				GEdge e = add_edge(v, p, g).first;
				pString.addEdge(e, "-");
			};
			lib::Chem::addImplicitHydrogens(g, pString, vertex.gVertex, atomId, hydrogenAdder);
		}
	}
private:
	GraphType &g;
	PropString &pString;
};

} // namespace detail

lib::IO::Graph::Read::Data parse(const std::string &dfs, std::ostream &s) {
	using IteratorType = std::string::const_iterator;
	IteratorType first = dfs.begin(), last = dfs.end();
	detail::Chain chain;
	bool res = lib::IO::parse(first, last, detail::parser::graphDFS, chain, s);
	if(!res) return lib::IO::Graph::Read::Data();

	auto g = std::make_unique<GraphType>();
	auto pString = std::make_unique<PropString>(*g);
	detail::Converter conv(*g, *pString, s);
	if(conv.convert(chain)) {
		detail::ImplicitHydrogenAdder adder(*g, *pString);
		adder(chain);
		write(*g, *pString);
		lib::IO::Graph::Read::Data data;
		data.g = std::move(g);
		data.pString = std::move(pString);
		for(auto &&vp : conv.idVertexMap) {
			data.externalToInternalIds[vp.first] = get(boost::vertex_index_t(), *g, vp.second);
		}
		return data;
	} else {
		return lib::IO::Graph::Read::Data();
	}
}

} // namespace DFSEncoding
} // namespace Graph
} // namespace lib
} // namespace mod

BOOST_FUSION_ADAPT_STRUCT(mod::lib::Graph::DFSEncoding::detail::LabelVertex,
		(std::string, label)
		(bool, implicit)
		(boost::optional<unsigned int>, id))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Graph::DFSEncoding::detail::RingClosure,
		(unsigned int, id))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Graph::DFSEncoding::detail::Vertex,
		(mod::lib::Graph::DFSEncoding::detail::BaseVertex, vertex)
		(std::vector<mod::lib::Graph::DFSEncoding::detail::Branch>, branches))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Graph::DFSEncoding::detail::Edge,
		(std::string, label))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Graph::DFSEncoding::detail::Chain,
		(mod::lib::Graph::DFSEncoding::detail::Vertex, head)
		(std::vector<mod::lib::Graph::DFSEncoding::detail::EVPair>, tail))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Graph::DFSEncoding::detail::Branch,
		(std::vector<mod::lib::Graph::DFSEncoding::detail::EVPair>, tail))

#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/IO/IO.h>

#include <stack>
#include <string>
#include <sstream>

namespace mod {
namespace lib {
namespace Graph {
namespace DFSEncoding {
namespace detail {

enum class Colour {
	White, Grey, Black
};

struct Printer : public boost::static_visitor<void> {

	Printer(std::ostream &s, std::map<unsigned int, unsigned int> idMap) : s(s), idMap(idMap) { }

	void operator()(const Chain &chain) {
		(*this)(chain.head);
		(*this)(chain.tail);
	}

	void operator()(const Vertex &v) {
		boost::apply_visitor(*this, v.vertex);
		for(const Branch &b : v.branches) {
			s << "(";
			(*this)(b.tail);
			s << ")";
		}
	}

	void operator()(const LabelVertex &v) {
		s << "[";
		escapeLabel(s, v.label, ']');
		s << "]";
		if(v.id && idMap.find(v.id.get()) != idMap.end()) s << idMap[v.id.get()];
	}

	void operator()(const RingClosure &v) {
		assert(v.id != std::numeric_limits<unsigned int>::max());
		s << idMap[v.id];
	}

	void operator()(const std::vector<EVPair> &evPairs) {
		for(const EVPair &p : evPairs) {
			(*this)(p.first);
			(*this)(p.second);
		}
	}

	void operator()(const Edge &e) {
		if(e.label.size() == 1) {
			char c = e.label[0];
			switch(c) {
			case '-': return;
			case ':':
			case '=':
			case '#':
				s << c;
				return;
			}
		}
		s << "{";
		escapeLabel(s, e.label, '}');
		s << "}";
	}
private:
	std::ostream &s;
	std::map<unsigned int, unsigned int> idMap;
};

struct Prettyfier : public boost::static_visitor<void> {

	Prettyfier(const std::vector<bool> &targetForRing) : targetForRing(targetForRing) { }

	void operator()(Chain &chain) {
		(*this)(chain.head);
		(*this)(chain.tail);
		if(chain.head.branches.size() > 0 && chain.tail.size() == 0) {
			chain.tail = chain.head.branches.back().tail;
			chain.head.branches.pop_back();
		}
	}

	void operator()(Vertex &v) {
		boost::apply_visitor(*this, v.vertex);
		for(Branch &b : v.branches) (*this)(b.tail);
	}

	void operator()(LabelVertex &v) {
		if(v.id)
			if(!targetForRing[v.id.get()]) v.id.reset();
	}

	void operator()(RingClosure &v) {
		assert(v.id != std::numeric_limits<unsigned int>::max());
		assert(targetForRing[v.id]);
	}

	void operator()(std::vector<EVPair> &evPairs) {
		for(EVPair &p : evPairs) {
			(*this)(p.first);
			(*this)(p.second);
		}
		if(evPairs.size() > 0 && evPairs.back().second.branches.size() > 0) {
			unsigned int index = evPairs.size() - 1;
			std::vector<EVPair> tempCopy = evPairs.back().second.branches.back().tail;
			std::vector<EVPair> newVec = evPairs;
			newVec.insert(newVec.end(), tempCopy.begin(), tempCopy.end());
			newVec[index].second.branches.pop_back();
			evPairs = newVec;
		}
	}

	void operator()(Edge &e) { }
private:
	const std::vector<bool> targetForRing;
};

std::pair<Chain, std::map<unsigned int, unsigned int> > write(const lib::Graph::GraphType &g, const PropString &pString) {
	using namespace detail;
	using GEdgeIter = lib::Graph::GraphType::out_edge_iterator;
	typedef std::pair<GVertex, std::pair<GEdgeIter, GEdgeIter> > VertexInfo;
	std::vector<Vertex*> realVertices(num_vertices(g), nullptr);
	Chain chain;
	chain.hasNonSmilesRingClosure = false;

	std::vector<Colour> colour(num_vertices(g), Colour::White);
	std::vector<bool> targetForRing(num_vertices(g), false);
	std::map<GEdge, Colour> edgeColour;
	for(GEdge e : asRange(edges(g))) edgeColour[e] = Colour::White;
	std::stack<VertexInfo> stack;
	{ // discover root
		GVertex cur = *vertices(g).first;
		unsigned int curId = get(boost::vertex_index_t(), g, cur);
		assert(curId < colour.size());
		colour[curId] = Colour::Grey;
		assert(!realVertices[curId]);
		realVertices[curId] = &chain.head;
		LabelVertex lv;
		lv.id = curId;
		lv.label = pString[cur];
		realVertices[curId]->vertex = lv;
		stack.push(std::make_pair(cur, out_edges(cur, g)));
	}
	while(!stack.empty()) {
		GVertex cur = stack.top().first;
		GEdgeIter iter, iterEnd;
		boost::tie(iter, iterEnd) = stack.top().second;
		stack.pop();
		unsigned int curId = get(boost::vertex_index_t(), g, cur);
		assert(realVertices[curId]);
		Vertex *curVertex = realVertices[curId];
		//		std::cout << "CurVertex: " << pString(cur) << std::endl;
		while(iter != iterEnd) {
			GVertex next = target(*iter, g);
			GEdge test;
			unsigned int nextId = get(boost::vertex_index_t(), g, next);
			Edge edge;
			edge.label = pString[*iter];
			// mark edge
			Colour oldEdgeColour = edgeColour[*iter];
			edgeColour[*iter] = Colour::Black;
			//			std::cout << "\tEdge: " << pString(cur)
			//					<< " ->(" << edge.label << ", " << (oldEdgeColour == Black ? "black" : "white") << ") "
			//					<< pString(next) << "\t";
			if(colour[nextId] == Colour::White) { // tree edge, new vertex
				//				std::cout << "white" << std::endl;
				// create the new vertex
				assert(!realVertices[nextId]);
				LabelVertex lv;
				lv.id = nextId;
				lv.label = pString[next];
				Vertex newVertex;
				newVertex.vertex = lv;
				curVertex->branches.push_back((Branch()));
				curVertex->branches.back().tail.push_back(std::make_pair(edge, newVertex));
				Vertex *nextVertex = &curVertex->branches.back().tail.back().second;
				realVertices[nextId] = nextVertex;
				colour[nextId] = Colour::Grey;
				// switch to the new vertex
				iter++;
				stack.push(std::make_pair(cur, std::make_pair(iter, iterEnd)));
				cur = next;
				curId = nextId;
				curVertex = nextVertex;
				boost::tie(iter, iterEnd) = out_edges(next, g);
				//				std::cout << "CurVertex: " << pString(cur)<< std::endl;
			} else if(colour[nextId] == Colour::Grey) { // back edge, maybe an already traversed edge
				//				std::cout << "grey" << std::endl;
				if(oldEdgeColour == Colour::Black) iter++; // already traversed
				else {
					RingClosure rc;
					rc.id = nextId;
					Vertex backVertex;
					backVertex.vertex = rc;
					curVertex->branches.push_back((Branch()));
					curVertex->branches.back().tail.push_back(std::make_pair(edge, backVertex));
					if(targetForRing[nextId]) chain.hasNonSmilesRingClosure = true;
					targetForRing[nextId] = true;
					iter++;
				}
			} else {
				//				std::cout << "black" << std::endl;
				iter++;
			}
		}
		colour[curId] = Colour::Black;
	}

	std::map<unsigned int, unsigned int> idMap;
	unsigned int nextMappedId = 1;
	for(unsigned int id = 0; id < targetForRing.size(); id++) if(targetForRing[id]) idMap[id] = nextMappedId++;
	Prettyfier pretty(targetForRing);
	pretty(chain);
	return std::make_pair(chain, idMap);
}

} // namespace detail

std::pair<std::string, bool> write(const lib::Graph::GraphType &g, const PropString &pString) {
	if(num_vertices(g) == 0) return std::make_pair("", false);
	using namespace detail;
	std::pair<Chain, std::map<unsigned int, unsigned int> > res = detail::write(g, pString);
	Chain &chain = res.first;
	std::map<unsigned int, unsigned int> &idMap = res.second;

	std::stringstream graphDFS;
	Printer p(graphDFS, idMap);
	p(chain);
	return std::make_pair(graphDFS.str(), chain.hasNonSmilesRingClosure);
}

} // namespace DFSEncoding
} // namespace Graph
} // namespace lib
} // namespace mod

