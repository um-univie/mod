#include "Rule.h"

#include <mod/lib/IO/GMLParserCommon.h>

#include <jla_boost/Memory.hpp>

#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_permutation.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_uint.hpp>

#include <unordered_map>
#include <unordered_set>

using namespace mod::lib::IO::Parser;

namespace mod {
namespace lib {
namespace IO {
namespace Rule {
namespace Read {

// The GML parser is based on the rule GML parser in GGL

namespace {

template<typename Iter, template<typename> class Skipper>
struct Parser : public qi::grammar<Iter, Skipper<Iter>, GML::Rule()> {

	Parser(std::ostream &s) : Parser::base_type(start), errorHandler(s) {
		start %= qi::eps > qi::lit("rule") > '['
				> -ruleID
				> -left > -context > -right
				> matchConstraints
				;
		start.name("ruleStatement");
		ruleID %= qi::lit("ruleID") > escapedString;
		ruleID.name("ruleIDStatement");
		left %= qi::lit("left") > vertexAndEdgeList;
		left.name("leftStatement");
		context %= qi::lit("context") > vertexAndEdgeList;
		context.name("contextStatement");
		right %= qi::lit("right") > vertexAndEdgeList;
		right.name("rightStatement");
		matchConstraints %= qi::lit(']') | (matchConstraint > matchConstraints);
		matchConstraints.name("matchConstraint");
		matchConstraint %= qi::lit("constrainAdj") > '[' > (
				idStatement
				^ opStatement
				^ countStatement
				^ nodeLabelsStatement
				^ edgeLabelsStatement
				) >> ']';
		matchConstraint.name("matchConstraint");
		idStatement %= "id" > qi::int_;
		idStatement.name("idStatement");
		opStatement %= qi::lit("op") > relOp;
		opStatement.name("opStatement");
		relOp %= qi::string("<") | qi::string("<=")
				| qi::string("=")
				| qi::string(">=") | qi::string(">");
		relOp.name("relationalOperator");
		countStatement %= "count" > qi::uint_;
		countStatement.name("countStatement");
		nodeLabelsStatement %= qi::lit("nodeLabels") > '[' > *(qi::lit("label") > escapedString) > ']';
		nodeLabelsStatement.name("nodeLabelsStatement");
		edgeLabelsStatement %= qi::lit("edgeLabels") > '[' > *(qi::lit("label") > escapedString) > ']';
		edgeLabelsStatement.name("edgeLabelsStatement");
		qi::on_error<qi::fail>(start, errorHandler(qi::_1, qi::_2, qi::_3, qi::_4));
	}
private:
	qi::rule<Iter, Skipper<Iter>, GML::Rule() > start;
	qi::rule<Iter, Skipper<Iter>, std::string() > ruleID;
	EscapedString<'"', '"', Iter> escapedString;
	qi::rule<Iter, Skipper<Iter>, GML::Graph() > left, context, right;
	GML::VertexAndEdgeList<Iter, Skipper> vertexAndEdgeList;
	qi::rule<Iter, Skipper<Iter>, std::vector<GML::MatchConstraint>() > matchConstraints;
	qi::rule<Iter, Skipper<Iter>, GML::MatchConstraint() > matchConstraint;
	qi::rule<Iter, Skipper<Iter>, int() > idStatement;
	qi::rule<Iter, Skipper<Iter>, std::string() > opStatement;
	qi::rule<Iter, Skipper<Iter>, std::string() > relOp;
	qi::rule<Iter, Skipper<Iter>, unsigned int() > countStatement;
	qi::rule<Iter, Skipper<Iter>, std::vector<std::string>() > nodeLabelsStatement, edgeLabelsStatement;
	phx::function<ErrorHandler<Iter> > errorHandler;
};

Data parseGML(std::istream &s, std::ostream &err) {
	Data data;

	struct FlagsHolder {

		FlagsHolder(std::istream &s) : s(s), flags(s.flags()) { }

		~FlagsHolder() {
			s.flags(flags);
		}
	private:
		std::istream &s;
		std::ios::fmtflags flags;
	} flagsHolder(s);
	s.unsetf(std::ios::skipws);
	IteratorType iterStart = spirit::make_default_multi_pass(BaseIteratorType(s));
	IteratorType iterEnd;
	Parser<IteratorType, GML::Skipper> parser(err);
	GML::Rule rule;
	bool res = lib::IO::Parser::phrase_parse(err, iterStart, iterEnd, parser, GML::Skipper<IteratorType>(), rule);
	if(!res) return data;

	auto checkSide = [&err](const GML::Graph &side, std::string name) {
		std::unordered_set<int> vertexIds;
		for(const GML::Vertex &v : side.vertices) {
			if(vertexIds.find(v.id) != end(vertexIds)) {
				err << "Duplicate vertex " << v.id << " in " << name << " graph.\n";
				return false;
			}
			vertexIds.insert(v.id);
		}
		std::set<std::pair<int, int> > edgeIds;
		for(const GML::Edge &e : side.edges) {
			if(edgeIds.find(std::minmax(e.source, e.target)) != end(edgeIds)) {
				err << "Duplicate edge (" << e.source << ", " << e.target << ") in " << name << " graph.\n";
				return false;
			}
		}
		return true;
	};
	if(!checkSide(rule.left, "left")) return data;
	if(!checkSide(rule.context, "context")) return data;
	if(!checkSide(rule.right, "right")) return data;

	using Vertex = lib::Rule::Vertex;
	using Edge = lib::Rule::Edge;
	data.name = rule.id;
	lib::Rule::LabelledRule dpoResult;
	lib::Rule::GraphType &g = get_graph(dpoResult);
	dpoResult.pString = make_unique<lib::Rule::PropStringCore>(g);
	auto &pString = *dpoResult.pString;

	struct VertexLabels {
		boost::optional<std::string> left, context, right;
		Vertex v;
	};
	std::map<int, VertexLabels> idMapVertex;
	for(const GML::Vertex &vGML : rule.left.vertices) {
		assert(!idMapVertex[vGML.id].left);
		idMapVertex[vGML.id].left = vGML.label;
	}
	for(const GML::Vertex &vGML : rule.context.vertices) {
		assert(!idMapVertex[vGML.id].context);
		idMapVertex[vGML.id].context = vGML.label;
	}
	for(const GML::Vertex &vGML : rule.right.vertices) {
		assert(!idMapVertex[vGML.id].right);
		idMapVertex[vGML.id].right = vGML.label;
	}
	for(auto &p : idMapVertex) {
		int id = p.first;
		if(p.second.left && p.second.context) {
			err << "Error in rule GML. Vertex " << id << " defined both in 'left' and 'context'." << std::endl;
			return data;
		}
		if(p.second.right && p.second.context) {
			err << "Error in rule GML. Vertex " << id << " defined both in 'context' and 'right'." << std::endl;
			return data;
		}
		if(p.second.left && p.second.right) {
			// check if same label
			if(p.second.left.get() == p.second.right.get()) {
				err << "Error in rule GML. Vertex " << id << " defined both in 'left' and 'right', but with the same label. It should be in 'context' instead." << std::endl;
				return data;
			}
		}
		p.second.v = add_vertex(g);
		if(p.second.context) {
			g[p.second.v].membership = lib::Rule::Membership::Context;
			pString.add(p.second.v, p.second.context.get(), p.second.context.get());
		} else if(p.second.left && p.second.right) {
			g[p.second.v].membership = lib::Rule::Membership::Context;
			pString.add(p.second.v, p.second.left.get(), p.second.right.get());
		} else if(p.second.left) {
			g[p.second.v].membership = lib::Rule::Membership::Left;
			pString.add(p.second.v, p.second.left.get(), "");
		} else {
			assert(p.second.right);
			g[p.second.v].membership = lib::Rule::Membership::Right;
			pString.add(p.second.v, "", p.second.right.get());
		}
	}

	struct EdgeLabels {
		boost::optional<std::string> left, context, right;
		Edge e;
	};
	std::map<std::pair<int, int>, EdgeLabels> idMapEdge;
	for(const GML::Edge &eGML : rule.left.edges) {
		auto eSorted = std::minmax(eGML.source, eGML.target);
		assert(!idMapEdge[eSorted].left);
		idMapEdge[eSorted].left = eGML.label;
	}
	for(const GML::Edge &eGML : rule.context.edges) {
		auto eSorted = std::minmax(eGML.source, eGML.target);
		assert(!idMapEdge[eSorted].context);
		idMapEdge[eSorted].context = eGML.label;
	}
	for(const GML::Edge &eGML : rule.right.edges) {
		auto eSorted = std::minmax(eGML.source, eGML.target);
		assert(!idMapEdge[eSorted].right);
		idMapEdge[eSorted].right = eGML.label;
	}

	for(const auto &p : idMapEdge) {
		int src = p.first.first;
		int tar = p.first.second;
		if(idMapVertex.find(src) == end(idMapVertex)) {
			err << "Error in rule GML. Edge endpoint '" << src << "' does not exist for edge (" << src << ", " << tar << ").\n";
			return data;
		}
		if(idMapVertex.find(tar) == end(idMapVertex)) {
			err << "Error in rule GML. Edge endpoint '" << tar << "' does not exist for edge (" << src << ", " << tar << ").\n";
			return data;
		}
		Vertex vSrc = idMapVertex[src].v, vTar = idMapVertex[tar].v;
		if(p.second.left && p.second.context) {
			err << "Error in rule GML. Edge (" << src << ", " << tar << ") defined both in 'left' and 'context'.\n";
			return data;
		}
		if(p.second.right && p.second.context) {
			err << "Error in rule GML. Edge (" << src << ", " << tar << ") defined both in 'context' and 'right'.\n";
			return data;
		}
		if(p.second.left && p.second.right) {
			// check if same label
			if(p.second.left.get() == p.second.right.get()) {
				err << "Error in rule GML. Edge (" << src << ", " << tar << ") defined both in 'left' and 'right', but with the same label. It should be in 'context' instead." << std::endl;
				return data;
			}
		}
		Edge e = add_edge(vSrc, vTar, g).first;
		if(p.second.context) {
			g[e].membership = lib::Rule::Membership::Context;
			pString.add(e, p.second.context.get(), p.second.context.get());
		} else if(p.second.left && p.second.right) {
			g[e].membership = lib::Rule::Membership::Context;
			pString.add(e, p.second.left.get(), p.second.right.get());
		} else if(p.second.left) {
			g[e].membership = lib::Rule::Membership::Left;
			pString.add(e, p.second.left.get(), "");
		} else {
			assert(p.second.right);
			g[e].membership = lib::Rule::Membership::Right;
			pString.add(e, "", p.second.right.get());
		}
	}

	// constraints
	for(const GML::MatchConstraint &cGML : rule.matchConstraints) {
		auto iter = idMapVertex.find(cGML.id);
		if(iter == end(idMapVertex)) {
			err << "Error in rule GML. Vertex " << cGML.id << " in constraint does not exist." << std::endl;
			return data;
		}
		Vertex vConstrained = iter->second.v;
		auto opFromString = [](const std::string & s) {
			using namespace lib::GraphMorphism::MatchConstraint;
			if(s == "<") return Operator::LT;
			else if(s == "<=") return Operator::LEQ;
			else if(s == "=") return Operator::EQ;
			else if(s == ">=") return Operator::GEQ;
			else if(s == ">") return Operator::GT;
			else MOD_ABORT;
		};
		auto c = make_unique<lib::GraphMorphism::MatchConstraint::VertexAdjacency<lib::Rule::GraphType> >(
				vConstrained, opFromString(cGML.op), cGML.count);
		for(const auto &s : cGML.nodeLabels) c->vertexLabels.insert(s);
		for(const auto &s : cGML.edgeLabels) c->edgeLabels.insert(s);
		dpoResult.leftComponentMatchConstraints.push_back(std::move(c));
	}
	data.rule = std::move(dpoResult);
	return data;
}

} // namespace

Data gml(std::istream &s, std::ostream &err) {
	return parseGML(s, err);
}

} // namespace Read
} // namespace Rule
} // namespace IO
} // namespace lib
} // namespace mod
