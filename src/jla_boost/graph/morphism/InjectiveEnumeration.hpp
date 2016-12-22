#ifndef JLA_BOOST_GRAPH_MORPHISM_INJECTIVEENUMERATION_HPP
#define JLA_BOOST_GRAPH_MORPHISM_INJECTIVEENUMERATION_HPP

#include <jla_boost/graph/morphism/VectorVertexMap.hpp>

// debug
//#include <iostream>

namespace jla_boost {
namespace GraphMorphism {
using namespace boost; // TODO: remvoe

// The stack is partitioned into 3 sections, in the following order from the bottom:
// 1: The unchecked matches.
//    The vertices mapped by the user, which may not abide the defined constraints.
// 2: The checked matches.
//    The vertices mapped by the user, which fulfill the defined constraints.
//    preTryPush, prePop
// 3: The dynamic match.
//    The vertices mapped by the enumeration algorithm.
//    tryPush, pop
//
// getStackSize returns the size of the complete stack

template<typename Derived, typename GraphLeft, typename GraphRight, typename EdgePred, typename VertexPred>
struct InjectiveEnumerationState {
	using VertexLeft = typename boost::graph_traits<GraphLeft>::vertex_descriptor;
	using VertexRight = typename boost::graph_traits<GraphRight>::vertex_descriptor;
	using EdgeLeft = typename boost::graph_traits<GraphLeft>::edge_descriptor;
	using EdgeRight = typename boost::graph_traits<GraphRight>::edge_descriptor;
public:

	static VertexLeft vNullLeft() {
		return boost::graph_traits<GraphLeft>::null_vertex();
	}

	static VertexRight vNullRight() {
		return boost::graph_traits<GraphRight>::null_vertex();
	}
protected:

	InjectiveEnumerationState(const GraphLeft &gLeft, const GraphRight &gRight, EdgePred edgePred, VertexPred vertexPred)
	: gLeft(gLeft), gRight(gRight), edgePred(edgePred), vertexPred(vertexPred),
	// alg state 
	m(gLeft, gRight), mInverse(gRight, gLeft) { }
private:

	void pushImpl(VertexLeft vLeft, VertexRight vRight) {
		assert(rightFromLeft(vLeft) == vNullRight());
		assert(leftFromRight(vRight) == vNullLeft());
		put(m, gLeft, gRight, vLeft, vRight);
		put(mInverse, gRight, gLeft, vRight, vLeft);
		stack.emplace_back(vLeft, vRight);
	}

	std::pair<VertexLeft, VertexRight> popNoAsserts() {
		auto p = stack.back();
		put(m, gLeft, gRight, p.first, vNullRight());
		put(mInverse, gRight, gLeft, p.second, vNullLeft());
		stack.pop_back();
		return p;
	}
public:

	std::size_t getStackSize() {
		return stack.size();
	}

	VertexRight rightFromLeft(VertexLeft vLeft) const {
		return get(m, gLeft, gRight, vLeft);
	}

	VertexLeft leftFromRight(VertexRight vRight) const {
		return get(mInverse, gRight, gLeft, vRight);
	}

	auto getVertexMap() const {
		return makeInvertibleVertexMapAdaptor(std::cref(m), std::cref(mInverse));
	}

	void preForcePush(VertexLeft vLeft, VertexRight vRight) {
		assert(uncheckedEnd == checkedEnd); // don't do this after preTryPush
		assert(stack.size() == uncheckedEnd); // or during enumeration
		pushImpl(vLeft, vRight);
		++checkedEnd;
		++uncheckedEnd;
	}

	std::pair<VertexLeft, VertexRight> preForcePop() {
		assert(stack.size() == uncheckedEnd); // don't do this during enumeration
		assert(uncheckedEnd == checkedEnd); // and only if not preTryPush has succeeded
		assert(!stack.empty()); // and of course only if there is anything
		--uncheckedEnd;
		--checkedEnd;
		return popNoAsserts();
	}

	// returns true iff the push succeeded

	bool preTryPush(VertexLeft vLeft, VertexRight vRight) {
		assert(stack.size() == checkedEnd); // don't do this during enumeration
		bool res = tryPush(vLeft, vRight);
		if(res) ++checkedEnd;
		return res;
	}

	std::pair<VertexLeft, VertexRight> prePop() {
		assert(stack.size() == checkedEnd); // don't do this during enumeration
		assert(stack.size() > uncheckedEnd); // and only if preTryPush has been used
		--checkedEnd;
		return popNoAsserts();
	}

	// returns true iff the push succeeded

