#include "Dump.h"

#include <mod/Graph.h>
#include <mod/Error.h>
#include <mod/Rule.h>
#include <mod/lib/DG/Hyper.h>
#include <mod/lib/Graph/Merge.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/Graph.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/ParserCommon.h>
#include <mod/lib/Rule/Shallow.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>
#include <jla_boost/Memory.hpp>

#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_difference.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_lexeme.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_uint.hpp>

using namespace mod::lib::IO::Parser;

namespace mod {
namespace lib {
namespace DG {
namespace Dump {
namespace {

struct ConstructionData {
	const std::vector<std::shared_ptr<mod::Rule> > &rules;
	std::vector<std::tuple<unsigned int, std::string, std::unique_ptr<lib::Graph::GraphType>, std::unique_ptr<lib::Graph::PropStringType> > > vertices;
	const std::vector<std::tuple<unsigned int, std::string> > rulesParsed;
	const std::vector < std::tuple<unsigned int, unsigned int, std::vector<int> > > edges;
};

struct NonHyperDump : public NonHyper {

	NonHyperDump(const std::vector<std::shared_ptr<mod::Graph> > &graphs,
			ConstructionData &&constructionData)
	: NonHyper(graphs), constructionData(&constructionData) {
		calculate();
	}
private:

	std::string getType() const {
		return "DGDump";
	}

	void calculateImpl() {
		const std::vector<std::shared_ptr<mod::Rule> > &rules = constructionData->rules;
		auto &vertices = constructionData->vertices;
		const auto &rulesParsed = constructionData->rulesParsed;
		const auto &edges = constructionData->edges;
		std::unordered_map<unsigned int, std::shared_ptr<mod::Rule> > ruleMap;
		std::unordered_map<unsigned int, std::shared_ptr<mod::Graph> > graphMap;
		for(const auto &t : rulesParsed) {
			auto iter = std::find_if(begin(rules), end(rules), [&t](std::shared_ptr<mod::Rule> r) -> bool {
				return r->getName() == get<1>(t);
			});
			std::shared_ptr<mod::Rule> r;
			if(iter == end(rules)) {
				auto rInternal = make_unique<lib::Rule::Shallow>();
				rInternal->setName(get<1>(t));
				r = mod::Rule::makeRule(std::move(rInternal));
			} else {
				r = *iter;
				IO::log() << "Rule linked: " << r->getName() << std::endl;
			}
			this->rules.push_back(r);
			ruleMap[get<0>(t)] = r;
		}

		// do merge of vertices and edges in order of increasing id
		unsigned int iVertices = 0;
		unsigned int iEdges = 0;
		for(unsigned int id = 0; id < vertices.size() + edges.size(); id++) {
			if(iVertices < vertices.size() && get<0>(vertices[iVertices]) == id) {
				auto &v = vertices[iVertices];
				auto p = checkIfNew(std::move(get<2>(v)), std::move(get<3>(v)));
				bool wasNew = addGraphAsVertex(p.first);
				graphMap[id] = p.first;
				if(!p.second) IO::log() << "Graph linked: " << get<1>(v) << " -> " << p.first->getName() << std::endl;
				if(wasNew) giveProductStatus(p.first);
				iVertices++;
			} else if(iEdges < edges.size() && get<0>(edges[iEdges]) == id) {
				const auto &e = edges[iEdges];
				auto mEduct = make_unique<lib::Graph::Merge>();
				auto mProduct = make_unique<lib::Graph::Merge>();
				for(int adj : get<2>(e)) {
					assert(adj != 0);
					unsigned int id = std::abs(adj) - 1;
					auto *m = adj < 0 ? mEduct.get() : mProduct.get();
					auto gIter = graphMap.find(id);
					assert(gIter != end(graphMap));
					m->mergeWith(gIter->second->getGraph());
				}
				mEduct->lock();
				mProduct->lock();
				auto rIter = ruleMap.find(get<1>(e));
				assert(rIter != end(ruleMap));
				const auto *r = &rIter->second->getBase();
				const lib::Graph::Base *gEduct, *gProduct;
				if(mEduct->getSingles().size() == 1) {
					gEduct = *begin(mEduct->getSingles());
				} else {
					gEduct = addToMergeStore(mEduct.release());
				}
				if(mProduct->getSingles().size() == 1) {
					gProduct = *begin(mProduct->getSingles());
				} else {
					gProduct = addToMergeStore(mProduct.release());
				}
				suggestDerivation(gEduct, gProduct, r);
				iEdges++;
			} else {
				MOD_ABORT;
			}
		}
		constructionData = nullptr;
	}

