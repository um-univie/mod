#ifndef MOD_LIB_GRAPHMORPHISM_ALLVISITOR_H
#define MOD_LIB_GRAPHMORPHISM_ALLVISITOR_H

#include <mod/lib/GraphMorphism/Constraints/ShortestPath.h>
#include <mod/lib/GraphMorphism/Constraints/VertexAdjacency.h>
#include <mod/lib/GraphMorphism/Constraints/Visitor.h>

namespace mod {
namespace lib {
namespace GraphMorphism {
namespace Constraints {
namespace detail {

template<typename Graph, template<typename> class ...Cs>
struct AllVisitor : BaseVisitor<Graph>, Visitor<const Cs<Graph> >... {
};

template<typename Graph, template<typename> class ...Cs>
struct AllVisitorNonConst : BaseVisitorNonConst<Graph>, Visitor<Cs<Graph> >... {
};

} // namespace detail

template<typename Graph>
struct AllVisitor : detail::AllVisitor<Graph,
/*   */ ShortestPath,
/*   */ VertexAdjacency
> {
};

template<typename Graph>
struct AllVisitorNonConst : detail::AllVisitorNonConst<Graph,
/*   */ ShortestPath,
/*   */ VertexAdjacency
> {
};

} // namespace Constraints
} // namespace GraphMorphism
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPHMORPHISM_ALLVISITOR_H */