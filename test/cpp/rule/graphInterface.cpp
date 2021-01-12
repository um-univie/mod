
#include <mod/GraphConcepts.hpp>
#include <mod/rule/GraphInterface.hpp>

#include <boost/concept_check.hpp>

int main() {
	BOOST_CONCEPT_ASSERT((mod::concepts::Graph<mod::rule::Rule, true>));
	BOOST_CONCEPT_ASSERT((mod::concepts::LabelledGraph<mod::rule::Rule::LeftGraph, false>));
	BOOST_CONCEPT_ASSERT((mod::concepts::Graph<mod::rule::Rule::ContextGraph, false>));
	BOOST_CONCEPT_ASSERT((mod::concepts::LabelledGraph<mod::rule::Rule::RightGraph, false>));
}