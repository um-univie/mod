#ifndef MOD_LIB_GRAPHMORPHISM_VISITOR_HPP
#define MOD_LIB_GRAPHMORPHISM_VISITOR_HPP

namespace mod::lib::GraphMorphism::Constraints {

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

} // namespace mod::lib::GraphMorphism::Constraints

#endif // MOD_LIB_GRAPHMORPHISM_VISITOR_HPP