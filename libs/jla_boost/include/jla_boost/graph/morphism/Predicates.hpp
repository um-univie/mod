#ifndef JLA_BOOST_GRAPH_MORPHISM_PREDICATES_H
#define JLA_BOOST_GRAPH_MORPHISM_PREDICATES_H

// - PropertyPredicate

namespace jla_boost {
namespace GraphMorphism {

// PropertyPredicate
//------------------------------------------------------------------------------

template<typename Pred, typename PropDomain, typename PropCodomain, typename Next>
struct PropertyPredicate {

	PropertyPredicate(Pred pred, PropDomain pDomain, PropCodomain pCodomain, Next next)
	: pred(pred), pDomain(pDomain), pCodomain(pCodomain), next(next) { }

	template<typename VEDomain, typename VECodomain, typename ...Args>
	bool operator()(const VEDomain &veDomain, const VECodomain &veCodomain, Args&&... args) const {
		return pred(get(pDomain, veDomain), get(pCodomain, veCodomain))
				&& next(veDomain, veCodomain, std::forward<Args>(args)...);
	}
private:
	Pred pred;
	PropDomain pDomain;
	PropCodomain pCodomain;
	Next next;
};

template<typename Pred, typename PropDomain, typename PropCodomain, typename Next = AlwaysTrue>
auto makePropertyPredicate(Pred pred, PropDomain &&pDomain, PropCodomain &&pCodomain, Next next = AlwaysTrue()) {
	return PropertyPredicate<Pred, PropDomain, PropCodomain, Next>(pred, std::forward<PropDomain>(pDomain), std::forward<PropCodomain>(pCodomain), next);
}

template<typename PropDomain, typename PropCodomain, typename Next = AlwaysTrue>
auto makePropertyPredicateEq(PropDomain &&pDomain, PropCodomain &&pCodomain, Next next = AlwaysTrue()) {
	return makePropertyPredicate(std::equal_to<>(), std::forward<PropDomain>(pDomain), std::forward<PropCodomain>(pCodomain), next);
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_PREDICATES_H */