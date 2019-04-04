#include "Dump.h"

#include <mod/Error.h>
#include <mod/graph/Graph.h>
#include <mod/rule/Rule.h>
#include <mod/lib/DG/Hyper.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/Stereo.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/IO/Graph.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/ParsingUtil.h>
#include <mod/lib/Rules/Real.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/spirit/home/x3/char/char.hpp>
#include <boost/spirit/home/x3/char/char_class.hpp>
#include <boost/spirit/home/x3/directive/lexeme.hpp>
#include <boost/spirit/home/x3/numeric/int.hpp>
#include <boost/spirit/home/x3/numeric/uint.hpp>
#include <boost/spirit/home/x3/string/literal_string.hpp>
#include <boost/spirit/home/x3/operator/difference.hpp>
#include <boost/spirit/home/x3/operator/kleene.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Dump {
namespace {

struct ConstructionData {
	const std::vector<std::shared_ptr<rule::Rule> > &rules;
	std::vector<std::tuple<unsigned int, std::string, std::unique_ptr<lib::Graph::GraphType>, std::unique_ptr<lib::Graph::PropString> > > vertices;
	const std::vector<std::tuple<unsigned int, std::string> > rulesParsed;
	const std::vector<std::tuple<unsigned int, std::vector<unsigned int>, std::vector<int> > > edges;
};

struct NonHyperDump : public NonHyper {

	NonHyperDump(const std::vector<std::shared_ptr<graph::Graph> > &graphs,
			ConstructionData &&constructionData)
	: NonHyper(graphs,{LabelType::String, LabelRelation::Isomorphism}), constructionData(&constructionData) {
		calculate(true);
	}
private:

	virtual std::string getType() const override {
		return "DGDump";
	}

	virtual void calculateImpl(bool printInfo) override {
		const std::vector<std::shared_ptr<rule::Rule> > &rules = constructionData->rules;
		auto &vertices = constructionData->vertices;
		const auto &rulesParsed = constructionData->rulesParsed;
		const auto &edges = constructionData->edges;
		std::unordered_map<unsigned int, std::shared_ptr<rule::Rule> > ruleMap;
		std::unordered_map<unsigned int, std::shared_ptr<graph::Graph> > graphMap;
		for(const auto &t : rulesParsed) {
			auto iter = std::find_if(begin(rules), end(rules), [&t](std::shared_ptr<rule::Rule> r) -> bool {
				return r->getName() == get<1>(t);
			});
			if(iter != end(rules)) {
				std::shared_ptr<rule::Rule> r = *iter;
				if(printInfo)
					IO::log() << "Rule linked: " << r->getName() << std::endl;
				this->rules.push_back(r);
				ruleMap[get<0>(t)] = r;
			} else {
				std::string msg = "Can not load DG dump. Rule not found: '" + get<1>(t) + "'";
				throw InputError(std::move(msg));
			}
		}

		// do merge of vertices and edges in order of increasing id
		unsigned int iVertices = 0;
		unsigned int iEdges = 0;
		for(unsigned int id = 0; id < vertices.size() + edges.size(); id++) {
			if(iVertices < vertices.size() && get<0>(vertices[iVertices]) == id) {
				auto &v = vertices[iVertices];
				auto gCand = std::make_unique<lib::Graph::Single>(std::move(get<2>(v)), std::move(get<3>(v)), nullptr);
				auto p = checkIfNew(std::move(gCand));
				bool wasNew = addGraphAsVertex(p.first);
				graphMap[id] = p.first;
				if(!p.second && printInfo) IO::log() << "Graph linked: " << get<1>(v) << " -> " << p.first->getName() << std::endl;
				if(wasNew) giveProductStatus(p.first);
				iVertices++;
			} else if(iEdges < edges.size() && get<0>(edges[iEdges]) == id) {
				const auto &e = edges[iEdges];
				std::vector<const lib::Graph::Single*> srcGraphs, tarGraphs;
				for(int adj : get<2>(e)) {
					assert(adj != 0);
					unsigned int id = std::abs(adj) - 1;
					auto &m = adj < 0 ? srcGraphs : tarGraphs;
					auto gIter = graphMap.find(id);
					assert(gIter != end(graphMap));
					m.push_back(&gIter->second->getGraph());
				}
				GraphMultiset gmsSrc(std::move(srcGraphs)), gmsTar(std::move(tarGraphs));
				const auto &ruleIds = get<1>(e);
				suggestDerivation(gmsSrc, gmsTar, nullptr);
				for(const auto rId : ruleIds) {
					auto rIter = ruleMap.find(rId);
					assert(rIter != end(ruleMap));
					const auto *r = &rIter->second->getRule();
					suggestDerivation(gmsSrc, gmsTar, r);
				}
				iEdges++;
			} else {
				MOD_ABORT;
			}
		}
		constructionData = nullptr;
	}

