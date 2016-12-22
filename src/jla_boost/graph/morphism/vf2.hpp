#ifndef JLA_BOOST_GRAPH_MORPHISM_VF2_HPP
#define JLA_BOOST_GRAPH_MORPHISM_VF2_HPP

#include <jla_boost/graph/morphism/VertexOrderByMult.hpp>

#include <jla_boost/Functional.hpp>
#include <jla_boost/graph/PairToRangeAdaptor.hpp>
#include <jla_boost/graph/morphism/InvertibleVertexMapAdaptor.hpp>
#include <jla_boost/graph/morphism/VectorVertexMap.hpp>

#include <boost/assert.hpp>
#include <boost/concept/assert.hpp>
#include <boost/concept_check.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/type_traits/has_less.hpp>
#include <boost/mpl/int.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/utility/enable_if.hpp>

#include <iostream>
#include <iomanip>
#include <iterator>
#include <vector>
#include <utility>

namespace jla_boost {
namespace GraphMorphism {
using namespace boost; // TODO: remvoe
namespace detail {

template<typename GraphDom, typename GraphCodom, typename IdxDom, typename IdxCodom>
class base_state {
	using VertexMapType = VectorVertexMap<GraphDom, GraphCodom>;
	using VertexDom = typename boost::graph_traits<GraphDom>::vertex_descriptor;
	using VertexCodom = typename boost::graph_traits<GraphCodom>::vertex_descriptor;

	using in_out_map_type = iterator_property_map<typename std::vector<std::size_t>::iterator, IdxDom, std::size_t, std::size_t&>;
public:
	base_state(const base_state&) = delete;
	base_state &operator=(const base_state&) = delete;
public:

	base_state(const GraphDom &gDom, const GraphCodom &gCodom)
	: gDom(gDom), gCodom(gCodom), core_(gDom, gCodom),
	in_vec_(num_vertices(gDom), 0),
	out_vec_(num_vertices(gDom), 0),
	in_(in_vec_.begin(), get(boost::vertex_index_t(), gDom)),
	out_(out_vec_.begin(), get(boost::vertex_index_t(), gDom)),
	term_in_count_(0), term_out_count_(0), term_both_count_(0), preimage_size(0) { }

	// Adds a vertex pair to the state of graph graph_this

	void push(const VertexDom &v_this, const VertexCodom& v_other) {
		++preimage_size;
		put(core_, gDom, gCodom, v_this, v_other);

		if(!get(in_, v_this)) {
			put(in_, v_this, preimage_size);
			++term_in_count_;
			if(get(out_, v_this))
				++term_both_count_;
		}

		if(!get(out_, v_this)) {
			put(out_, v_this, preimage_size);
			++term_out_count_;
			if(get(in_, v_this))
				++term_both_count_;
		}

		for(auto e : asRange(in_edges(v_this, gDom))) {
			auto w = source(e, gDom);
			if(!get(in_, w)) {
				put(in_, w, preimage_size);
				++term_in_count_;
				if(get(out_, w))
					++term_both_count_;
			}
		}

		for(auto e : asRange(out_edges(v_this, gDom))) {
			auto w = target(e, gDom);
			if(!get(out_, w)) {
				put(out_, w, preimage_size);
				++term_out_count_;
				if(get(in_, w))
					++term_both_count_;
			}
		}

	}

	// Removes vertex pair from state of graph_this

	void pop(const VertexDom& v_this, const VertexCodom&) {
		if(!preimage_size) return;

		if(get(in_, v_this) == preimage_size) {
			put(in_, v_this, 0);
			--term_in_count_;
			if(get(out_, v_this))
				--term_both_count_;
		}

		for(auto e : asRange(in_edges(v_this, gDom))) {
			auto w = source(e, gDom);
			if(get(in_, w) == preimage_size) {
				put(in_, w, 0);
				--term_in_count_;
				if(get(out_, w))
					--term_both_count_;
			}
		}

		if(get(out_, v_this) == preimage_size) {
			put(out_, v_this, 0);
			--term_out_count_;
			if(get(in_, v_this))
				--term_both_count_;
		}

		for(auto e : asRange(out_edges(v_this, gDom))) {
			auto w = target(e, gDom);
			if(get(out_, w) == preimage_size) {
				put(out_, w, 0);
				--term_out_count_;
				if(get(in_, w))
					--term_both_count_;
			}
		}
		put(core_, gDom, gCodom, v_this, boost::graph_traits<GraphCodom>::null_vertex());
		--preimage_size;
	}

	// Returns true if the in-terminal set is not empty  

	bool term_in() const {
		return preimage_size < term_in_count_;
	}

	// Returns true if vertex belongs to the in-terminal set

	bool term_in(const VertexDom& v) const {
		return (get(in_, v) > 0) &&
				(get(core_, gDom, gCodom, v) == graph_traits<GraphCodom>::null_vertex());
	}

	// Returns true if the out-terminal set is not empty  

	bool term_out() const {
		return preimage_size < term_out_count_;
	}

