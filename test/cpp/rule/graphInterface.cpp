
#include <mod/GraphConcepts.hpp>
#include <mod/rule/GraphInterface.hpp>

#include <boost/concept_check.hpp>

int main() {
	BOOST_CONCEPT_ASSERT((mod::concepts::Graph<mod::rule::Rule>));
	BOOST_CONCEPT_ASSERT((mod::concepts::LabelledGraph<mod::rule::Rule::LeftGraph>));
	BOOST_CONCEPT_ASSERT((mod::concepts::Graph<mod::rule::Rule::ContextGraph>));
	BOOST_CONCEPT_ASSERT((mod::concepts::LabelledGraph<mod::rule::Rule::RightGraph>));
}