	virtual void listImpl(std::ostream &s) const override { }
private:
	std::vector<std::shared_ptr<rule::Rule> > rules;
	ConstructionData *constructionData;
};

using BaseIteratorType = std::istream_iterator<char>;
using IteratorType = spirit::multi_pass<BaseIteratorType>;
using PosIter = IO::PositionIter<IteratorType>;

template<typename Parser, typename Attr>
bool parse(IteratorType &textFirst, PosIter &first, const PosIter &last, const Parser &p, Attr &attr, std::ostream &err) {
	try {
		bool res = IO::detail::ParseDispatch<x3::space_type>::parse(first, last, p, attr, x3::space);
		if(!res) {
			err << "Error while parsing DG dump.\n";
			IO::detail::doParserError(textFirst, first, last, lib::IO::log());
			return false;
		}
		return res;
	} catch(const x3::expectation_failure<PosIter> &e) {
		err << "Error while parsing DG dump.\n";
		IO::detail::doParserExpectationError(e, textFirst, last, err);
		return false;
	}
}

} // namespace

NonHyper *load(const std::vector<std::shared_ptr<graph::Graph> > &graphs, const std::vector<std::shared_ptr<rule::Rule> > &rules, std::istream &s, std::ostream &err) {

	struct FlagsHolder {
		std::istream &s;
		std::ios::fmtflags flags;

		FlagsHolder(std::istream &s) : s(s), flags(s.flags()) {
			s.unsetf(std::ios::skipws);
		}

		~FlagsHolder() {
			s.flags(flags);
		}
	} flagsHolder(s);
	IteratorType textFirst = spirit::make_default_multi_pass(BaseIteratorType(s));
	IteratorType textLast;
	PosIter first(textFirst), last(textLast);

	unsigned int numVertices, numEdges, numRules;
	std::vector<std::tuple<unsigned int, std::string, std::unique_ptr<lib::Graph::GraphType>, std::unique_ptr<lib::Graph::PropString> > > vertices;
	std::vector<std::tuple<unsigned int, std::string> > rulesParsed;
	std::vector<std::tuple<unsigned int, std::vector<unsigned int>, std::vector<int> > > edges;

	std::unordered_set<unsigned int> validVertices, validRules;
#define PARSE(p, a) if(!parse(textFirst, first, last, p, a, err)) return nullptr
#define TRY_PARSE(p, a) parse(textFirst, first, last, p, a, err)
	unsigned int version = 1;
	TRY_PARSE("version:" >> x3::uint_, version);
	PARSE("numVertices:" >> x3::uint_, numVertices);
	PARSE("numEdges:" >> x3::uint_, numEdges);
	PARSE("numRules:" >> x3::uint_, numRules);
	for(unsigned int i = 0; i < numVertices; i++) {
		unsigned int id;
		std::string name, dfs;
		PARSE("vertex:" >> x3::uint_, id);
		PARSE('"' >> x3::lexeme[*(x3::char_ - '"') >> '"'], name);
		PARSE('"' >> x3::lexeme[*(x3::char_ - '"') >> '"'], dfs);
		auto gData = IO::Graph::Read::dfs(dfs, err);
		if(!gData.g) {
			err << "GraphDFS \"" << dfs << "\" could not be parsed for vertex " << id << std::endl;
			return nullptr;
		}
		if(gData.pStereo) MOD_ABORT;
		vertices.emplace_back(id, name, std::move(gData.g), std::move(gData.pString));
		validVertices.insert(id);
	}
	for(unsigned int i = 0; i < numRules; i++) {
		unsigned int id;
		std::string name;
		PARSE("rule:" >> x3::uint_, id);
		PARSE('"' >> x3::lexeme[*(x3::char_ - '"') >> '"'], name);
		rulesParsed.emplace_back(id, name);
		if(validRules.find(id) != end(validRules)) {
			err << "Parsed data is corrupt, duplicate rule id, " << id << std::endl;
			return nullptr;
		}
		validRules.insert(id);
	}
	for(unsigned int i = 0; i < numEdges; i++) {
		unsigned int id;
		std::vector<unsigned int> ruleIds;
		std::vector<int> adj;
		PARSE("edge:" >> x3::uint_, id);
		unsigned int numRules;
		if(version == 1) numRules = 1;
		else PARSE(x3::uint_, numRules);
		for(std::size_t i = 0; i < numRules; ++i) {
			unsigned int ruleId;
			PARSE(x3::uint_, ruleId);
			ruleIds.push_back(ruleId);
		}
		PARSE(*x3::int_, adj);
		for(const auto rId : ruleIds) {
			if(validRules.find(rId) == end(validRules)) {
				err << "Parsed data is corrupt, ruleId, " << rId << ", out of range [0, " << numRules << "[ for edge " << id << std::endl;
				return nullptr;
			}
		}
		for(int a : adj) {
			if(a == 0) {
				err << "Parsed data is corrut, adjacency is 0 for edge " << id << std::endl;
				return nullptr;
			}
			if(validVertices.find(std::abs(a) - 1) == end(validVertices)) {
				err << "Parsed data is corrupt, adjacency, " << a << ", is not a valid vertex for edge " << id << std::endl;
				return nullptr;
			}
			if(std::abs(a) - 1 >= static_cast<int> (id)) {
				err << "Parsed data is corrupt, adjacency " << a << " for edge " << id << " is too large." << std::endl;
				return nullptr;
			}
		}
		edges.emplace_back(id, ruleIds, adj);
	}
	using Vertex = decltype(vertices)::value_type;
	using Edge = decltype(edges)::value_type;
	std::sort(begin(vertices), end(vertices), [](const Vertex &a, const Vertex & b) {
		return get<0>(a) < get<0>(b);
	});
	std::sort(begin(edges), end(edges), [](const Edge &a, const Edge & b) {
		return get<0>(a) < get<0>(b);
	});
	unsigned int n = std::max(vertices.empty() ? 0 : get<0>(vertices.back()), edges.empty() ? 0 : get<0>(edges.back()));
	std::vector<bool> used(n, false);
	for(const auto &v : vertices) {
		unsigned int id = get<0>(v);
		if(used[id]) {
			err << "Parsed data is corrupt, id " << id << " is duplicated." << std::endl;
			return nullptr;
		}
		used[id] = true;
	}
	for(const auto &e : edges) {
		unsigned int id = get<0>(e);
		if(used[id]) {
			err << "Parsed data is corrupt, id " << id << " is duplicated." << std::endl;
			return nullptr;
		}
		used[id] = true;
	}
	for(unsigned int i = 0; i < used.size(); i++) {
		if(!used[i]) {
			err << "Parsed data is corrupt, id " << i << " is unused." << std::endl;
			return nullptr;
		}
	}
	return new NonHyperDump(graphs, ConstructionData{rules, std::move(vertices), rulesParsed, edges});
}