	bool tryPush(VertexLeft vLeft, VertexRight vRight) {
		assert(rightFromLeft(vLeft) == vNullRight());
		assert(leftFromRight(vRight) == vNullLeft());
		//		std::cout << "tryPush(" << vLeft << ", " << vRight << "):\n";
		if(!vertexPred(vLeft, vRight)) {
			//			std::cout << "\tfailed: vertexPred\n";
			return false;
		}

		// TODO: rewrite this to use out_edges instead
		// Verify edges with existing sub-graph
		for(auto vOtherLeft : asRange(vertices(this->gLeft))) {
			auto vOtherRight = get(this->m, this->gLeft, this->gRight, vOtherLeft);
			// Skip unassociated vertices
			if(vOtherRight == vNullRight()) continue;

			// check in-edges of vLeft
			//------------------------------------------------------------------------

			// NOTE: This will not work with parallel edges, since the
			// first matching edge is always chosen.
			EdgeLeft edge_to_new1, edge_from_new1;
			bool edge_to_new_exists1 = false, edge_from_new_exists1 = false;

			EdgeRight edge_to_new2, edge_from_new2;
			bool edge_to_new_exists2 = false, edge_from_new_exists2 = false;

			// Search for edge from existing to new vertex (gLeft)
			for(auto eOutLeft : asRange(out_edges(vOtherLeft, this->gLeft))) {
				if(target(eOutLeft, this->gLeft) == vLeft) {
					edge_to_new1 = eOutLeft;
					edge_to_new_exists1 = true;
					break;
				}
			}

			// Search for edge from existing to new vertex (gRight)
			for(auto eOutRight : asRange(out_edges(vOtherRight, this->gRight))) {
				if(target(eOutRight, this->gRight) == vRight) {
					edge_to_new2 = eOutRight;
					edge_to_new_exists2 = true;
					break;
				}
			}

			// Make sure edges from existing to new vertices are equivalent
			if((edge_to_new_exists1 != edge_to_new_exists2) ||
					((edge_to_new_exists1 && edge_to_new_exists2) &&
					!edgePred(edge_to_new1, edge_to_new2))) {
				//				std::cout << "\tfailed: edgePred, in-edge(" << vOtherLeft << ", " << vOtherRight << ")\n";
				return false;
			}

			// check out-edges from this
			//------------------------------------------------------------------------
			// but if undirected we don't need to check those we already have checked

			bool is_undirected1 = is_undirected(this->gLeft),
					is_undirected2 = is_undirected(this->gRight);

			if(is_undirected1 && is_undirected2) {
				continue;
			}

			if(!is_undirected1) {
				// Search for edge from new to existing vertex (gLeft)
				for(auto eOutLeft : asRange(out_edges(vLeft, this->gLeft))) {
					if(target(eOutLeft, this->gLeft) == vOtherLeft) {
						edge_from_new1 = eOutLeft;
						edge_from_new_exists1 = true;
						break;
					}
				}
			}

			if(!is_undirected2) {
				// Search for edge from new to existing vertex (gRight)
				for(auto eOutRight : asRange(out_edges(vRight, this->gRight))) {
					if(target(eOutRight, this->gRight) == vOtherRight) {
						edge_from_new2 = eOutRight;
						edge_from_new_exists2 = true;
						break;
					}
				}
			}

			// Make sure edges from new to existing vertices are equivalent
			//			if(edge_from_new_exists1 != edge_from_new_exists2) return false; // here is the induced part
			if(edge_from_new_exists1 && edge_from_new_exists2) {
				if(!this->edgePred(edge_from_new1, edge_from_new2)) {
					//					std::cout << "\tfailed: edgePred, out-edge(" << vOtherLeft << ", " << vOtherRight << ")\n";
					return false;
				}
			}
		} // foreach left vertex

		bool res = static_cast<Derived&> (*this).visit_tryPush(vLeft, vRight);
		if(!res) {
			//			std::cout << "\tfailed: user\n";
			return false;
		}
		//		std::cout << "\tsuccess\n";
		// now actually do the push
		pushImpl(vLeft, vRight);
		return true;
	}

	std::pair<VertexLeft, VertexRight> pop() {
		assert(stack.size() > checkedEnd);
		return popNoAsserts();
	}
protected: // args
	const GraphLeft &gLeft;
	const GraphRight &gRight;
	EdgePred edgePred;
	VertexPred vertexPred;
private: // alg state
	// we do very specific puts so we manage the two directions manually
	VectorVertexMap<GraphLeft, GraphRight> m;
	VectorVertexMap<GraphRight, GraphLeft> mInverse;
	std::vector<std::pair<VertexLeft, VertexRight> > stack;
	std::size_t uncheckedEnd = 0;
	std::size_t checkedEnd = 0;
};

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_INJECTIVEENUMERATION_HPP */