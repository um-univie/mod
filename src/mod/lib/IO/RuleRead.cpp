#include "Rule.h"

#include <mod/lib/GraphMorphism/Constraints/ShortestPath.h>
#include <mod/lib/GraphMorphism/Constraints/VertexAdjacency.h>
#include <mod/lib/IO/GMLUtils.h>
#include <mod/lib/Rules/Properties/Molecule.h>
#include <mod/lib/Rules/Properties/Stereo.h>
#include <mod/lib/Stereo/Inference.h>

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
	GML::Rule rule;
	{
		gml::ast::List ast;
		bool res = gml::parser::parse(s, ast, err);
		if(!res) return Data();
		using namespace gml::converter::edsl;
		auto cVertex = GML::makeVertexConverter(0);
		auto cEdge = GML::makeEdgeConverter(0);
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
				(string("labelType", &GML::Rule::labelType), 0, 1)
				(makeSide("left", &GML::Rule::left), 0, 1)
				(makeSide("context", &GML::Rule::context), 0, 1)
				(makeSide("right", &GML::Rule::right), 0, 1)
				(constrainAdj) (constrainShortestPath)
				;
		auto iterBegin = ast.list.begin();
		res = gml::converter::convert(iterBegin, ast.list.end(), cRule, err, rule);
		if(!res) return Data();
	}

	auto checkSide = [&err](const GML::Graph &side, std::string name) {
		std::unordered_set<int> vertexIds;
		for(const GML::Vertex &v : side.vertices) {
			if(vertexIds.find(v.id) != end(vertexIds)) {
				err << "Duplicate vertex " << v.id << " in " << name << " graph.";
				return false;
			}
			vertexIds.insert(v.id);
		}
		std::set<std::pair<int, int> > edgeIds;
		for(const GML::Edge &e : side.edges) {
			if(e.source == e.target) {
				err << "Loop edge (on " << e.source << ", in " << name << ") is not allowed.";
				return false;
			}
			auto eSorted = std::minmax(e.source, e.target);
			if(edgeIds.find(eSorted) != end(edgeIds)) {
				err << "Duplicate edge (" << e.source << ", " << e.target << ") in " << name << " graph.";
				return false;
			}
			edgeIds.insert(eSorted);
		}
		return true;
	};
	if(!checkSide(rule.left, "left")) return Data();
	if(!checkSide(rule.context, "context")) return Data();
	if(!checkSide(rule.right, "right")) return Data();

	using Vertex = lib::Rules::Vertex;
	using Edge = lib::Rules::Edge;
	Data data;
	data.rule = lib::Rules::LabelledRule();
	data.name = rule.id;
	if(rule.labelType) {
		if(rule.labelType.get() == "string") data.labelType = LabelType::String;
		else if(rule.labelType.get() == "term") data.labelType = LabelType::Term;
		else {
			err << "Error in rule GML. Unknown labelType '" << rule.labelType.get() << "'.";
			return Data();
		}
	}

	auto &dpoResult = data.rule.get();
	auto &g = get_graph(dpoResult);
	dpoResult.pString = std::make_unique<lib::Rules::PropStringCore>(g);
	auto &pString = *dpoResult.pString;

	struct VertexLabels {
		bool inLeft = false, inContext = false, inRight = false;
		Label<std::string> string, stereo;
	public:
		Vertex v;
		boost::optional<lib::IO::Stereo::Read::ParsedEmbedding> parsedEmbeddingLeft, parsedEmbeddingRight;
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
			if(vData.string.left) {
				err << "Error in rule GML. Vertex " << id << " has a label both in 'context' and 'left'.";
				return Data();
			}
			if(vData.string.right) {
				err << "Error in rule GML. Vertex " << id << " has a label both in 'context' and 'right'.";
				return Data();
			}
			// Note: terms follow the same semantics as string, i.e., the same string in L and R becomes the exact same terms.
			vData.string.left = vData.string.right = vData.string.context;
		}
		if(vData.stereo.context) {
			if(vData.stereo.left) {
				err << "Error in rule GML. Vertex " << id << " has stereo both in 'context' and 'left'.";
				return Data();
			}
			if(vData.stereo.right) {
				err << "Error in rule GML. Vertex " << id << " has stereo both in 'context' and 'right'.";
				return Data();
			}
			// for stereo it matters if it's L+R or it's K
		}

		// Check that there is a string/stereo in left/right when inLeft/inRight
		if(vData.inLeft && !vData.string.left) {
			err << "Error in rule GML. Vertex " << id << " is in L, but has no label.";
			return Data();
		}
		if(vData.inRight && !vData.string.right) {
			err << "Error in rule GML. Vertex " << id << " is in R, but has no label.";
			return Data();
		}

		vData.v = add_vertex(g);
		vertexMapId[vData.v] = id;
		data.externalToInternalIds[id] = get(boost::vertex_index_t(), g, vData.v);
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
	} // for each vertex

	for(auto &p : idMapEdge) {
		int src = p.first.first;
		int tar = p.first.second;
		if(idMapVertex.find(src) == end(idMapVertex)) {
			err << "Error in rule GML. Edge endpoint '" << src << "' does not exist for edge (" << src << ", " << tar << ").";
			return Data();
		}
		if(idMapVertex.find(tar) == end(idMapVertex)) {
			err << "Error in rule GML. Edge endpoint '" << tar << "' does not exist for edge (" << src << ", " << tar << ").";
			return Data();
		}
		Vertex vSrc = idMapVertex[src].v, vTar = idMapVertex[tar].v;
		auto &eData = p.second;
		// First find the right membership:
		// inContext <=> inLeft && inRight
		if(eData.inContext) eData.inLeft = eData.inRight = true;
		else if(eData.inLeft && eData.inRight) eData.inContext = true;

		// Check labels and make (left, right) the correct labels
		if(eData.string.context) {
			if(eData.string.left) {
				err << "Error in rule GML. Edge (" << src << ", " << tar << ") has a label both in 'context' and 'left'.";
				return Data();
			}
			if(eData.string.right) {
				err << "Error in rule GML. Edge (" << src << ", " << tar << ") has a label both in 'context' and 'left'.";
				return Data();
			}
			eData.string.left = eData.string.right = eData.string.context;
		}
		if(eData.stereo.context) {
			if(eData.stereo.left) {
				err << "Error in rule GML. Edge (" << src << ", " << tar << ") has stereo both in 'context' and 'left'.";
				return Data();
			}
			if(eData.stereo.right) {
				err << "Error in rule GML. Edge (" << src << ", " << tar << ") has stereo both in 'context' and 'left'.";
				return Data();
			}
			// for stereo it matters if it's L+R or it's K
		}

		// Check that there is a string in left/right when inLeft/inRight
		if(eData.inLeft && !eData.string.left) {
			err << "Error in rule GML. Edge (" << src << ", " << tar << ") is in L, but has no label.";
			return Data();
		}
		if(eData.inRight && !eData.string.right) {
			err << "Error in rule GML. Edge (" << src << ", " << tar << ") is in R, but has no label.";
			return Data();
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
	} // for each edge
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
					err << "Error in rule GML. Vertex " << cGML.id << " in adjacency constraint does not exist.";
					return false;
				}
				Vertex vConstrained = iter->second.v;
				lib::GraphMorphism::Constraints::Operator op;
				{
					const auto &s = cGML.op;
					using namespace lib::GraphMorphism::Constraints;
					if(s == "<") op = Operator::LT;
					else if(s == "<=") op = Operator::LEQ;
					else if(s == "=") op = Operator::EQ;
					else if(s == ">=") op = Operator::GEQ;
					else if(s == ">") op = Operator::GT;
					else {
						err << "Error in rule GML. Unknown operator '" << s << "' in adjacency constraint.";
						return false;
					}
				}
				auto c = std::make_unique<lib::GraphMorphism::Constraints::VertexAdjacency<lib::Rules::LabelledRule::LeftGraphType> >(
						vConstrained, op, cGML.count);
				for(const auto &s : cGML.nodeLabels) c->vertexLabels.insert(s);
				for(const auto &s : cGML.edgeLabels) c->edgeLabels.insert(s);
				dpoResult.leftMatchConstraints.push_back(std::move(c));
				return true;
			}

			bool operator()(const GML::ShortestPathConstraint &cGML) {
				auto iterSrc = idMapVertex.find(cGML.source);
				auto iterTar = idMapVertex.find(cGML.target);
				if(iterSrc == end(idMapVertex)) {
					err << "Error in rule GML. Vertex " << cGML.source << " in shortest path constraint does not exist.";
					return false;
				}
				if(iterTar == end(idMapVertex)) {
					err << "Error in rule GML. Vertex " << cGML.target << " in shortest path constraint does not exist.";
					return false;
				}
				Vertex vSrc = iterSrc->second.v;
				Vertex vTar = iterTar->second.v;
				lib::GraphMorphism::Constraints::Operator op;
				{
					const auto &s = cGML.op;
					using namespace lib::GraphMorphism::Constraints;
					if(s == "<") op = Operator::LT;
					else if(s == "<=") op = Operator::LEQ;
					else if(s == "=") op = Operator::EQ;
					else if(s == ">=") op = Operator::GEQ;
					else if(s == ">") op = Operator::GT;
					else {
						err << "Error in rule GML. Unknown operator '" << s << "' in shortest path constraint.";
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
				auto c = std::make_unique<lib::GraphMorphism::Constraints::ShortestPath<lib::Rules::LabelledRule::LeftGraphType> >(
						vSrc, vTar, op, cGML.length);
				dpoResult.leftMatchConstraints.push_back(std::move(c));
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
	bool doStereo = false;
	for(const auto &v : rule.left.vertices) doStereo = doStereo || v.stereo;
	for(const auto &v : rule.context.vertices) doStereo = doStereo || v.stereo;
	for(const auto &v : rule.right.vertices) doStereo = doStereo || v.stereo;
	for(const auto &e : rule.left.edges) doStereo = doStereo || e.stereo;
	for(const auto &e : rule.context.edges) doStereo = doStereo || e.stereo;
	for(const auto &e : rule.right.edges) doStereo = doStereo || e.stereo;
	if(!doStereo) return data;

	// Stereo
	//==========================================================================
	auto vFromVertexId = [&idMapVertex](int id) {
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
	std::stringstream ssErr;
	const auto &gGeometry = lib::Stereo::getGeometryGraph();
	// Set the explicitly defined edge categories.
	//----------------------------------------------------------------------------
	for(const auto &p : idMapEdge) {
		const auto handleSide = [&err, &ssErr, &p](const boost::optional<std::string> &os, const std::string &side, auto &inference) {
			if(!os) return true;
			const std::string &s = *os;
			if(s.size() != 1) {
				err << "Error in stereo data for edge (" << p.first.first << ", " << p.first.second << ") in " << side << ". ";
				err << "Parsing error in stereo data '" << s << "'.";
				return false;
			}
			lib::Stereo::EdgeCategory cat;
			switch(s.front()) {
			case '*': cat = lib::Stereo::EdgeCategory::Any;
				break;
			default:
				err << "Error in stereo data for edge (" << p.first.first << ", " << p.first.second << ") in " << side << ". ";
				err << "Parsing error in stereo data '" << s << "'.";
				return false;
			}
			bool res = inference.assignEdgeCategory(p.second.e, cat, ssErr);
			if(!res) {
				err << "Error in stereo data for edge (" << p.first.first << ", " << p.first.second << ") in " << side << ". ";
				err << ssErr.str();
				return false;
			}
			return true;
		};
		if(!handleSide(p.second.stereo.left, "L", leftInference)) return Data();
		if(!handleSide(p.second.stereo.right, "R", rightInference)) return Data();
	} // for each edge
	// Set the explicitly stereo data.
	//----------------------------------------------------------------------------
	for(auto &p : idMapVertex) {
		const auto handleSide = [&](const boost::optional<std::string> &os, const std::string &side, auto &inference, auto &parsedEmbedding, const auto &gSide) {
			if(!os) return true;
			const auto &v = p.second.v;
			parsedEmbedding = lib::IO::Stereo::Read::parseEmbedding(os.get(), ssErr);
			if(!parsedEmbedding) {
				err << "Error in stereo data for vertex " << p.first << " in " << side << ". ";
				err << ssErr.str();
				return false;
			}
			// Geometry
			//..........................................................................
			const auto &embGML = *parsedEmbedding;
			if(embGML.geometry) {
				auto vGeo = gGeometry.findGeometry(*embGML.geometry);
				if(vGeo == gGeometry.nullGeometry()) {
					err << "Error in stereo data for vertex " << p.first << " in " << side << ". Invalid gGeometry '" << *embGML.geometry << "'." << std::endl;
					return false;
				}
				bool res = inference.assignGeometry(v, vGeo, ssErr);
				if(!res) {
					err << "Error in stereo data for vertex " << p.first << " in " << side << ". " << ssErr.str();
					return false;
				}
			}
			// Edges
			//..........................................................................
			if(embGML.edges) {
				inference.initEmbedding(v);
				for(const auto &e : *embGML.edges) {
					if(const int *idPtr = boost::get<int>(&e)) {
						int idNeighbour = *idPtr;
						if(idMapVertex.find(idNeighbour) == end(idMapVertex)) {
							err << "Error in graph GML. Neighbour vertex " << idNeighbour << " in stereo embedding for vertex " << p.first << " in " << side << " does not exist." << std::endl;
							return false;
						}
						auto ePair = edge(v, vFromVertexId(idNeighbour), gSide);
						if(!ePair.second) {
							err << "Error in graph GML. Vertex " << idNeighbour << " in stereo embedding for vertex " << p.first << " in " << side << " is not a neighbour." << std::endl;
							return false;
						}
						inference.addEdge(v, ePair.first);
					} else if(const char *virtPtr = boost::get<char>(&e)) {
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
				bool isFixed = embGML.fixation.get();
				if(isFixed) inference.fixSimpleGeometry(v);
			}
			return true;
		};
		if(!handleSide(p.second.stereo.left, "L", leftInference, p.second.parsedEmbeddingLeft, gLeft)) return Data();
		if(!handleSide(p.second.stereo.right, "R", rightInference, p.second.parsedEmbeddingRight, gRight)) return Data();
	} // for each vertex

	auto finalize = [&err, &ssErr, &vertexMapId](auto &inference, const std::string & side) {
		auto stereoResult = inference.finalize(ssErr, [&vertexMapId, &side](Vertex v) {
			auto iter = vertexMapId.find(v);
			assert(iter != vertexMapId.end());
			return std::to_string(iter->second) + " in " + side;
		});
		switch(stereoResult) {
		case lib::Stereo::DeductionResult::Success: return true;
		case lib::Stereo::DeductionResult::Warning:
			IO::log() << ssErr.str();
			return true;
		case lib::Stereo::DeductionResult::Error:
			err << ssErr.str();
			return false;
		}
		MOD_ABORT; // should not happen
	};
	auto resLeft = finalize(leftInference, "L");
	if(!resLeft) return Data();
	auto resRight = finalize(rightInference, "R");
	if(!resRight) return Data();

	const auto vertexInContext = [&](Vertex v) -> bool {
		const auto idIter = vertexMapId.find(v);
		assert(idIter != end(vertexMapId));
		const auto lIter = idMapVertex.find(idIter->second);
		assert(lIter != end(idMapVertex));
		assert(lIter->second.inContext);
		const auto &stereo = lIter->second.stereo;
		// if there is any stereo data, maybe we are in context
		if(stereo.left.is_initialized() || stereo.context.is_initialized() || stereo.right.is_initialized())
			return stereo.context.is_initialized();
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
		if(stereo.left.is_initialized() || stereo.context.is_initialized() || stereo.right.is_initialized())
			return stereo.context.is_initialized();
		else // otherwise, default to be in context
			return true;
	};
	dpoResult.pStereo = std::make_unique<lib::Rules::PropStereoCore>(g, std::move(leftInference), std::move(rightInference), vertexInContext, edgeInContext);
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