void write(const NonHyper &dgNonHyper, std::ostream & s) {
	if(dgNonHyper.getLabelSettings().withStereo) {
		throw mod::LogicError("Can not yet dump DGs with stereo data.");
	}
	using Vertex = lib::DG::HyperVertex;
	using Edge = lib::DG::HyperEdge;
	using VertexKind = lib::DG::HyperVertexKind;
	const lib::DG::HyperGraphType &dg = dgNonHyper.getHyper().getGraph();
	unsigned int numVertices = 0, numEdges = 0;

	for(const auto v : asRange(vertices(dg))) {
		if(dg[v].kind == VertexKind::Vertex) numVertices++;
		else numEdges++;
	}

	std::set<const lib::Rules::Real*, lib::Rules::LessById> rules;

	for(const auto v : asRange(vertices(dg))) {
		if(dg[v].kind != VertexKind::Edge) continue;
		for(const auto *r : dg[v].rules) rules.insert(r);
	}

	s << "version:\t2\n";
	s << "numVertices:\t" << numVertices << "\n";
	s << "numEdges:\t" << numEdges << "\n";
	s << "numRules:\t" << rules.size() << "\n";

	for(const auto v : asRange(vertices(dg))) {
		if(dg[v].kind != VertexKind::Vertex) continue;
		const auto id = get(boost::vertex_index_t(), dg, v);
		const lib::Graph::Single *g = dg[v].graph;
		assert(g);
		s << "vertex:\t" << id << "\t\"" << g->getName() << "\"\t\"" << g->getGraphDFS().first << "\"\n";
	}

	for(const auto *r : rules) s << "rule:\t" << r->getId() << "\t\"" << r->getName() << "\"\n";

	for(const auto v : asRange(vertices(dg))) {
		if(dg[v].kind != VertexKind::Edge) continue;
		const auto id = get(boost::vertex_index_t(), dg, v);
		s << "edge:\t" << id << "\t" << dg[v].rules.size();
		for(const auto *r : dg[v].rules) s << " " << r->getId();
		s << "\t";

		std::vector<int> coefs;
		for(const auto e : asRange(in_edges(v, dg))) {
			const auto v = source(e, dg);
			const auto id = 1 + get(boost::vertex_index_t(), dg, v);
			coefs.push_back(-id);
		}
		for(const auto e : asRange(out_edges(v, dg))) {
			const auto v = target(e, dg);
			const auto id = 1 + get(boost::vertex_index_t(), dg, v);
			coefs.push_back(id);
		}
		std::sort(begin(coefs), end(coefs));
		for(int coef : coefs) s << coef << " ";

		s << "\n";
	}
}

} // namespace Dump
} // namespace DG
} // namespace lib
} // namespace mod