	// Returns true if vertex belongs to the out-terminal set

	bool term_out(const VertexDom &v) const {
		return (get(out_, v) > 0) &&
				(get(core_, gDom, gCodom, v) == boost::graph_traits<GraphCodom>::null_vertex());
	}

	// Returns true of both (in- and out-terminal) sets are not empty

	bool term_both() const {
		return preimage_size < term_both_count_;
	}

	// Returns true if vertex belongs to both (in- and out-terminal) sets

	bool term_both(const VertexDom &v) const {
		return (get(in_, v) > 0) && (get(out_, v) > 0) &&
				(get(core_, gDom, gCodom, v) == boost::graph_traits<GraphCodom>::null_vertex());
	}

	// Returns true if vertex belongs to the core map, i.e. it is in the 
	// present mapping

	bool in_core(const VertexDom &v) const {
		return get(core_, gDom, gCodom, v) != boost::graph_traits<GraphCodom>::null_vertex();
	}

	// Returns the number of vertices in the mapping

	std::size_t count() const {
		return preimage_size;
	}

	// Returns the image (in graph_other) of vertex v (in graph_this)

	VertexCodom core(const VertexDom &v) const {
		return get(core_, gDom, gCodom, v);
	}

	// Returns the mapping

	const VertexMapType &get_map() const {
		return core_;
	}

	// Returns the "time" (or depth) when vertex was added to the in-terminal set

	std::size_t in_depth(VertexDom v) const {
		return get(in_, v);
	}

	// Returns the "time" (or depth) when vertex was added to the out-terminal set

	std::size_t out_depth(VertexDom v) const {
		return get(out_, v);
	}

	// Returns the terminal set counts

	boost::tuple<std::size_t, std::size_t, std::size_t>
	term_set() const {
		return boost::make_tuple(term_in_count_, term_out_count_, term_both_count_);
	}
public:
	const GraphDom &gDom;
	const GraphCodom &gCodom;
	VertexMapType core_;

	std::vector<std::size_t> in_vec_, out_vec_;
	in_out_map_type in_, out_;

	std::size_t term_in_count_, term_out_count_, term_both_count_, preimage_size;
};


// Function object that checks whether a valid edge
// exists. For multi-graphs matched edges are excluded  

template <typename Graph, typename Enable = void>
struct equivalent_edge_exists {
	typedef typename boost::graph_traits<Graph>::edge_descriptor edge_type;

	BOOST_CONCEPT_ASSERT((LessThanComparable<edge_type>));

	template<typename EdgePredicate>
	bool operator()(typename graph_traits<Graph>::vertex_descriptor s,
			typename graph_traits<Graph>::vertex_descriptor t,
			EdgePredicate is_valid_edge, const Graph& g) {

		BGL_FORALL_OUTEDGES_T(s, e, g, Graph) {
			if((target(e, g) == t) && is_valid_edge(e) &&
					(matched_edges_.find(e) == matched_edges_.end())) {
				matched_edges_.insert(e);
				return true;
			}
		}

		return false;
	}

private:

	std::set<edge_type> matched_edges_;
};

template <typename Graph>
struct equivalent_edge_exists<Graph, typename boost::disable_if<is_multigraph<Graph> >::type> {

	template<typename EdgePredicate>
	bool operator()(typename graph_traits<Graph>::vertex_descriptor s,
			typename graph_traits<Graph>::vertex_descriptor t,
			EdgePredicate is_valid_edge, const Graph& g) {

		typename graph_traits<Graph>::edge_descriptor e;
		bool found;
		boost::tie(e, found) = edge(s, t, g);
		if(!found)
			return false;
		else if(is_valid_edge(e))
			return true;

		return false;
	}

};


// Generates a predicate for edge e1 given  a binary predicate and a 
// fixed edge e2

template<typename Graph, typename EdgePred>
struct edge1_predicate {

	edge1_predicate(EdgePred edgePred, typename boost::graph_traits<Graph>::edge_descriptor e)
	: edgePred(edgePred), e(e) { }

	template<typename Edge>
	bool operator()(const Edge &e1) {
		return edgePred(e1, e);
	}

	EdgePred edgePred;
	typename boost::graph_traits<Graph>::edge_descriptor e;
};


// Generates a predicate for edge e2 given given a binary predicate and a
// fixed edge e1

template <typename Graph1,
typename Graph2,
typename EdgeEquivalencePredicate>
struct edge2_predicate {

	edge2_predicate(EdgeEquivalencePredicate edge_comp,
			typename graph_traits<Graph1>::edge_descriptor e1)
	: edge_comp_(edge_comp), e1_(e1) { }

	bool operator()(typename graph_traits<Graph2>::edge_descriptor e2) {
		return edge_comp_(e1_, e2);
	}

