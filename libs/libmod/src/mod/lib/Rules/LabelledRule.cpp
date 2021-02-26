#include "LabelledRule.hpp"

#include <mod/Config.hpp>
#include <mod/lib/GraphMorphism/Finder.hpp>
#include <mod/lib/Stereo/CloneUtil.hpp>
#include <mod/lib/Stereo/Inference.hpp>

#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>


#include <boost/graph/connected_components.hpp>

#include <iostream>

namespace mod::lib::Rules {

// LabelledRule
//------------------------------------------------------------------------------

LabelledRule::LabelledRule() : g(new GraphType()) {}

LabelledRule::LabelledRule(const LabelledRule &other, bool withConstraints) : LabelledRule() {
	auto &g = *this->g;
	const auto &gOther = get_graph(other);
	this->pString = std::make_unique<PropStringType>(g);
	auto &pString = *this->pString;
	auto &pStringOther = get_string(other);
	for(const auto vOther : asRange(vertices(gOther))) {
		const auto v = add_vertex(g);
		g[v].membership = membership(other, vOther);
		switch(g[v].membership) {
		case Membership::Left:
			pString.add(v, pStringOther.getLeft()[vOther], "");
			break;
		case Membership::Right:
			pString.add(v, "", pStringOther.getRight()[vOther]);
			break;
		case Membership::Context:
			pString.add(v, pStringOther.getLeft()[vOther], pStringOther.getRight()[vOther]);
			break;
		}
	}
	for(const auto eOther : asRange(edges(gOther))) {
		const auto e = add_edge(source(eOther, gOther), target(eOther, gOther), g).first;
		g[e].membership = membership(other, eOther);
		switch(g[e].membership) {
		case Membership::Left:
			pString.add(e, pStringOther.getLeft()[eOther], "");
			break;
		case Membership::Right:
			pString.add(e, "", pStringOther.getRight()[eOther]);
			break;
		case Membership::Context:
			pString.add(e, pStringOther.getLeft()[eOther], pStringOther.getRight()[eOther]);
			break;
		}
	}
	if(other.pStereo) {
		const auto &lgLeft = get_labelled_left(other);
		const auto &lgRight = get_labelled_right(other);
		const auto infLeft = Stereo::makeCloner(lgLeft, get_left(*this), jla_boost::Identity(), jla_boost::Identity());
		const auto infRight = Stereo::makeCloner(lgRight, get_right(*this), jla_boost::Identity(), jla_boost::Identity());
		const auto inContext = [&](const auto &ve) {
			return get_stereo(other).inContext(ve);
		};
		this->pStereo.reset(new PropStereoCore(g, infLeft, infRight, inContext, inContext));
	}
	if(withConstraints) {
		leftMatchConstraints.reserve(other.leftMatchConstraints.size());
		rightMatchConstraints.reserve(other.rightMatchConstraints.size());
		for(auto &&c : other.leftMatchConstraints)
			leftMatchConstraints.push_back(c->clone());
		for(auto &&c : other.rightMatchConstraints)
			rightMatchConstraints.push_back(c->clone());
	}
}

template<typename SideGraph>
std::vector<LabelledRule::ComponentGraph_v2> splitSideGraph(const SideGraph&& sideGraph,
                                                            std::vector<std::vector<std::size_t>>& componentVertexToCoreVertexMap,
                                                            std::vector<std::pair<size_t, size_t>>& coreVertexToComponentVertex
                                                            ) {
	struct GraphData {
		using Graph = LabelledRule::ComponentGraph_v2;
		using SideVertex = boost::graph_traits<LabelledLeftGraph::GraphType>::vertex_descriptor;
	public:
		GraphData() :
		    gPtr(new Graph::GraphType()),
		    pStringPtr(new Graph::PropStringType(*gPtr)) {}

	public:
		std::unique_ptr<GraphData::Graph::GraphType> gPtr;
		std::unique_ptr<GraphData::Graph::PropStringType> pStringPtr;
		std::unique_ptr<GraphData::Graph::PropStereoType> pStereoPtr;
		std::vector<SideVertex> vertexMap;
	};
	using Vertex = lib::Graph::Vertex;

	std::vector<LabelledRule::ComponentGraph_v2> out;

	std::size_t num_components = get_num_connected_components(sideGraph);

	//const auto& rDPO = rule.getDPORule();

	std::vector<GraphData> products(num_components);
	componentVertexToCoreVertexMap.clear();

	const auto &compMap = get_connected_component_map(sideGraph);
	//const auto &gRight = get_left(rDPO);
	const auto &gSideGraph = get_graph(sideGraph);
	auto rpString = get_string(sideGraph);
	assert(num_vertices(gSideGraph) == num_vertices(get_graph(sideGraph.r)));
	std::vector<Vertex> vertexMap(num_vertices(gSideGraph));
	for(const auto vSide : asRange(vertices(gSideGraph))) {
		const auto comp = compMap[get(boost::vertex_index_t(), gSideGraph, vSide)];
		auto &p = products[comp];
		const auto v = add_vertex(*p.gPtr);
		vertexMap[get(boost::vertex_index_t(), gSideGraph, vSide)] = v;
		p.pStringPtr->addVertex(v, rpString[vSide]);
	}
	for(const auto eSide : asRange(edges(gSideGraph))) {
		const auto vSideSrc = source(eSide, gSideGraph);
		const auto vSideTar = target(eSide, gSideGraph);
		const auto comp = compMap[get(boost::vertex_index_t(), gSideGraph, vSideSrc)];
		assert(comp == compMap[get(boost::vertex_index_t(), gSideGraph, vSideTar)]);
		const auto vCompSrc = vertexMap[get(boost::vertex_index_t(), gSideGraph, vSideSrc)];
		const auto vCompTar = vertexMap[get(boost::vertex_index_t(), gSideGraph, vSideTar)];
		const auto epComp = add_edge(vCompSrc, vCompTar, *products[comp].gPtr);
		assert(epComp.second);
		products[comp].pStringPtr->addEdge(epComp.first, rpString[eSide]);
	}

	// make the inverse vertex maps
	for(auto &p : products) {
		p.vertexMap.resize(num_vertices(*p.gPtr));
	}

	for(const auto vSide : asRange(vertices(gSideGraph))) {
		const auto comp = compMap[get(boost::vertex_index_t(), gSideGraph, vSide)];
		auto &p = products[comp];
		const auto v = vertexMap[get(boost::vertex_index_t(), gSideGraph, vSide)];
		p.vertexMap[get(boost::vertex_index_t(), *p.gPtr, v)] = vSide;
		coreVertexToComponentVertex[vSide] = std::make_pair(comp, v);
	}

	if (has_stereo(sideGraph.r)) {
		for(auto &p : products) {
			// const auto &lgRight = get_labelled_left(rDPO);
			assert(has_stereo(sideGraph));
			const auto vertexMap = [&p](const auto &vProduct) {
				return p.vertexMap[get(boost::vertex_index_t(), *p.gPtr, vProduct)];
			};
			const auto edgeMap = [&p, &sideGraph](const auto &eProduct) {
				const auto &g = *p.gPtr;
				const auto &gSide = get_graph(sideGraph);
				const auto vSrc = source(eProduct, g);
				const auto vTar = target(eProduct, g);
				const auto vSrcSide = p.vertexMap[get(boost::vertex_index_t(), g, vSrc)];
				const auto vTarSide = p.vertexMap[get(boost::vertex_index_t(), g, vTar)];
				const auto epSide = edge(vSrcSide, vTarSide, gSide);
				assert(epSide.second);
				return epSide.first;
			};
			const auto inf = Stereo::makeCloner(sideGraph, *p.gPtr, vertexMap, edgeMap);
			p.pStereoPtr = std::make_unique<lib::Graph::PropStereo>(*p.gPtr, inf);
		} // end foreach product
	} // end of stereo prop
	// wrap them

	for(auto &g : products) {
		out.emplace_back(std::move(g.gPtr), std::move(g.pStringPtr), std::move(g.pStereoPtr));
		componentVertexToCoreVertexMap.push_back(g.vertexMap);
	}
	return out;
}

void LabelledRule::initComponents() { // TODO: structure this better
	if(numLeftComponents != std::numeric_limits<std::size_t>::max()) MOD_ABORT;
	leftComponents.resize(num_vertices(get_graph(*this)), -1);
	rightComponents.resize(num_vertices(get_graph(*this)), -1);
	numLeftComponents = boost::connected_components(get_graph(get_labelled_left(*this)), leftComponents.data());
	numRightComponents = boost::connected_components(get_graph(get_labelled_right(*this)), rightComponents.data());
}

void LabelledRule::initComponentsV2() const { // TODO: structure this better
	if(numLeftComponents == std::numeric_limits<std::size_t>::max()) MOD_ABORT;
	if (numLeftComponents == leftComponentGraphs.size() && numRightComponents == rightComponentGraphs.size()) {
		return;
	}
	// experimental
	coreVertexToLeftComponentVertex = std::vector<std::pair<size_t, size_t>>(num_vertices(get_graph(*this)),
	                                                                         std::make_pair(boost::graph_traits<ComponentGraph_v2::GraphType>::null_vertex(),
	                                                                                        boost::graph_traits<ComponentGraph_v2::GraphType>::null_vertex()
	                                                                                        ));
	coreVertexToRightComponentVertex = std::vector<std::pair<size_t, size_t>>(num_vertices(get_graph(*this)),
	                                                                         std::make_pair(boost::graph_traits<ComponentGraph_v2::GraphType>::null_vertex(),
	                                                                                        boost::graph_traits<ComponentGraph_v2::GraphType>::null_vertex()
	                                                                                        ));
	leftComponentGraphs = splitSideGraph(get_labelled_left(*this), leftComponentVertexToCoreVertex, coreVertexToLeftComponentVertex);
	assert(leftComponentVertexToCoreVertex.size() == numLeftComponents);
	rightComponentGraphs = splitSideGraph(get_labelled_right(*this), rightComponentVertexToCoreVertex, coreVertexToRightComponentVertex);
	assert(rightComponentVertexToCoreVertex.size() == numRightComponents);
}


void LabelledRule::invert() {
	// invert the underlying graph
	// and not the actual inversion
	auto &g = *this->g;
	for(const auto v : asRange(vertices(g)))
		g[v].membership = jla_boost::GraphDPO::invert(g[v].membership);
	for(const auto e : asRange(edges(g)))
		g[e].membership = jla_boost::GraphDPO::invert(g[e].membership);
	// invert props
	if(pString) pString->invert();
	if(pTerm) pTerm->invert();
	if(pMolecule) pMolecule->invert();
	if(pStereo) {
		// TODO: this requires change all the offsets are recalculated
		throw FatalError("Missing implementation of rule inversion with stereo info.");
	}
	// also invert the component stuff
	using std::swap;
	swap(this->numLeftComponents, this->numRightComponents);
	swap(this->leftComponents, this->rightComponents);
	swap(this->leftMatchConstraints, this->rightMatchConstraints);
	// clear cached stuff
	this->projs.reset();
}

GraphType &get_graph(LabelledRule &r) {
	return *r.g;
}

const GraphType &get_graph(const LabelledRule &r) {
	return *r.g;
}

const LabelledRule::PropStringType &get_string(const LabelledRule &r) {
	assert(r.pString || r.pTerm);
	return *r.pString;
}

const LabelledRule::PropTermType &get_term(const LabelledRule &r) {
	assert(r.pString || r.pTerm);
	if(!r.pTerm) {
		r.pTerm.reset(new LabelledRule::PropTermType(
				get_graph(r), r.leftMatchConstraints, r.rightMatchConstraints,
				get_string(r), lib::Term::getStrings()
		));
	}
	return *r.pTerm;
}

bool has_stereo(const LabelledRule &r) {
	return bool(r.pStereo);
}

const LabelledRule::PropStereoType &get_stereo(const LabelledRule &r) {
	if(!has_stereo(r)) {
		auto gLeft = get_labelled_left(r);
		auto gRight = get_labelled_right(r);
		auto pMoleculeLeft = get_molecule(gLeft);
		auto pMoleculeRight = get_molecule(gRight);
		auto leftInference = lib::Stereo::makeInference(get_graph(gLeft), pMoleculeLeft, true);
		auto rightInference = lib::Stereo::makeInference(get_graph(gRight), pMoleculeRight, true);

		{
			lib::IO::Warnings warnings;
			auto res = leftInference.finalize(warnings, [&r](LabelledRule::Vertex v) {
				return std::to_string(get(boost::vertex_index_t(), get_graph(r), v)) + " left";
			});
			if(!getConfig().stereo.silenceDeductionWarnings.get())
				std::cout << warnings;
			res.throwIfError<StereoDeductionError>();
		}
		{
			lib::IO::Warnings warnings;
			auto res = rightInference.finalize(warnings, [&r](LabelledRule::Vertex v) {
				return std::to_string(get(boost::vertex_index_t(), get_graph(r), v)) + " right";
			});
			if(!getConfig().stereo.silenceDeductionWarnings.get())
				std::cout << warnings;
			res.throwIfError<StereoDeductionError>();
		}

		r.pStereo.reset(new PropStereoCore(get_graph(r),
		                                   std::move(leftInference), std::move(rightInference), jla_boost::AlwaysTrue(),
		                                   jla_boost::AlwaysTrue()));
	}
	return *r.pStereo;
}

const LabelledRule::PropMoleculeType &get_molecule(const LabelledRule &r) {
	if(!r.pMolecule) {
		r.pMolecule.reset(new LabelledRule::PropMoleculeType(get_graph(r), get_string(r)));
	}
	return *r.pMolecule;
}

const LabelledRule::LeftGraphType &get_left(const LabelledRule &r) {
	if(!r.projs) r.projs.reset(new LabelledRule::Projections(r));
	return r.projs->left;
}

const LabelledRule::ContextGraphType &get_context(const LabelledRule &r) {
	if(!r.projs) r.projs.reset(new LabelledRule::Projections(r));
	return r.projs->context;
}

const LabelledRule::RightGraphType &get_right(const LabelledRule &r) {
	if(!r.projs) r.projs.reset(new LabelledRule::Projections(r));
	return r.projs->right;
}

jla_boost::GraphDPO::Membership membership(const LabelledRule &r, const Vertex &v) {
	return get_graph(r)[v].membership;
}

jla_boost::GraphDPO::Membership membership(const LabelledRule &r, const Edge &e) {
	return get_graph(r)[e].membership;
}

void put_membership(LabelledRule &r, const Vertex &v, jla_boost::GraphDPO::Membership m) {
	get_graph(r)[v].membership = m;
}

void put_membership(LabelledRule &r, const Edge &e, jla_boost::GraphDPO::Membership m) {
	get_graph(r)[e].membership = m;
}

LabelledRule::LabelledLeftType get_labelled_left(const LabelledRule &r) {
	return LabelledRule::LabelledLeftType(r);
}

LabelledRule::LabelledRightType get_labelled_right(const LabelledRule &r) {
	return LabelledRule::LabelledRightType(r);
}

LabelledRule::Projections::Projections(const LabelledRule &r)
		: left(get_graph(r), Membership::Left),
		  context(get_graph(r), Membership::Context),
		  right(get_graph(r), Membership::Right) {}


// LabelledSideGraph
//------------------------------------------------------------------------------

namespace detail {



LabelledSideGraph::LabelledSideGraph(const LabelledRule &r, jla_boost::GraphDPO::Membership m)
		: r(r), m(m) {}

} // namespace detail

// LabelledLeftGraph
//------------------------------------------------------------------------------

LabelledLeftGraph::LabelledLeftGraph(const LabelledRule &r)
        : Base(r, jla_boost::GraphDPO::Membership::Left) {}

const LabelledLeftGraph::GraphType &get_graph(const LabelledLeftGraph &g) {
	return get_left(g.r);
}

LabelledLeftGraph::PropStringType get_string(const LabelledLeftGraph &g) {
	return get_string(g.r).getLeft();
}

LabelledLeftGraph::PropTermType get_term(const LabelledLeftGraph &g) {
	return get_term(g.r).getLeft();
}

bool has_stereo(const LabelledLeftGraph &g) {
	return has_stereo(g.r);
}

LabelledLeftGraph::PropStereoType get_stereo(const LabelledLeftGraph &g) {
	return get_stereo(g.r).getLeft();
}

const std::vector<std::unique_ptr<LabelledRule::LeftMatchConstraint> > &
get_match_constraints(const LabelledLeftGraph &g) {
	return g.r.leftMatchConstraints;
}

std::size_t get_num_connected_components(const LabelledLeftGraph &g) {
	return g.r.numLeftComponents;
}

const std::vector<size_t>& get_connected_component_map(const LabelledLeftGraph &g) {
	return g.r.leftComponents;
}

const LabelledRule::ComponentGraph_v2& get_component_graph_v2(std::size_t i, const LabelledLeftGraph &g) {
	assert(i < g.r.numLeftComponents);
	return g.r.leftComponentGraphs[i];
}

boost::graph_traits<SideGraphType>::vertex_descriptor get_component_core_vertex(std::size_t i, std::size_t v, const LabelledLeftGraph &g) {
	assert(i < g.r.numLeftComponents);
	assert(v < g.r.leftComponentVertexToCoreVertex[i].size());
	return g.r.leftComponentVertexToCoreVertex[i][v];
}

LabelledLeftGraph::PropMoleculeType get_molecule(const LabelledLeftGraph &g) {
	return get_molecule(g.r).getLeft();
}

LabelledLeftGraph::Base::ComponentGraph
get_component_graph(std::size_t i, const LabelledLeftGraph &g) {
	assert(i < get_num_connected_components(g));
	LabelledLeftGraph::Base::ComponentFilter filter(&get_graph(g), &g.r.leftComponents, i);
	return LabelledLeftGraph::Base::ComponentGraph(get_graph(g), filter, filter);
}

const std::vector<boost::graph_traits<GraphType>::vertex_descriptor> &
get_vertex_order_component(std::size_t i, const LabelledLeftGraph &g) {
	assert(i < get_num_connected_components(g));
	// the number of connected components is initialized externally after construction, so we have this annoying hax
	if(g.vertex_orders.empty()) g.vertex_orders.resize(get_num_connected_components(g));
	if(g.vertex_orders[i].empty()) {
		g.vertex_orders[i] = get_vertex_order(lib::GraphMorphism::DefaultFinderArgsProvider(), get_component_graph(i, g));
	}
	return g.vertex_orders[i];
}

// LabelledRightGraph
//------------------------------------------------------------------------------

LabelledRightGraph::LabelledRightGraph(const LabelledRule &r)
        : Base(r, jla_boost::GraphDPO::Membership::Right) {}

const LabelledRightGraph::GraphType &get_graph(const LabelledRightGraph &g) {
	return get_right(g.r);
}

LabelledRightGraph::PropStringType get_string(const LabelledRightGraph &g) {
	return get_string(g.r).getRight();
}

LabelledRightGraph::PropTermType get_term(const LabelledRightGraph &g) {
	return get_term(g.r).getRight();
}

bool has_stereo(const LabelledRightGraph &g) {
	return has_stereo(g.r);
}

LabelledRightGraph::PropStereoType get_stereo(const LabelledRightGraph &g) {
	return get_stereo(g.r).getRight();
}

const std::vector<std::unique_ptr<LabelledRule::RightMatchConstraint> > &
get_match_constraints(const LabelledRightGraph &g) {
	return g.r.rightMatchConstraints;
}

std::size_t get_num_connected_components(const LabelledRightGraph &g) {
	return g.r.numRightComponents;
}

const std::vector<size_t>& get_connected_component_map(const LabelledRightGraph &g) {
	return g.r.rightComponents;
}

const LabelledRule::ComponentGraph_v2& get_component_graph_v2(std::size_t i, const LabelledRightGraph &g) {
	assert(i < g.r.numRightComponents);
	return g.r.rightComponentGraphs[i];
}

boost::graph_traits<SideGraphType>::vertex_descriptor get_component_core_vertex(std::size_t i, std::size_t v, const LabelledRightGraph &g) {
	assert(i < g.r.numRightComponents);
	assert(v < g.r.rightComponentVertexToCoreVertex[i].size());
	return g.r.rightComponentVertexToCoreVertex[i][v];
}

LabelledRightGraph::Base::ComponentGraph
get_component_graph(std::size_t i, const LabelledRightGraph &g) {
	assert(i < get_num_connected_components(g));
	LabelledRightGraph::Base::ComponentFilter filter(&get_graph(g), &g.r.rightComponents, i);
	return LabelledRightGraph::Base::ComponentGraph(get_graph(g), filter, filter);
}

LabelledRightGraph::PropMoleculeType get_molecule(const LabelledRightGraph &g) {
	return get_molecule(g.r).getRight();
}

const std::vector<boost::graph_traits<GraphType>::vertex_descriptor> &
get_vertex_order_component(std::size_t i, const LabelledRightGraph &g) {
	assert(i < get_num_connected_components(g));
	// the number of connected components is initialized externally after construction, so we have this annoying hax
	if(g.vertex_orders.empty()) g.vertex_orders.resize(get_num_connected_components(g));
	if(g.vertex_orders[i].empty()) {
		g.vertex_orders[i] = get_vertex_order(lib::GraphMorphism::DefaultFinderArgsProvider(), get_component_graph(i, g));
	}
	return g.vertex_orders[i];
}

} // namespace mod::lib::Rules
