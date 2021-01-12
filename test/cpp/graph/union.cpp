
#include <mod/GraphConcepts.hpp>
#include <mod/graph/Union.hpp>

#include <boost/concept_check.hpp>

int main() {
	BOOST_CONCEPT_ASSERT((mod::concepts::LabelledGraph<mod::graph::Union, false>));
}