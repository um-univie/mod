
#include <mod/GraphConcepts.hpp>
#include <mod/graph/GraphInterface.hpp>

#include <boost/concept_check.hpp>

int main() {
	BOOST_CONCEPT_ASSERT((mod::concepts::LabelledGraph<mod::graph::Graph>));
}