	EdgeEquivalencePredicate edge_comp_;
	typename graph_traits<Graph1>::edge_descriptor e1_;
};

enum problem_selector {
	subgraph_mono, subgraph_iso, isomorphism
};

// The actual state associated with both graphs

template<typename GraphDom, typename GraphCodom,
typename IndexMap1, typename IndexMap2,
typename EdgePred, typename VertexPred,
typename Callback,
problem_selector problem_selection>
class state {
	typedef typename graph_traits<GraphDom>::vertex_descriptor vertex1_type;
	typedef typename graph_traits<GraphCodom>::vertex_descriptor vertex2_type;

	typedef typename graph_traits<GraphDom>::edge_descriptor edge1_type;
	typedef typename graph_traits<GraphCodom>::edge_descriptor edge2_type;

	typedef typename graph_traits<GraphDom>::vertices_size_type graph1_size_type;
	typedef typename graph_traits<GraphCodom>::vertices_size_type graph2_size_type;
public:

	// Three helper functions used in Feasibility and Valid functions to test
	// terminal set counts when testing for:
	// - graph sub-graph monomorphism, or

	inline bool comp_term_sets(graph1_size_type a,
			graph2_size_type b,
			boost::mpl::int_<subgraph_mono>) const {
		return a <= b;
	}

	// - graph sub-graph isomorphism, or

	inline bool comp_term_sets(graph1_size_type a,
			graph2_size_type b,
			boost::mpl::int_<subgraph_iso>) const {
		return a <= b;
	}

	// - graph isomorphism

	inline bool comp_term_sets(graph1_size_type a,
			graph2_size_type b,
			boost::mpl::int_<isomorphism>) const {
		return a == b;
	}

	state(const state&) = delete;
	state &operator=(const state&) = delete;
public:

	state(const GraphDom &gDom, const GraphCodom &gCodom, EdgePred edgePred, VertexPred vertexPred)
	: gDom(gDom), gCodom(gCodom), edgePred(edgePred), vertexPred(vertexPred),
	stateDom(gDom, gCodom), stateCodom(gCodom, gDom) { }

	// Add vertex pair to the state

	void push(const vertex1_type& v, const vertex2_type& w) {
		stateDom.push(v, w);
		stateCodom.push(w, v);
	}

	// Remove vertex pair from state

	void pop(const vertex1_type& v, const vertex2_type&) {
		vertex2_type w = stateDom.core(v);
		stateDom.pop(v, w);
		stateCodom.pop(w, v);
	}

	// Checks the feasibility of a new vertex pair

	bool feasible(const vertex1_type& v_new, const vertex2_type& w_new) {

		if(!vertexPred(v_new, w_new)) return false;

		// graph1
		graph1_size_type term_in1_count = 0, term_out1_count = 0, rest1_count = 0;

		{
			equivalent_edge_exists<GraphCodom> edge2_exists;

			BGL_FORALL_INEDGES_T(v_new, e1, gDom, GraphDom) {
				vertex1_type v = source(e1, gDom);

				if(stateDom.in_core(v) || (v == v_new)) {
					vertex2_type w = w_new;
					if(v != v_new)
						w = stateDom.core(v);
					if(!edge2_exists(w, w_new,
							edge2_predicate<GraphDom, GraphCodom, EdgePred>(edgePred, e1),
							gCodom))
						return false;

				} else {
					if(0 < stateDom.in_depth(v))
						++term_in1_count;
					if(0 < stateDom.out_depth(v))
						++term_out1_count;
					if((stateDom.in_depth(v) == 0) && (stateDom.out_depth(v) == 0))
						++rest1_count;
				}
			}
		}

		{
			equivalent_edge_exists<GraphCodom> edge2_exists;

			BGL_FORALL_OUTEDGES_T(v_new, e1, gDom, GraphDom) {
				vertex1_type v = target(e1, gDom);
				if(stateDom.in_core(v) || (v == v_new)) {
					vertex2_type w = w_new;
					if(v != v_new)
						w = stateDom.core(v);

					if(!edge2_exists(w_new, w,
							edge2_predicate<GraphDom, GraphCodom, EdgePred>(edgePred, e1),
							gCodom))
						return false;

				} else {
					if(0 < stateDom.in_depth(v))
						++term_in1_count;
					if(0 < stateDom.out_depth(v))
						++term_out1_count;
					if((stateDom.in_depth(v) == 0) && (stateDom.out_depth(v) == 0))
						++rest1_count;
				}
			}
		}

		// graph2
		graph2_size_type term_out2_count = 0, term_in2_count = 0, rest2_count = 0;

		{
			equivalent_edge_exists<GraphDom> edge1_exists;

			BGL_FORALL_INEDGES_T(w_new, e2, gCodom, GraphCodom) {
				vertex2_type w = source(e2, gCodom);
				if(stateCodom.in_core(w) || (w == w_new)) {
					if(problem_selection != subgraph_mono) {
						vertex1_type v = v_new;
						if(w != w_new)
							v = stateCodom.core(w);

						if(!edge1_exists(v, v_new,
								edge1_predicate<GraphCodom, EdgePred>(edgePred, e2),
								gDom))
							return false;
					}
				} else {
					if(0 < stateCodom.in_depth(w))
						++term_in2_count;
					if(0 < stateCodom.out_depth(w))
						++term_out2_count;
					if((stateCodom.in_depth(w) == 0) && (stateCodom.out_depth(w) == 0))
						++rest2_count;
				}
			}
		}

		{
			equivalent_edge_exists<GraphDom> edge1_exists;

			BGL_FORALL_OUTEDGES_T(w_new, e2, gCodom, GraphCodom) {
				vertex2_type w = target(e2, gCodom);
				if(stateCodom.in_core(w) || (w == w_new)) {
					if(problem_selection != subgraph_mono) {
						vertex1_type v = v_new;
						if(w != w_new)
							v = stateCodom.core(w);

						if(!edge1_exists(v_new, v,
								edge1_predicate<GraphCodom, EdgePred>(edgePred, e2),
								gDom))
							return false;
					}
				} else {
					if(0 < stateCodom.in_depth(w))
						++term_in2_count;
					if(0 < stateCodom.out_depth(w))
						++term_out2_count;
					if((stateCodom.in_depth(w) == 0) && (stateCodom.out_depth(w) == 0))
						++rest2_count;
				}
			}
		}

		if(problem_selection != subgraph_mono) { // subgraph_iso and isomorphism
			return comp_term_sets(term_in1_count, term_in2_count,
					boost::mpl::int_<problem_selection>()) &&
					comp_term_sets(term_out1_count, term_out2_count,
					boost::mpl::int_<problem_selection>()) &&
					comp_term_sets(rest1_count, rest2_count,
					boost::mpl::int_<problem_selection>());
		} else { // subgraph_mono
			return comp_term_sets(term_in1_count, term_in2_count,
					boost::mpl::int_<problem_selection>()) &&
					comp_term_sets(term_out1_count, term_out2_count,
					boost::mpl::int_<problem_selection>()) &&
					comp_term_sets(term_in1_count + term_out1_count + rest1_count,
					term_in2_count + term_out2_count + rest2_count,
					boost::mpl::int_<problem_selection>());
		}
	}