	void listImpl(std::ostream &s) const { }
private:
	std::vector<std::shared_ptr<mod::Rule> > rules;
	ConstructionData *constructionData;
};

template<typename Parser, typename Attr>
bool parse(IteratorType iterRealStart, IteratorType &iterStart, IteratorType iterEnd, Parser &parser, Attr &attr) {
	bool res = qi::phrase_parse(iterStart, iterEnd, parser, qi::space, attr);
	if(!res) {
		(lib::IO::Parser::ErrorHandler<IteratorType, true>(std::cout))(iterRealStart, iterEnd, iterStart, "Error in DG dump file");
	}
	return res;
}

} // namespace

NonHyper *load(const std::vector<std::shared_ptr<mod::Graph> > &graphs, const std::vector<std::shared_ptr<mod::Rule> > &rules, std::istream &s, std::ostream &err) {

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
	IteratorType iterStart = spirit::make_default_multi_pass(BaseIteratorType(s));
	IteratorType iterRealStart = iterStart;
	IteratorType iterEnd;

	unsigned int numVertices, numEdges, numRules;
	std::vector<std::tuple<unsigned int, std::string, std::unique_ptr<lib::Graph::GraphType>, std::unique_ptr<lib::Graph::PropStringType> > > vertices;
	std::vector<std::tuple<unsigned int, std::string> > rulesParsed;
	std::vector<std::tuple<unsigned int, unsigned int, std::vector<int> > > edges;

	std::unordered_set<unsigned int> validVertices, validRules;
#define PARSE(p, a) if(!parse(iterRealStart, iterStart, iterEnd, p, a)) return nullptr
	PARSE("numVertices:" >> qi::uint_, numVertices);
	PARSE("numEdges:" >> qi::uint_, numEdges);
	PARSE("numRules:" >> qi::uint_, numRules);
	for(unsigned int i = 0; i < numVertices; i++) {
		unsigned int id;
		std::string name, dfs;
		PARSE("vertex:" >> qi::uint_, id);
		PARSE('"' >> qi::lexeme[*(qi::char_ - '"') >> '"'], name);
		PARSE('"' >> qi::lexeme[*(qi::char_ - '"') >> '"'], dfs);
		auto gData = IO::Graph::Read::dfs(dfs, err);
		if(!gData.first) {
			err << "GraphDFS \"" << dfs << "\" could not be parsed for vertex " << id << std::endl;
			return nullptr;
		}
		vertices.emplace_back(id, name, std::move(gData.first), std::move(gData.second));
		validVertices.insert(id);
	}
	for(unsigned int i = 0; i < numRules; i++) {
		unsigned int id;
		std::string name;
		PARSE("rule:" >> qi::uint_, id);
		PARSE('"' >> qi::lexeme[*(qi::char_ - '"') >> '"'], name);
		rulesParsed.emplace_back(id, name);
		if(validRules.find(id) != end(validRules)) {
			err << "Parsed data is corrupt, duplicate rule id, " << id << std::endl;
			return nullptr;
		}
		validRules.insert(id);
	}
	for(unsigned int i = 0; i < numEdges; i++) {
		unsigned int id, ruleId;
		std::vector<int> adj;
		PARSE("edge:" >> qi::uint_, id);
		PARSE(qi::uint_, ruleId);
		PARSE(*qi::int_, adj);
		if(validRules.find(ruleId) == end(validRules)) {
			err << "Parsed data is corrupt, ruleId, " << ruleId << ", out of range [0, " << numRules << "[ for edge " << id << std::endl;
			return nullptr;
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
		edges.emplace_back(id, ruleId, adj);
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

void write(const NonHyper &dgNonHyper, std::ostream &s) {
	using Vertex = lib::DG::HyperVertex;
	using Edge = lib::DG::HyperEdge;
	using VertexKind = lib::DG::HyperVertexKind;
	const lib::DG::HyperGraphType &dg = dgNonHyper.getHyper().getGraph();
	unsigned int numVertices = 0, numEdges = 0;

	for(Vertex v : asRange(vertices(dg))) {
		if(dg[v].kind == VertexKind::Vertex) numVertices++;
		else numEdges++;
	}

	std::set<const lib::Rule::Base*, lib::Rule::Less> rules;

	for(Vertex v : asRange(vertices(dg))) {
		if(dg[v].kind != VertexKind::Edge) continue;
		assert(dg[v].rules.size() == 1); // TODO: we don't need to do something special here, right?
		for(auto *r : dg[v].rules) rules.insert(r);
	}

	s << "numVertices:\t" << numVertices << std::endl;
	s << "numEdges:\t" << numEdges << std::endl;
	s << "numRules:\t" << rules.size() << std::endl;

	for(Vertex v : asRange(vertices(dg))) {
		if(dg[v].kind != VertexKind::Vertex) continue;
		unsigned int id = get(boost::vertex_index_t(), dg, v);
		const lib::Graph::Single *g = dg[v].graph;
		assert(g);
		s << "vertex:\t" << id << "\t\"" << g->getName() << "\"\t\"" << g->getGraphDFS().first << "\"" << std::endl;
	}


	for(const lib::Rule::Base *r : rules) s << "rule:\t" << r->getId() << "\t\"" << r->getName() << "\"" << std::endl;

	for(Vertex v : asRange(vertices(dg))) {
		if(dg[v].kind != VertexKind::Edge) continue;
		unsigned int id = get(boost::vertex_index_t(), dg, v);
		assert(dg[v].rules.size() == 1);
		const lib::Rule::Base *r = dg[v].rules.front();
		s << "edge:\t" << id << "\t" << r->getId() << "\t";

		std::vector<int> coefs;

		for(Edge e : asRange(in_edges(v, dg))) {
			Vertex v = source(e, dg);
			unsigned int id = 1 + get(boost::vertex_index_t(), dg, v);
			coefs.push_back(-id);
		}

		for(Edge e : asRange(out_edges(v, dg))) {
			Vertex v = target(e, dg);
			unsigned int id = 1 + get(boost::vertex_index_t(), dg, v);
			coefs.push_back(id);
		}

		std::sort(begin(coefs), end(coefs));

		for(int coef : coefs) s << coef << " ";

		s << std::endl;
	}
}

} // namespace Dump
} // namespace DG
} // namespace lib
} // namespace mod
