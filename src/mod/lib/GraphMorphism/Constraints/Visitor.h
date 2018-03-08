#ifndef MOD_LIB_GRAPHMORPHISM_VISITOR_H
#define MOD_LIB_GRAPHMORPHISM_VISITOR_H

namespace mod {
namespace lib {
namespace GraphMorphism {
namespace Constraints {

template<typename Graph>
struct BaseVisitor {
	virtual ~BaseVisitor() = default;
};

template<typename Graph>
struct BaseVisitorNonConst {
	virtual ~BaseVisitorNonConst() = default;
};

template<typename C>
struct Visitor {
	virtual void operator()(C &c) = 0;
};

} // namespace Constraints
} // namespace GraphMorphism
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPHMORPHISM_VISITOR_H */