	// Returns true if vertex v in graph1 is a possible candidate to
	// be added to the current state

	bool possible_candidate1(const vertex1_type& v) const {
		if(stateDom.term_both() && stateCodom.term_both())
			return stateDom.term_both(v);
		else if(stateDom.term_out() && stateCodom.term_out())
			return stateDom.term_out(v);
		else if(stateDom.term_in() && stateCodom.term_in())
			return stateDom.term_in(v);
		else
			return !stateDom.in_core(v);
	}

	// Returns true if vertex w in graph2 is a possible candidate to
	// be added to the current state

	bool possible_candidate2(const vertex2_type& w) const {
		if(stateDom.term_both() && stateCodom.term_both())
			return stateCodom.term_both(w);
		else if(stateDom.term_out() && stateCodom.term_out())
			return stateCodom.term_out(w);
		else if(stateDom.term_in() && stateCodom.term_in())
			return stateCodom.term_in(w);
		else
			return !stateCodom.in_core(w);
	}

	// Returns true if a mapping was found

	bool success() const {
		return stateDom.count() == num_vertices(gDom);
	}

	// Returns true if a state is valid

	bool valid() const {
		boost::tuple<graph1_size_type, graph1_size_type, graph1_size_type> term1;
		boost::tuple<graph2_size_type, graph2_size_type, graph2_size_type> term2;

		term1 = stateDom.term_set();
		term2 = stateCodom.term_set();

		return comp_term_sets(boost::get<0>(term1), boost::get<0>(term2),
				boost::mpl::int_<problem_selection>()) &&
				comp_term_sets(boost::get<1>(term1), boost::get<1>(term2),
				boost::mpl::int_<problem_selection>()) &&
				comp_term_sets(boost::get<2>(term1), boost::get<2>(term2),
				boost::mpl::int_<problem_selection>());
	}

	// Calls the user_callback with a graph (sub)graph mapping 

	bool call_back(Callback user_callback) const {
		return user_callback(
				makeInvertibleVertexMapAdaptor(std::cref(stateDom.get_map()), std::cref(stateCodom.get_map())),
				gDom, gCodom);
	}
public:
	const GraphDom &gDom;
	const GraphCodom &gCodom;

	//	IndexMap1 index_map1_;

	EdgePred edgePred;
	VertexPred vertexPred;

