#include "Rule.h"

#include <mod/lib/IO/GMLUtils.h>

#include <gml/parser.hpp>
#include <gml/converter.hpp>
#include <gml/converter_edsl.hpp>

#include <unordered_map>
#include <unordered_set>

namespace mod {
namespace lib {
namespace IO {
namespace Rules {
namespace Read {
namespace {

template<typename T>
struct Label {
	boost::optional<T> left, context, right;
};

Data parseGML(std::istream &s, std::ostream &err) {
	Data data;
	GML::Rule rule;
	{
		gml::ast::List ast;
		bool res = gml::parser::parse(s, ast, err);
		if(!res) return data;
		using namespace gml::converter::edsl;
		auto cVertex = list<GML::Vertex>("node", &GML::Graph::vertices)
				(int_("id", &GML::Vertex::id), 1, 1)
				(string("label", &GML::Vertex::label), 0, 1);
		auto cEdge = list<GML::Edge>("edge", &GML::Graph::edges)
				(int_("source", &GML::Edge::source), 1, 1)
				(int_("target", &GML::Edge::target), 1, 1)
				(string("label", &GML::Edge::label), 0, 1);
		auto nodeLabels = list<Parent>("nodeLabels")
				(string("label", &GML::AdjacencyConstraint::nodeLabels))
				;
		auto edgeLabels = list<Parent>("edgeLabels")
				(string("label", &GML::AdjacencyConstraint::edgeLabels))
				;
		auto constrainAdj = list<GML::AdjacencyConstraint>("constrainAdj", &GML::Rule::matchConstraints)
				(int_("id", &GML::AdjacencyConstraint::id), 1, 1)
				(string("op", &GML::AdjacencyConstraint::op), 1, 1)
				(int_("count", &GML::AdjacencyConstraint::count), 1, 1)
				(nodeLabels) (edgeLabels)
				;
		auto constrainShortestPath = list<GML::ShortestPathConstraint>("constrainShortestPath", &GML::Rule::matchConstraints)
				(int_("source", &GML::ShortestPathConstraint::source), 1, 1)
				(int_("target", &GML::ShortestPathConstraint::target), 1, 1)
				(string("op", &GML::ShortestPathConstraint::op), 1, 1)
				(int_("length", &GML::ShortestPathConstraint::length), 1, 1)
				;
		auto makeSide = [&](std::string name, GML::Graph GML::Rule::*side) {
			return list<GML::Graph>(name, side)(cVertex) (cEdge);
		};
		auto cRule = list<Parent>("rule")
				(string("ruleID", &GML::Rule::id), 0, 1)
				(makeSide("left", &GML::Rule::left), 0, 1)
				(makeSide("context", &GML::Rule::context), 0, 1)
				(makeSide("right", &GML::Rule::right), 0, 1)
				(constrainAdj) (constrainShortestPath)
				;
		auto iterBegin = ast.list.begin();
		res = gml::converter::convert(iterBegin, ast.list.end(), cRule, err, rule);
		if(!res) return data;
	}

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

	using Vertex = lib::Rules::Vertex;
	using Edge = lib::Rules::Edge;
	data.name = rule.id;
	lib::Rules::LabelledRule dpoResult;
	lib::Rules::GraphType &g = get_graph(dpoResult);
	dpoResult.pString = std::make_unique<lib::Rules::PropStringCore>(g);
	auto &pString = *dpoResult.pString;

	struct VertexLabels {
		bool inLeft = false, inContext = false, inRight = false;
		Label<std::string> string;
	public:
		Vertex v;
	};

	struct EdgeLabels {
		bool inLeft = false, inContext = false, inRight = false;
		Label<std::string> string;
	public:
		Edge e;
	};
	std::map<int, VertexLabels> idMapVertex;
	std::map<std::pair<int, int>, EdgeLabels> idMapEdge;
	for(const GML::Vertex &vGML : rule.left.vertices) {
		auto &v = idMapVertex[vGML.id];
		v.inLeft = true;
		v.string.left = vGML.label;
	}
	for(const GML::Vertex &vGML : rule.context.vertices) {
		auto &v = idMapVertex[vGML.id];
		v.inContext = true;
		v.string.context = vGML.label;
	}
	for(const GML::Vertex &vGML : rule.right.vertices) {
		auto &v = idMapVertex[vGML.id];
		v.inRight = true;
		v.string.right = vGML.label;
	}
	for(const GML::Edge &eGML : rule.left.edges) {
		auto eSorted = std::minmax(eGML.source, eGML.target);
		auto &e = idMapEdge[eSorted];
		e.inLeft = true;
		e.string.left = eGML.label;
	}
	for(const GML::Edge &eGML : rule.context.edges) {
		auto eSorted = std::minmax(eGML.source, eGML.target);
		auto &e = idMapEdge[eSorted];
		e.inContext = true;
		e.string.context = eGML.label;
	}
	for(const GML::Edge &eGML : rule.right.edges) {
		auto eSorted = std::minmax(eGML.source, eGML.target);
		auto &e = idMapEdge[eSorted];
		e.inRight = true;
		e.string.right = eGML.label;
	}

	for(auto &p : idMapVertex) {
		int id = p.first;
		auto &vData = p.second;
		// First find the right membership:
		// inContext <=> inLeft && inRight
		if(vData.inContext) vData.inLeft = vData.inRight = true;
		else if(vData.inLeft && vData.inRight) vData.inContext = true;
		// Check string labels and make (left, right) the correct labels
		if(vData.string.context) {
			if(vData.string.left) {
				err << "Error in rule GML. Vertex " << id << " has a label both in 'context' and 'left'." << std::endl;
				return data;
			}
			if(vData.string.right) {
				err << "Error in rule GML. Vertex " << id << " has a label both in 'context' and 'left'." << std::endl;
				return data;
			}
			vData.string.left = vData.string.right = vData.string.context;
		}
		// Check that there is a string in left/right when inLeft/inRight
		if(vData.inLeft && !vData.string.left) {
			err << "Error in rule GML. Vertex " << id << " is in L, but has no label." << std::endl;
			return data;
		}
		if(vData.inRight && !vData.string.right) {
			err << "Error in rule GML. Vertex " << id << " is in R, but has no label." << std::endl;
			return data;
		}

		vData.v = add_vertex(g);
		if(vData.inContext) {
			put_membership(dpoResult, vData.v, lib::Rules::Membership::Context);
			pString.add(vData.v, vData.string.left.get(), vData.string.right.get());
		} else if(vData.inLeft) {
			assert(!vData.inRight);
			put_membership(dpoResult, vData.v, lib::Rules::Membership::Left);
			pString.add(vData.v, vData.string.left.get(), "");
		} else {
			assert(vData.inRight);
			put_membership(dpoResult, vData.v, lib::Rules::Membership::Right);
			pString.add(vData.v, "", vData.string.right.get());
		}
	}
	for(auto &p : idMapEdge) {
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
		auto &eData = p.second;
		// First find the right membership:
		// inContext <=> inLeft && inRight
		if(eData.inContext) eData.inLeft = eData.inRight = true;
		else if(eData.inLeft && eData.inRight) eData.inContext = true;
		// Check string labels and make (left, right) the correct labels
		if(eData.string.context) {
			if(eData.string.left) {
				err << "Error in rule GML. Edge (" << src << ", " << tar << ") has a label both in 'context' and 'left'." << std::endl;
				return data;
			}
			if(eData.string.right) {
				err << "Error in rule GML. Edge (" << src << ", " << tar << ") has a label both in 'context' and 'left'." << std::endl;
				return data;
			}
			eData.string.left = eData.string.right = eData.string.context;
		}
		// Check that there is a string in left/right when inLeft/inRight
		if(eData.inLeft && !eData.string.left) {
			err << "Error in rule GML. Edge (" << src << ", " << tar << ") is in L, but has no label." << std::endl;
			return data;
		}
		if(eData.inRight && !eData.string.right) {
			err << "Error in rule GML. Edge (" << src << ", " << tar << ") is in R, but has no label." << std::endl;
			return data;
		}

		eData.e = add_edge(vSrc, vTar, g).first;
		if(eData.inContext) {
			put_membership(dpoResult, eData.e, lib::Rules::Membership::Context);
			pString.add(eData.e, eData.string.left.get(), eData.string.right.get());
		} else if(eData.inLeft) {
			assert(!eData.inRight);
			put_membership(dpoResult, eData.e, lib::Rules::Membership::Left);
			pString.add(eData.e, eData.string.left.get(), "");
		} else {
			assert(eData.inRight);
			put_membership(dpoResult, eData.e, lib::Rules::Membership::Right);
			pString.add(eData.e, "", eData.string.right.get());
		}
	}
	// the graph is set, so initialise the component storage
	dpoResult.initComponents();

	// constraints
	for(const GML::MatchConstraint &cGML : rule.matchConstraints) {

		struct MatchConstraintConverter : boost::static_visitor<bool> {

			MatchConstraintConverter(lib::Rules::LabelledRule &dpoResult, const std::map<int, VertexLabels> &idMapVertex, std::ostream &err)
			: dpoResult(dpoResult), idMapVertex(idMapVertex), err(err) { }

			bool operator()(const GML::AdjacencyConstraint &cGML) {
				auto iter = idMapVertex.find(cGML.id);
				if(iter == end(idMapVertex)) {
					err << "Error in rule GML. Vertex " << cGML.id << " in adjacency constraint does not exist." << std::endl;
					return false;
				}
				Vertex vConstrained = iter->second.v;
				lib::GraphMorphism::MatchConstraint::Operator op;
				{
					const auto &s = cGML.op;
					using namespace lib::GraphMorphism::MatchConstraint;
					if(s == "<") op = Operator::LT;
					else if(s == "<=") op = Operator::LEQ;
					else if(s == "=") op = Operator::EQ;
					else if(s == ">=") op = Operator::GEQ;
					else if(s == ">") op = Operator::GT;
					else {
						err << "Error in rule GML. Unknown operator '" << s << "' in adjacency constraint." << std::endl;
						return false;
					}
				}
				auto c = std::make_unique<lib::GraphMorphism::MatchConstraint::VertexAdjacency<lib::Rules::LabelledRule::LeftGraphType> >(
						vConstrained, op, cGML.count);
				for(const auto &s : cGML.nodeLabels) c->vertexLabels.insert(s);
				for(const auto &s : cGML.edgeLabels) c->edgeLabels.insert(s);
				dpoResult.leftComponentMatchConstraints.push_back(std::move(c));
				return true;
			}

			bool operator()(const GML::ShortestPathConstraint &cGML) {
				auto iterSrc = idMapVertex.find(cGML.source);
				auto iterTar = idMapVertex.find(cGML.target);
				if(iterSrc == end(idMapVertex)) {
					err << "Error in rule GML. Vertex " << cGML.source << " in shortest path constraint does not exist." << std::endl;
					return false;
				}
				if(iterTar == end(idMapVertex)) {
					err << "Error in rule GML. Vertex " << cGML.target << " in shortest path constraint does not exist." << std::endl;
					return false;
				}
				Vertex vSrc = iterSrc->second.v;
				Vertex vTar = iterTar->second.v;
				lib::GraphMorphism::MatchConstraint::Operator op;
				{
					const auto &s = cGML.op;
					using namespace lib::GraphMorphism::MatchConstraint;
					if(s == "<") op = Operator::LT;
					else if(s == "<=") op = Operator::LEQ;
					else if(s == "=") op = Operator::EQ;
					else if(s == ">=") op = Operator::GEQ;
					else if(s == ">") op = Operator::GT;
					else {
						err << "Error in rule GML. Unknown operator '" << s << "' in shortest path constraint." << std::endl;
						return false;
					}
				}
				auto compSrc = dpoResult.leftComponents[get(boost::vertex_index_t(), get_graph(dpoResult), vSrc)];
				auto compTar = dpoResult.leftComponents[get(boost::vertex_index_t(), get_graph(dpoResult), vTar)];
				if(compSrc != compTar) {
					err << "Error in rule GML. Vertex " << cGML.source << " and " << cGML.target << " are in different connected components of the left graph. "
							<< "This is currently not supported for the shortest path constraint." << std::endl;
					return false;
				}
				auto c = std::make_unique<lib::GraphMorphism::MatchConstraint::ShortestPath<lib::Rules::LabelledRule::LeftGraphType> >(
						vSrc, vTar, op, cGML.length);
				dpoResult.leftComponentMatchConstraints.push_back(std::move(c));
				return true;
			}
		public:
			lib::Rules::LabelledRule &dpoResult;
			const std::map<int, VertexLabels> &idMapVertex;
			std::ostream &err;
		} visitor(dpoResult, idMapVertex, err);
		bool success = boost::apply_visitor(visitor, cGML);
		if(!success) return data;
	}
	data.rule = std::move(dpoResult);
	return data;
}

} // namespace

Data gml(std::istream &s, std::ostream &err) {
	return parseGML(s, err);
}

} // namespace Read
} // namespace Rules
} // namespace IO
} // namespace lib
} // namespace mod
