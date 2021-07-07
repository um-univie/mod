#include "Rule.hpp"

#include <mod/lib/GraphMorphism/Constraints/ShortestPath.hpp>
#include <mod/lib/GraphMorphism/Constraints/VertexAdjacency.hpp>
#include <mod/lib/IO/GMLUtils.hpp>
#include <mod/lib/Rules/Properties/Molecule.hpp>
#include <mod/lib/Rules/Properties/Stereo.hpp>
#include <mod/lib/Stereo/Inference.hpp>

#include <gml/parser.hpp>
#include <gml/converter.hpp>
#include <gml/converter_edsl.hpp>

#include <unordered_map>
#include <unordered_set>

namespace mod::lib::IO::Rules::Read {
namespace {

template<typename T>
struct Label {
	std::optional<T> left, context, right;
};

} // namespace

Result<Data> gml(lib::IO::Warnings &warnings, std::string_view input) {
	GML::Rule rule;
	{ // GML parsing and conversion
		gml::ast::KeyValue ast;
		try {
			ast = gml::parser::parse(input);
		} catch(const gml::parser::error &e) {
			return Result<>::Error(e.what());
		}
		using namespace gml::converter::edsl;
		auto cVertex = GML::makeVertexConverter(0);
		auto cEdge = GML::makeEdgeConverter(0);
		auto nodeLabels = list<Parent>("nodeLabels")
				(string("label", &GML::AdjacencyConstraint::nodeLabels));
		auto edgeLabels = list<Parent>("edgeLabels")
				(string("label", &GML::AdjacencyConstraint::edgeLabels));
		auto constrainAdj = list<GML::AdjacencyConstraint>("constrainAdj", &GML::Rule::matchConstraints)
				(int_("id", &GML::AdjacencyConstraint::id), 1, 1)
				(string("op", &GML::AdjacencyConstraint::op), 1, 1)
				(int_("count", &GML::AdjacencyConstraint::count), 1, 1)
				(nodeLabels)(edgeLabels);
		auto constrainShortestPath = list<GML::ShortestPathConstraint>("constrainShortestPath",
		                                                               &GML::Rule::matchConstraints)
				(int_("source", &GML::ShortestPathConstraint::source), 1, 1)
				(int_("target", &GML::ShortestPathConstraint::target), 1, 1)
				(string("op", &GML::ShortestPathConstraint::op), 1, 1)
				(int_("length", &GML::ShortestPathConstraint::length), 1, 1);
		auto makeSide = [&](std::string name, GML::Graph GML::Rule::*side) {
			return list<GML::Graph>(name, side)(cVertex)(cEdge);
		};
		auto cRule = list<Parent>("rule")
				(string("ruleID", &GML::Rule::id), 0, 1)
				(string("labelType", &GML::Rule::labelType), 0, 1)
				(makeSide("left", &GML::Rule::left), 0, 1)
				(makeSide("context", &GML::Rule::context), 0, 1)
				(makeSide("right", &GML::Rule::right), 0, 1)
				(constrainAdj)(constrainShortestPath);
		auto iterBegin = &ast;
		auto iterEnd = iterBegin + 1;
		try {
			gml::converter::convert(iterBegin, iterEnd, cRule, rule);
		} catch(const gml::converter::error &e) {
			return Result<>::Error(e.what());
		}
	} // end of GML parsing

	const auto checkSide = [](const GML::Graph &side, std::string name) -> Result<> {
		std::unordered_set<int> vertexIds;
		for(const GML::Vertex &v : side.vertices) {
			if(vertexIds.find(v.id) != end(vertexIds))
				return Result<>::Error("Duplicate vertex " + std::to_string(v.id) + " in " + name + " graph.");
			vertexIds.insert(v.id);
		}
		std::set<std::pair<int, int> > edgeIds;
		for(const GML::Edge &e : side.edges) {
			if(e.source == e.target)
				return Result<>::Error("Loop edge (on " + std::to_string(e.source) + ", in " + name + ") is not allowed.");
			auto eSorted = std::minmax(e.source, e.target);
			if(edgeIds.find(eSorted) != end(edgeIds))
				return Result<>::Error(
						"Duplicate edge (" + std::to_string(e.source) + ", " + std::to_string(e.target) + ") in "
						+ name + " graph.");
			edgeIds.insert(eSorted);
		}
		return Result<>();
	};

	if(auto res = checkSide(rule.left, "left"); !res) return res;
	if(auto res = checkSide(rule.context, "context"); !res) return res;
	if(auto res = checkSide(rule.right, "right"); !res) return res;

	using Vertex = lib::Rules::Vertex;
	using Edge = lib::Rules::Edge;
	Data data;
	data.rule = lib::Rules::LabelledRule();
	data.name = rule.id;
	if(rule.labelType) {
		const std::string &ltString = *rule.labelType;
		if(ltString == "string") data.labelType = LabelType::String;
		else if(ltString == "term") data.labelType = LabelType::Term;
		else return Result<>::Error("Error in rule GML. Unknown labelType '" + ltString + "'.");
	}

	auto &dpoResult = *data.rule;
	auto &g = get_graph(dpoResult);
	dpoResult.pString = std::make_unique<lib::Rules::PropStringCore>(g);
	auto &pString = *dpoResult.pString;

	struct VertexLabels {
		bool inLeft = false, inContext = false, inRight = false;
		Label<std::string> string, stereo;
	public:
		Vertex v;
		std::optional<lib::IO::Stereo::Read::ParsedEmbedding> parsedEmbeddingLeft, parsedEmbeddingRight;
	};

	struct EdgeLabels {
		bool inLeft = false, inContext = false, inRight = false;
		Label<std::string> string, stereo;
	public:
		Edge e;
	};
	std::map<int, VertexLabels> idMapVertex;
	std::map<Vertex, int> vertexMapId;
	std::map<std::pair<int, int>, EdgeLabels> idMapEdge;
	for(const GML::Vertex &vGML : rule.left.vertices) {
		auto &v = idMapVertex[vGML.id];
		v.inLeft = true;
		v.string.left = vGML.label;
		v.stereo.left = vGML.stereo;
	}
	for(const GML::Vertex &vGML : rule.context.vertices) {
		auto &v = idMapVertex[vGML.id];
		v.inContext = true;
		v.string.context = vGML.label;
		v.stereo.context = vGML.stereo;
	}
	for(const GML::Vertex &vGML : rule.right.vertices) {
		auto &v = idMapVertex[vGML.id];
		v.inRight = true;
		v.string.right = vGML.label;
		v.stereo.right = vGML.stereo;
	}
	for(const GML::Edge &eGML : rule.left.edges) {
		auto eSorted = std::minmax(eGML.source, eGML.target);
		auto &e = idMapEdge[eSorted];
		e.inLeft = true;
		e.string.left = eGML.label;
		e.stereo.left = eGML.stereo;
	}
	for(const GML::Edge &eGML : rule.context.edges) {
		auto eSorted = std::minmax(eGML.source, eGML.target);
		auto &e = idMapEdge[eSorted];
		e.inContext = true;
		e.string.context = eGML.label;
		e.stereo.context = eGML.stereo;
	}
	for(const GML::Edge &eGML : rule.right.edges) {
		auto eSorted = std::minmax(eGML.source, eGML.target);
		auto &e = idMapEdge[eSorted];
		e.inRight = true;
		e.string.right = eGML.label;
		e.stereo.right = eGML.stereo;
	}

	for(auto &p : idMapVertex) {
		int id = p.first;
		auto &vData = p.second;
		// First find the right membership:
		// inContext <=> inLeft && inRight
		if(vData.inContext) vData.inLeft = vData.inRight = true;
		else if(vData.inLeft && vData.inRight) vData.inContext = true;

		// Check labels and make (left, right) the correct labels
		if(vData.string.context) {
			if(vData.string.left)
				return Result<>::Error(
						"Error in rule GML. Vertex " + std::to_string(id) + " has a label both in 'context' and 'left'.");
			if(vData.string.right)
				return Result<>::Error(
						"Error in rule GML. Vertex " + std::to_string(id) + " has a label both in 'context' and 'right'.");
			// Note: terms follow the same semantics as string, i.e., the same string in L and R becomes the exact same terms.
			vData.string.left = vData.string.right = vData.string.context;
		}
		if(vData.stereo.context) {
			if(vData.stereo.left)
				return Result<>::Error(
						"Error in rule GML. Vertex " + std::to_string(id) + " has stereo both in 'context' and 'left'.");
			if(vData.stereo.right)
				return Result<>::Error(
						"Error in rule GML. Vertex " + std::to_string(id) + " has stereo both in 'context' and 'right'.");
			// for stereo it matters if it's L+R or it's K
		}

		// Check that there is a string/stereo in left/right when inLeft/inRight
		if(vData.inLeft && !vData.string.left)
			return Result<>::Error("Error in rule GML. Vertex " + std::to_string(id) + " is in L, but has no label.");
		if(vData.inRight && !vData.string.right)
			return Result<>::Error("Error in rule GML. Vertex " + std::to_string(id) + " is in R, but has no label.");

		vData.v = add_vertex(g);
		vertexMapId[vData.v] = id;
		data.externalToInternalIds[id] = get(boost::vertex_index_t(), g, vData.v);
		if(vData.inContext) {
			put_membership(dpoResult, vData.v, lib::Rules::Membership::Context);
			pString.add(vData.v, *vData.string.left, *vData.string.right);
		} else if(vData.inLeft) {
			assert(!vData.inRight);
			put_membership(dpoResult, vData.v, lib::Rules::Membership::Left);
			pString.add(vData.v, *vData.string.left, "");
		} else {
			assert(vData.inRight);
			put_membership(dpoResult, vData.v, lib::Rules::Membership::Right);
			pString.add(vData.v, "", *vData.string.right);
		}
	} // for each vertex

	for(auto &p : idMapEdge) {
		const int src = p.first.first;
		const int tar = p.first.second;
		if(idMapVertex.find(src) == end(idMapVertex))
			return Result<>::Error(
					"Error in rule GML. Edge endpoint '" + std::to_string(src) + "' does not exist for edge ("
					+ std::to_string(src) + ", " + std::to_string(tar) + ").");
		if(idMapVertex.find(tar) == end(idMapVertex))
			return Result<>::Error(
					"Error in rule GML. Edge endpoint '" + std::to_string(tar) + "' does not exist for edge ("
					+ std::to_string(src) + ", " + std::to_string(tar) + ").");
		Vertex vSrc = idMapVertex[src].v, vTar = idMapVertex[tar].v;
		auto &eData = p.second;
		// First find the right membership:
		// inContext <=> inLeft && inRight
		if(eData.inContext) eData.inLeft = eData.inRight = true;
		else if(eData.inLeft && eData.inRight) eData.inContext = true;

		// Check labels and make (left, right) the correct labels
		if(eData.string.context) {
			if(eData.string.left)
				return Result<>::Error("Error in rule GML. Edge (" + std::to_string(src) + ", " + std::to_string(tar) +
				                       ") has a label both in 'context' and 'left'.");
			if(eData.string.right)
				return Result<>::Error("Error in rule GML. Edge (" + std::to_string(src) + ", " + std::to_string(tar) +
				                       ") has a label both in 'context' and 'right'.");
			eData.string.left = eData.string.right = eData.string.context;
		}
		if(eData.stereo.context) {
			if(eData.stereo.left)
				return Result<>::Error("Error in rule GML. Edge (" + std::to_string(src) + ", " + std::to_string(tar) +
				                       ") has stereo both in 'context' and 'left'.");
			if(eData.stereo.right)
				return Result<>::Error("Error in rule GML. Edge (" + std::to_string(src) + ", " + std::to_string(tar) +
				                       ") has stereo both in 'context' and 'right'.");
			// for stereo it matters if it's L+R or it's K
		}

		// Check that there is a string in left/right when inLeft/inRight
		if(eData.inLeft && !eData.string.left)
			return Result<>::Error("Error in rule GML. Edge (" + std::to_string(src) + ", " + std::to_string(tar) +
			                       ") is in L, but has no label.");
		if(eData.inRight && !eData.string.right)
			return Result<>::Error("Error in rule GML. Edge (" + std::to_string(src) + ", " + std::to_string(tar) +
			                       ") is in R, but has no label.");

		eData.e = add_edge(vSrc, vTar, g).first;
		if(eData.inContext) {
			put_membership(dpoResult, eData.e, lib::Rules::Membership::Context);
			pString.add(eData.e, *eData.string.left, *eData.string.right);
		} else if(eData.inLeft) {
			assert(!eData.inRight);
			put_membership(dpoResult, eData.e, lib::Rules::Membership::Left);
			pString.add(eData.e, *eData.string.left, "");
		} else {
			assert(eData.inRight);
			put_membership(dpoResult, eData.e, lib::Rules::Membership::Right);
			pString.add(eData.e, "", *eData.string.right);
		}
	} // for each edge
	// the graph is set, so initialise the component storage
	dpoResult.initComponents();

	// constraints
	for(const GML::MatchConstraint &cGML : rule.matchConstraints) {
		struct MatchConstraintConverter {
			MatchConstraintConverter(lib::Rules::LabelledRule &dpoResult, const std::map<int, VertexLabels> &idMapVertex)
					: dpoResult(dpoResult), idMapVertex(idMapVertex) {}

			Result<> operator()(const GML::AdjacencyConstraint &cGML) {
				const auto iter = idMapVertex.find(cGML.id);
				if(iter == end(idMapVertex))
					return Result<>::Error("Error in rule GML. Vertex " + std::to_string(cGML.id) +
					                       " in adjacency constraint does not exist.");
				const Vertex vConstrained = iter->second.v;
				lib::GraphMorphism::Constraints::Operator op;
				{
					const auto &s = cGML.op;
					using namespace lib::GraphMorphism::Constraints;
					if(s == "<") op = Operator::LT;
					else if(s == "<=") op = Operator::LEQ;
					else if(s == "=") op = Operator::EQ;
					else if(s == ">=") op = Operator::GEQ;
					else if(s == ">") op = Operator::GT;
					else return Result<>::Error("Error in rule GML. Unknown operator '" + s + "' in adjacency constraint.");
				}
				auto c = std::make_unique<
						lib::GraphMorphism::Constraints::VertexAdjacency < lib::Rules::LabelledRule::LeftGraphType>
				         > (vConstrained, op, cGML.count);
				c->vertexLabels.insert(cGML.nodeLabels.begin(), cGML.nodeLabels.end());
				c->edgeLabels.insert(cGML.edgeLabels.begin(), cGML.edgeLabels.end());
				dpoResult.leftMatchConstraints.push_back(std::move(c));
				return Result<>();
			}

			Result<> operator()(const GML::ShortestPathConstraint &cGML) {
				const auto iterSrc = idMapVertex.find(cGML.source);
				const auto iterTar = idMapVertex.find(cGML.target);
				if(iterSrc == end(idMapVertex))
					return Result<>::Error("Error in rule GML. Vertex " + std::to_string(cGML.source) +
					                       " in shortest path constraint does not exist.");
				if(iterTar == end(idMapVertex))
					return Result<>::Error("Error in rule GML. Vertex " + std::to_string(cGML.target) +
					                       " in shortest path constraint does not exist.");
				const Vertex vSrc = iterSrc->second.v;
				const Vertex vTar = iterTar->second.v;
				lib::GraphMorphism::Constraints::Operator op;
				{
					const auto &s = cGML.op;
					using namespace lib::GraphMorphism::Constraints;
					if(s == "<") op = Operator::LT;
					else if(s == "<=") op = Operator::LEQ;
					else if(s == "=") op = Operator::EQ;
					else if(s == ">=") op = Operator::GEQ;
					else if(s == ">") op = Operator::GT;
					else
						return Result<>::Error(
								"Error in rule GML. Unknown operator '" + s + "' in shortest path constraint.");
				}
				const auto compSrc = dpoResult.leftComponents[get(boost::vertex_index_t(), get_graph(dpoResult), vSrc)];
				const auto compTar = dpoResult.leftComponents[get(boost::vertex_index_t(), get_graph(dpoResult), vTar)];
				if(compSrc != compTar)
					return Result<>::Error(
							"Error in rule GML. Vertex " + std::to_string(cGML.source) + " and " + std::to_string(cGML.target)
							+ " are in different connected components of the left graph. "
							+ "This is currently not supported for the shortest path constraint.");
				auto c = std::make_unique<
						lib::GraphMorphism::Constraints::ShortestPath < lib::Rules::LabelledRule::LeftGraphType>
				         > (vSrc, vTar, op, cGML.length);
				dpoResult.leftMatchConstraints.push_back(std::move(c));
				return Result<>();
			}
		public:
			lib::Rules::LabelledRule &dpoResult;
			const std::map<int, VertexLabels> &idMapVertex;
		} visitor(dpoResult, idMapVertex);
		if(auto res = std::visit(visitor, cGML); !res) return res;
	} // for each constriant
	bool doStereo = false;
	for(const auto &v : rule.left.vertices) doStereo = doStereo || v.stereo;
	for(const auto &v : rule.context.vertices) doStereo = doStereo || v.stereo;
	for(const auto &v : rule.right.vertices) doStereo = doStereo || v.stereo;
	for(const auto &e : rule.left.edges) doStereo = doStereo || e.stereo;
	for(const auto &e : rule.context.edges) doStereo = doStereo || e.stereo;
	for(const auto &e : rule.right.edges) doStereo = doStereo || e.stereo;
	if(!doStereo) return std::move(data); // TODO: remove std::move when C++20/P1825R0 is available

	// Stereo
	//==========================================================================
	const auto vFromVertexId = [&idMapVertex](int id) {
		auto iter = idMapVertex.find(id);
		assert(iter != end(idMapVertex));
		return iter->second.v;
	};
	lib::Rules::PropMoleculeCore mol(g, pString); // temporary for doing the inference
	lib::Rules::DPOProjection gLeft(g, lib::Rules::Membership::Left), gRight(g, lib::Rules::Membership::Right);
	auto molLeft = mol.getLeft();
	auto molRight = mol.getRight();
	auto leftInference = lib::Stereo::makeInference(gLeft, molLeft, true);
	auto rightInference = lib::Stereo::makeInference(gRight, molRight, true);
	const auto &gGeometry = lib::Stereo::getGeometryGraph();
	// Set the explicitly defined edge categories.
	//----------------------------------------------------------------------------
	for(const auto &p : idMapEdge) {
		const auto handleSide = [&p](const std::optional<std::string> &os, const std::string &side,
		                             auto &inference) -> Result<> {
			if(!os) return Result<>();
			const std::string &s = *os;
			if(s.size() != 1)
				return Result<>::Error("Error in stereo data for edge (" + std::to_string(p.first.first)
				                       + ", " + std::to_string(p.first.second) + ") in " + side
				                       + ". Parsing error in stereo data '" + s + "'.");
			lib::Stereo::EdgeCategory cat;
			switch(s.front()) {
			case '*':
				cat = lib::Stereo::EdgeCategory::Any;
				break;
			default:
				return Result<>::Error("Error in stereo data for edge (" + std::to_string(p.first.first) + ", "
				                       + std::to_string(p.first.second) + ") in " + side
				                       + ". Parsing error in stereo data '" + s + "'.");
			}
			auto res = inference.assignEdgeCategory(p.second.e, cat);
			if(!res) {
				res.setError("Error in stereo data for edge (" + std::to_string(p.first.first) + ", "
				             + std::to_string(p.first.second) + ") in " + side + ". " + res.extractError());
				return res;
			}
			return res;
		};
		if(auto res = handleSide(p.second.stereo.left, "L", leftInference); !res) return res;
		if(auto res = handleSide(p.second.stereo.right, "R", rightInference); !res) return res;
	} // for each edge
	// Set the explicitly stereo data.
	//----------------------------------------------------------------------------
	for(auto &p : idMapVertex) {
		const auto handleSide = [&](const std::optional<std::string> &os, const std::string &side, auto &inference,
		                            auto &parsedEmbedding, const auto &gSide) {
			if(!os) return Result<>();
			const auto &v = p.second.v;
			if(auto parsedEmbeddingRes = lib::IO::Stereo::Read::parseEmbedding(*os)) {
				parsedEmbedding = std::move(*parsedEmbeddingRes);
			} else {
				return Result<>::Error(
						"Error in stereo data for vertex " + std::to_string(p.first) + " in " + side + ". " +
						parsedEmbeddingRes.extractError());
			}
			// Geometry
			//..........................................................................
			const auto &embGML = *parsedEmbedding;
			if(embGML.geometry) {
				auto vGeo = gGeometry.findGeometry(*embGML.geometry);
				if(vGeo == gGeometry.nullGeometry())
					return Result<>::Error("Error in stereo data for vertex " + std::to_string(p.first) + " in " + side +
					                       ". Invalid gGeometry '" + *embGML.geometry + "'.");
				if(auto res = inference.assignGeometry(v, vGeo); !res) {
					return Result<>::Error(
							"Error in stereo data for vertex " + std::to_string(p.first) + " in " + side + ". " +
							res.extractError());
				}
			}
			// Edges
			//..........................................................................
			if(embGML.edges) {
				inference.initEmbedding(v);
				for(const auto &e : *embGML.edges) {
					if(const int *idPtr = std::get_if<int>(&e)) {
						int idNeighbour = *idPtr;
						if(idMapVertex.find(idNeighbour) == end(idMapVertex))
							return Result<>::Error("Error in graph GML. Neighbour vertex " + std::to_string(idNeighbour) +
							                       " in stereo embedding for vertex "
							                       + std::to_string(p.first) + " in " + side + " does not exist.");
						auto ePair = edge(v, vFromVertexId(idNeighbour), gSide);
						if(!ePair.second)
							return Result<>::Error("Error in graph GML. Vertex " + std::to_string(idNeighbour) +
							                       " in stereo embedding for vertex "
							                       + std::to_string(p.first) + " in " + side + " is not a neighbour.");
						inference.addEdge(v, ePair.first);
					} else if(const char *virtPtr = std::get_if<char>(&e)) {
						switch(*virtPtr) {
						case 'e':
							inference.addLonePair(v);
							break;
						default:
							MOD_ABORT; // the parser should know what is allowed
						}
					} else {
						MOD_ABORT; // the parser should know what is allowed
					}
				}
			}
			// Fixation
			//..........................................................................
			if(embGML.fixation) {
				// TODO: expand this when more complicated geometries are implemented
				const bool isFixed = *embGML.fixation;
				if(isFixed) inference.fixSimpleGeometry(v);
			}
			return Result<>();
		};
		if(auto res = handleSide(p.second.stereo.left, "L", leftInference, p.second.parsedEmbeddingLeft,
		                         gLeft); !res)
			return res;
		if(auto res = handleSide(p.second.stereo.right, "R", rightInference, p.second.parsedEmbeddingRight,
		                         gRight); !res)
			return res;
	} // for each vertex

	const auto finalize = [&warnings, &vertexMapId](auto &inference, const std::string &side) {
		return inference.finalize(warnings, [&vertexMapId, &side](Vertex v) {
			auto iter = vertexMapId.find(v);
			assert(iter != vertexMapId.end());
			return std::to_string(iter->second) + " in " + side;
		});
	};
	if(auto resLeft = finalize(leftInference, "L"); !resLeft) return resLeft;
	if(auto resRight = finalize(rightInference, "R"); !resRight) return resRight;

	const auto vertexInContext = [&](Vertex v) -> bool {
		const auto idIter = vertexMapId.find(v);
		assert(idIter != end(vertexMapId));
		const auto lIter = idMapVertex.find(idIter->second);
		assert(lIter != end(idMapVertex));
		assert(lIter->second.inContext);
		const auto &stereo = lIter->second.stereo;
		// if there is any stereo data, maybe we are in context
		if(stereo.left.has_value() || stereo.context.has_value() || stereo.right.has_value())
			return stereo.context.has_value();
		else // otherwise, default to be in context
			return true;
	};
	const auto edgeInContext = [&](Edge e) -> bool {
		const auto vSrc = source(e, g);
		const auto vTar = target(e, g);
		const auto idSrcIter = vertexMapId.find(vSrc);
		const auto idTarIter = vertexMapId.find(vTar);
		assert(idSrcIter != end(vertexMapId));
		assert(idTarIter != end(vertexMapId));
		const auto lIter = idMapEdge.find(std::make_pair(idSrcIter->second, idTarIter->second));
		assert(lIter != end(idMapEdge));
		assert(lIter->second.inContext);
		const auto &stereo = lIter->second.stereo;
		// if there is any stereo data, maybe we are in context
		if(stereo.left.has_value() || stereo.context.has_value() || stereo.right.has_value())
			return stereo.context.has_value();
		else // otherwise, default to be in context
			return true;
	};
	dpoResult.pStereo = std::make_unique<lib::Rules::PropStereoCore>(g, std::move(leftInference),
	                                                                 std::move(rightInference), vertexInContext,
	                                                                 edgeInContext);
	return std::move(data); // TODO: remove std::move when C++20/P1825R0 is available
}

} // namespace mod::lib::IO::Rules::Read