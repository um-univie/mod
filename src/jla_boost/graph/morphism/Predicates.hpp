#ifndef JLA_BOOST_GRAPH_MORPHISM_PREDICATES_H
#define JLA_BOOST_GRAPH_MORPHISM_PREDICATES_H

// - PropertyPredicate

namespace jla_boost {
namespace GraphMorphism {

// PropertyPredicate
//------------------------------------------------------------------------------

template<typename Pred, typename PropLeft, typename PropRight, typename Next>
struct PropertyPredicate {

	PropertyPredicate(Pred pred, PropLeft pLeft, PropRight pRight, Next next)
	: pred(pred), pLeft(pLeft), pRight(pRight), next(next) { }

	template<typename VertexOrEdge>
	bool operator()(VertexOrEdge veLeft, VertexOrEdge veRight) {
		return pred(get(pLeft, veLeft), get(pRight, veRight)) && next(veLeft, veRight);
	}
private:
	Pred pred;
	PropLeft pLeft;
	PropRight pRight;
	Next next;
};

template<typename Pred, typename PropLeft, typename PropRight, typename Next = AlwaysTrue>
auto makePropertyPredicate(Pred pred, PropLeft &&pLeft, PropRight &&pRight, Next next = AlwaysTrue()) {
	return PropertyPredicate<Pred, PropLeft, PropRight, Next>(pred, std::forward<PropLeft>(pLeft), std::forward<PropRight>(pRight), next);
}

template<typename PropLeft, typename PropRight, typename Next = AlwaysTrue>
auto makePropertyPredicateEq(PropLeft &&pLeft, PropRight &&pRight, Next next = AlwaysTrue()) {
	return makePropertyPredicate(std::equal_to<>(), std::forward<PropLeft>(pLeft), std::forward<PropRight>(pRight), next);
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_PREDICATES_H */