	base_state<GraphDom, GraphCodom, IndexMap1, IndexMap2> stateDom;
	base_state<GraphCodom, GraphDom, IndexMap2, IndexMap1> stateCodom;
};


// Data structure to keep info used for back tracking during
// matching process

template<typename Graph1,
typename Graph2,
typename VertexOrder1>
struct vf2_match_continuation {
	typename VertexOrder1::const_iterator graph1_verts_iter;
	typename graph_traits<Graph2>::vertex_iterator graph2_verts_iter;
};

// Non-recursive method that explores state space using a depth-first
// search strategy.  At each depth possible pairs candidate are compute
// and tested for feasibility to extend the mapping. If a complete
// mapping is found, the mapping is output to user_callback in the form
// of a correspondence map (graph1 to graph2). Returning false from the
// user_callback will terminate the search. Function match will return
// true if the entire search space was explored.

template<typename Graph1,
typename Graph2,
typename IndexMap1,
typename IndexMap2,
typename VertexOrder1,
typename EdgeEquivalencePredicate,
typename VertexEquivalencePredicate,
typename SubGraphIsoMapCallback,
problem_selector problem_selection>
bool match(const Graph1& graph1, const Graph2& graph2,
		SubGraphIsoMapCallback user_callback, const VertexOrder1& vertex_order1,
		state<Graph1, Graph2, IndexMap1, IndexMap2,
		EdgeEquivalencePredicate, VertexEquivalencePredicate,
		SubGraphIsoMapCallback, problem_selection>& s) {

	typename VertexOrder1::const_iterator graph1_verts_iter;

	typedef typename graph_traits<Graph2>::vertex_iterator vertex2_iterator_type;
	vertex2_iterator_type graph2_verts_iter, graph2_verts_iter_end;

	typedef vf2_match_continuation<Graph1, Graph2, VertexOrder1> match_continuation_type;
	std::vector<match_continuation_type> k;
	bool found_match = false;

recur:
	if(s.success()) {
		if(!s.call_back(user_callback))
			return true;
		found_match = true;

		goto back_track;
	}

	if(!s.valid())
		goto back_track;

	graph1_verts_iter = vertex_order1.begin();
	while(graph1_verts_iter != vertex_order1.end() &&
			!s.possible_candidate1(*graph1_verts_iter)) {
		++graph1_verts_iter;
	}

	boost::tie(graph2_verts_iter, graph2_verts_iter_end) = vertices(graph2);
	while(graph2_verts_iter != graph2_verts_iter_end) {
		if(s.possible_candidate2(*graph2_verts_iter)) {
			if(s.feasible(*graph1_verts_iter, *graph2_verts_iter)) {
				match_continuation_type kk;
				kk.graph1_verts_iter = graph1_verts_iter;
				kk.graph2_verts_iter = graph2_verts_iter;
				k.push_back(kk);

				s.push(*graph1_verts_iter, *graph2_verts_iter);
				goto recur;
			}
		}
graph2_loop:
		++graph2_verts_iter;
	}

back_track:
	if(k.empty())
		return found_match;

	const match_continuation_type kk = k.back();
	graph1_verts_iter = kk.graph1_verts_iter;
	graph2_verts_iter = kk.graph2_verts_iter;
	k.pop_back();

	s.pop(*graph1_verts_iter, *graph2_verts_iter);

	goto graph2_loop;
}

// Enumerates all graph sub-graph mono-/iso-morphism mappings between graphs
// graph_small and graph_large. Continues until user_callback returns true or the
// search space has been fully explored.

template <problem_selector problem_selection,
typename GraphSmall,
typename GraphLarge,
typename IndexMapSmall,
typename IndexMapLarge,
typename VertexOrderSmall,
typename EdgeEquivalencePredicate,
typename VertexEquivalencePredicate,
typename SubGraphIsoMapCallback>
bool vf2_subgraph_morphism(const GraphSmall& graph_small, const GraphLarge& graph_large,
		SubGraphIsoMapCallback user_callback,
		IndexMapSmall index_map_small, IndexMapLarge index_map_large,
		const VertexOrderSmall& vertex_order_small,
		EdgeEquivalencePredicate edge_comp,
		VertexEquivalencePredicate vertex_comp) {

	// Graph requirements
	BOOST_CONCEPT_ASSERT((BidirectionalGraphConcept<GraphSmall>));
	BOOST_CONCEPT_ASSERT((VertexListGraphConcept<GraphSmall>));
	BOOST_CONCEPT_ASSERT((EdgeListGraphConcept<GraphSmall>));
	BOOST_CONCEPT_ASSERT((AdjacencyMatrixConcept<GraphSmall>));

	BOOST_CONCEPT_ASSERT((BidirectionalGraphConcept<GraphLarge>));
	BOOST_CONCEPT_ASSERT((VertexListGraphConcept<GraphLarge>));
	BOOST_CONCEPT_ASSERT((EdgeListGraphConcept<GraphLarge>));
	BOOST_CONCEPT_ASSERT((AdjacencyMatrixConcept<GraphLarge>));

	typedef typename graph_traits<GraphSmall>::vertex_descriptor vertex_small_type;
	typedef typename graph_traits<GraphLarge>::vertex_descriptor vertex_large_type;

	typedef typename graph_traits<GraphSmall>::vertices_size_type size_type_small;
	typedef typename graph_traits<GraphLarge>::vertices_size_type size_type_large;

	// Property map requirements
	BOOST_CONCEPT_ASSERT((ReadablePropertyMapConcept<IndexMapSmall, vertex_small_type>));
	typedef typename property_traits<IndexMapSmall>::value_type IndexMapSmallValue;
	BOOST_STATIC_ASSERT((is_convertible<IndexMapSmallValue, size_type_small>::value));

	BOOST_CONCEPT_ASSERT((ReadablePropertyMapConcept<IndexMapLarge, vertex_large_type>));
	typedef typename property_traits<IndexMapLarge>::value_type IndexMapLargeValue;
	BOOST_STATIC_ASSERT((is_convertible<IndexMapLargeValue, size_type_large>::value));

	// Edge & vertex requirements
	typedef typename graph_traits<GraphSmall>::edge_descriptor edge_small_type;
	typedef typename graph_traits<GraphLarge>::edge_descriptor edge_large_type;

	BOOST_CONCEPT_ASSERT((BinaryPredicateConcept<EdgeEquivalencePredicate,
			edge_small_type, edge_large_type>));

	BOOST_CONCEPT_ASSERT((BinaryPredicateConcept<VertexEquivalencePredicate,
			vertex_small_type, vertex_large_type>));

	// Vertex order requirements
	BOOST_CONCEPT_ASSERT((ContainerConcept<VertexOrderSmall>));
	typedef typename VertexOrderSmall::value_type order_value_type;
	BOOST_STATIC_ASSERT((is_same<vertex_small_type, order_value_type>::value));
	BOOST_ASSERT(num_vertices(graph_small) == vertex_order_small.size());

	if(num_vertices(graph_small) > num_vertices(graph_large))
		return false;

	typename graph_traits<GraphSmall>::edges_size_type num_edges_small = num_edges(graph_small);
	typename graph_traits<GraphLarge>::edges_size_type num_edges_large = num_edges(graph_large);

	// Double the number of edges for undirected graphs: each edge counts as
	// in-edge and out-edge
	if(is_undirected(graph_small)) num_edges_small *= 2;
	if(is_undirected(graph_large)) num_edges_large *= 2;
	if(num_edges_small > num_edges_large)
		return false;

	detail::state<GraphSmall, GraphLarge, IndexMapSmall, IndexMapLarge,
			EdgeEquivalencePredicate, VertexEquivalencePredicate,
			SubGraphIsoMapCallback, problem_selection>
			s(graph_small, graph_large, edge_comp, vertex_comp);

	return detail::match(graph_small, graph_large, user_callback, vertex_order_small, s);
}

} // namespace detail

// Enumerates all graph sub-graph monomorphism mappings between graphs
// graph_small and graph_large. Continues until user_callback returns true or the
// search space has been fully explored.

template <typename GraphSmall,
typename GraphLarge,
typename IndexMapSmall,
typename IndexMapLarge,
typename VertexOrderSmall,
typename EdgeEquivalencePredicate,
typename VertexEquivalencePredicate,
typename SubGraphIsoMapCallback>
bool vf2_subgraph_mono(const GraphSmall& graph_small, const GraphLarge& graph_large,
		SubGraphIsoMapCallback user_callback,
		IndexMapSmall index_map_small, IndexMapLarge index_map_large,
		const VertexOrderSmall& vertex_order_small,
		EdgeEquivalencePredicate edge_comp,
		VertexEquivalencePredicate vertex_comp) {
	return detail::vf2_subgraph_morphism<detail::subgraph_mono>
			(graph_small, graph_large,
			user_callback,
			index_map_small, index_map_large,
			vertex_order_small,
			edge_comp,
			vertex_comp);
}


// All default interface for vf2_subgraph_iso

template <typename GraphSmall,
typename GraphLarge,
typename SubGraphIsoMapCallback>
bool vf2_subgraph_mono(const GraphSmall& graph_small, const GraphLarge& graph_large,
		SubGraphIsoMapCallback user_callback) {
	return vf2_subgraph_mono(graph_small, graph_large, user_callback,
			get(vertex_index, graph_small), get(vertex_index, graph_large),
			vertex_order_by_mult(graph_small),
			AlwaysTrue(), AlwaysTrue());
}


// Named parameter interface of vf2_subgraph_iso

template <typename GraphSmall,
typename GraphLarge,
typename VertexOrderSmall,
typename SubGraphIsoMapCallback,
typename Param,
typename Tag,
typename Rest>
bool vf2_subgraph_mono(const GraphSmall& graph_small, const GraphLarge& graph_large,
		SubGraphIsoMapCallback user_callback,
		const VertexOrderSmall& vertex_order_small,
		const bgl_named_params<Param, Tag, Rest>& params) {
	return vf2_subgraph_mono(graph_small, graph_large, user_callback,
			choose_const_pmap(get_param(params, vertex_index1),
			graph_small, vertex_index),
			choose_const_pmap(get_param(params, vertex_index2),
			graph_large, vertex_index),
			vertex_order_small,
			choose_param(get_param(params, edges_equivalent_t()),
			AlwaysTrue()),
			choose_param(get_param(params, vertices_equivalent_t()),
			AlwaysTrue())
			);
}


// Enumerates all graph sub-graph isomorphism mappings between graphs
// graph_small and graph_large. Continues until user_callback returns true or the
// search space has been fully explored.

template <typename GraphSmall,
typename GraphLarge,
typename IndexMapSmall,
typename IndexMapLarge,
typename VertexOrderSmall,
typename EdgeEquivalencePredicate,
typename VertexEquivalencePredicate,
typename SubGraphIsoMapCallback>
bool vf2_subgraph_iso(const GraphSmall& graph_small, const GraphLarge& graph_large,
		SubGraphIsoMapCallback user_callback,
		IndexMapSmall index_map_small, IndexMapLarge index_map_large,
		const VertexOrderSmall& vertex_order_small,
		EdgeEquivalencePredicate edge_comp,
		VertexEquivalencePredicate vertex_comp) {
	return detail::vf2_subgraph_morphism<detail::subgraph_iso>
			(graph_small, graph_large,
			user_callback,
			index_map_small, index_map_large,
			vertex_order_small,
			edge_comp,
			vertex_comp);
}


// All default interface for vf2_subgraph_iso

template <typename GraphSmall,
typename GraphLarge,
typename SubGraphIsoMapCallback>
bool vf2_subgraph_iso(const GraphSmall& graph_small, const GraphLarge& graph_large,
		SubGraphIsoMapCallback user_callback) {

	return vf2_subgraph_iso(graph_small, graph_large, user_callback,
			get(vertex_index, graph_small), get(vertex_index, graph_large),
			vertex_order_by_mult(graph_small),
			AlwaysTrue(), AlwaysTrue());
}


// Named parameter interface of vf2_subgraph_iso

template <typename GraphSmall,
typename GraphLarge,
typename VertexOrderSmall,
typename SubGraphIsoMapCallback,
typename Param,
typename Tag,
typename Rest>
bool vf2_subgraph_iso(const GraphSmall& graph_small, const GraphLarge& graph_large,
		SubGraphIsoMapCallback user_callback,
		const VertexOrderSmall& vertex_order_small,
		const bgl_named_params<Param, Tag, Rest>& params) {

	return vf2_subgraph_iso(graph_small, graph_large, user_callback,
			choose_const_pmap(get_param(params, vertex_index1),
			graph_small, vertex_index),
			choose_const_pmap(get_param(params, vertex_index2),
			graph_large, vertex_index),
			vertex_order_small,
			choose_param(get_param(params, edges_equivalent_t()),
			AlwaysTrue()),
			choose_param(get_param(params, vertices_equivalent_t()),
			AlwaysTrue())
			);

}


// Enumerates all isomorphism mappings between graphs graph1_ and graph2_.
// Continues until user_callback returns true or the search space has been
// fully explored.

template <typename Graph1,
typename Graph2,
typename IndexMap1,
typename IndexMap2,
typename VertexOrder1,
typename EdgeEquivalencePredicate,
typename VertexEquivalencePredicate,
typename GraphIsoMapCallback>
bool vf2_graph_iso(const Graph1& graph1, const Graph2& graph2,
		GraphIsoMapCallback user_callback,
		IndexMap1 index_map1, IndexMap2 index_map2,
		const VertexOrder1& vertex_order1,
		EdgeEquivalencePredicate edge_comp,
		VertexEquivalencePredicate vertex_comp) {

	// Graph requirements
	BOOST_CONCEPT_ASSERT((BidirectionalGraphConcept<Graph1>));
	BOOST_CONCEPT_ASSERT((VertexListGraphConcept<Graph1>));
	BOOST_CONCEPT_ASSERT((EdgeListGraphConcept<Graph1>));
	BOOST_CONCEPT_ASSERT((AdjacencyMatrixConcept<Graph1>));

	BOOST_CONCEPT_ASSERT((BidirectionalGraphConcept<Graph2>));
	BOOST_CONCEPT_ASSERT((VertexListGraphConcept<Graph2>));
	BOOST_CONCEPT_ASSERT((EdgeListGraphConcept<Graph2>));
	BOOST_CONCEPT_ASSERT((AdjacencyMatrixConcept<Graph2>));


	typedef typename graph_traits<Graph1>::vertex_descriptor vertex1_type;
	typedef typename graph_traits<Graph2>::vertex_descriptor vertex2_type;

	typedef typename graph_traits<Graph1>::vertices_size_type size_type1;
	typedef typename graph_traits<Graph2>::vertices_size_type size_type2;

	// Property map requirements
	BOOST_CONCEPT_ASSERT((ReadablePropertyMapConcept<IndexMap1, vertex1_type>));
	typedef typename property_traits<IndexMap1>::value_type IndexMap1Value;
	BOOST_STATIC_ASSERT((is_convertible<IndexMap1Value, size_type1>::value));

	BOOST_CONCEPT_ASSERT((ReadablePropertyMapConcept<IndexMap2, vertex2_type>));
	typedef typename property_traits<IndexMap2>::value_type IndexMap2Value;
	BOOST_STATIC_ASSERT((is_convertible<IndexMap2Value, size_type2>::value));

	// Edge & vertex requirements
	typedef typename graph_traits<Graph1>::edge_descriptor edge1_type;
	typedef typename graph_traits<Graph2>::edge_descriptor edge2_type;

	BOOST_CONCEPT_ASSERT((BinaryPredicateConcept<EdgeEquivalencePredicate,
			edge1_type, edge2_type>));

	BOOST_CONCEPT_ASSERT((BinaryPredicateConcept<VertexEquivalencePredicate,
			vertex1_type, vertex2_type>));

	// Vertex order requirements
	BOOST_CONCEPT_ASSERT((ContainerConcept<VertexOrder1>));
	typedef typename VertexOrder1::value_type order_value_type;
	BOOST_STATIC_ASSERT((is_same<vertex1_type, order_value_type>::value));
	BOOST_ASSERT(num_vertices(graph1) == vertex_order1.size());

	if(num_vertices(graph1) != num_vertices(graph2))
		return false;

	typename graph_traits<Graph1>::edges_size_type num_edges1 = num_edges(graph1);
	typename graph_traits<Graph2>::edges_size_type num_edges2 = num_edges(graph2);

	// Double the number of edges for undirected graphs: each edge counts as
	// in-edge and out-edge
	if(is_undirected(graph1)) num_edges1 *= 2;
	if(is_undirected(graph2)) num_edges2 *= 2;
	if(num_edges1 != num_edges2)
		return false;

	detail::state<Graph1, Graph2, IndexMap1, IndexMap2,
			EdgeEquivalencePredicate, VertexEquivalencePredicate,
			GraphIsoMapCallback, detail::isomorphism>
			s(graph1, graph2, edge_comp, vertex_comp);

	return detail::match(graph1, graph2, user_callback, vertex_order1, s);
}


// All default interface for vf2_graph_iso

template <typename Graph1,
typename Graph2,
typename GraphIsoMapCallback>
bool vf2_graph_iso(const Graph1& graph1, const Graph2& graph2,
		GraphIsoMapCallback user_callback) {

	return vf2_graph_iso(graph1, graph2, user_callback,
			get(vertex_index, graph1), get(vertex_index, graph2),
			vertex_order_by_mult(graph1),
			AlwaysTrue(), AlwaysTrue());
}


// Named parameter interface of vf2_graph_iso

template <typename Graph1,
typename Graph2,
typename VertexOrder1,
typename GraphIsoMapCallback,
typename Param,
typename Tag,
typename Rest>
bool vf2_graph_iso(const Graph1& graph1, const Graph2& graph2,
		GraphIsoMapCallback user_callback,
		const VertexOrder1& vertex_order1,
		const bgl_named_params<Param, Tag, Rest>& params) {

	return vf2_graph_iso(graph1, graph2, user_callback,
			choose_const_pmap(get_param(params, vertex_index1),
			graph1, vertex_index),
			choose_const_pmap(get_param(params, vertex_index2),
			graph2, vertex_index),
			vertex_order1,
			choose_param(get_param(params, edges_equivalent_t()),
			AlwaysTrue()),
			choose_param(get_param(params, vertices_equivalent_t()),
			AlwaysTrue())
			);

}


// Verifies a graph (sub)graph isomorphism map 

template<typename Graph1,
typename Graph2,
typename CorresponenceMap1To2,
typename EdgeEquivalencePredicate,
typename VertexEquivalencePredicate>
inline bool verify_vf2_subgraph_iso(const Graph1& graph1, const Graph2& graph2,
		const CorresponenceMap1To2 f,
		EdgeEquivalencePredicate edge_comp,
		VertexEquivalencePredicate vertex_comp) {

	BOOST_CONCEPT_ASSERT((EdgeListGraphConcept<Graph1>));
	BOOST_CONCEPT_ASSERT((AdjacencyMatrixConcept<Graph2>));

	detail::equivalent_edge_exists<Graph2> edge2_exists;

	BGL_FORALL_EDGES_T(e1, graph1, Graph1) {
		typename graph_traits<Graph1>::vertex_descriptor s1, t1;
		typename graph_traits<Graph2>::vertex_descriptor s2, t2;

		s1 = source(e1, graph1);
		t1 = target(e1, graph1);
		s2 = get(f, graph1, graph2, s1);
		t2 = get(f, graph1, graph2, t1);

		if(!vertex_comp(s1, s2) || !vertex_comp(t1, t2))
			return false;

		typename graph_traits<Graph2>::edge_descriptor e2;

		if(!edge2_exists(s2, t2,
				detail::edge2_predicate<Graph1, Graph2, EdgeEquivalencePredicate>(edge_comp, e1),
				graph2))
			return false;

	}

	return true;
}

// Variant of verify_subgraph_iso with all default parameters

template<typename Graph1,
typename Graph2,
typename CorresponenceMap1To2>
inline bool verify_vf2_subgraph_iso(const Graph1& graph1, const Graph2& graph2,
		const CorresponenceMap1To2 f) {
	return verify_vf2_subgraph_iso(graph1, graph2, f,
			AlwaysTrue(), AlwaysTrue());
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif // JLA_BOOST_GRAPH_MORPHISM_VF2_HPP