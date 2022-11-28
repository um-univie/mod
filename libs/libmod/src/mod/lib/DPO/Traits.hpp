#ifndef MOD_LIB_DPO_TRAITS_HPP
#define MOD_LIB_DPO_TRAITS_HPP

namespace mod::lib::DPO {

template<typename Rule>
struct RuleTraits {
	using SideGraphType = typename Rule::SideGraphType;
	using KGraphType = typename Rule::KGraphType;
	using MorphismType = typename Rule::MorphismType;
};

} // namespace mod::lib::DPO

#endif // MOD_LIB_DPO_TRAITS_HPP