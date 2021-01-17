#include <mod/Chem.hpp>

#include <boost/concept_check.hpp>

#include <type_traits>

using namespace mod;

template<typename T>
void check() {
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<T>));
	BOOST_CONCEPT_ASSERT((boost::CopyConstructible<T>));
	BOOST_CONCEPT_ASSERT((boost::Assignable<T>));
	BOOST_CONCEPT_ASSERT((boost::EqualityComparable<T>));
	BOOST_CONCEPT_ASSERT((boost::LessThanComparable<T>));
}

int main() {
	check<AtomId>();
	check<Isotope>();
	check<Charge>();
	check<AtomData>();
	check<BondType>();
}