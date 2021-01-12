#ifndef MOD_LIB_STEREO_EDGECATEGORY_HPP
#define MOD_LIB_STEREO_EDGECATEGORY_HPP

#include <mod/Chem.hpp>

#include <array>
#include <bitset>
#include <iosfwd>
#include <numeric>
#include <type_traits>

namespace mod::lib::Stereo {

enum class EdgeCategory : unsigned char {
	// A partial order: Any is greater than all the others.
	// All the others are incomparable.
	// Shorthand for names: X, U, S, D, T, A
	// Shorthand for users: *, u, -, =, #, :
	Any,
	Undefined,
	Single,
	Double, Triple,
	Aromatic
};
constexpr std::underlying_type<EdgeCategory>::type EdgeCategorySize =
		static_cast<std::underlying_type<EdgeCategory>::type> (EdgeCategory::Aromatic) + 1;

bool carriesPlane(EdgeCategory cat);
std::ostream &operator<<(std::ostream &s, EdgeCategory c);
Stereo::EdgeCategory bondTypeToEdgeCategory(BondType bt);

// A simple bit-vector to indicate a subset of valid categories.

struct EdgeCategorySubset {
	EdgeCategorySubset all() &&;
	EdgeCategorySubset operator+(EdgeCategory cat) &&;
	EdgeCategorySubset operator-(EdgeCategory cat) &&;
	friend EdgeCategorySubset operator&(EdgeCategorySubset a, EdgeCategorySubset b);
	friend bool operator==(EdgeCategorySubset a, EdgeCategorySubset b);
	bool operator()(EdgeCategory cat) const;
	unsigned char count() const;
	EdgeCategory selectFirst() const;
	friend std::ostream &operator<<(std::ostream &s, const EdgeCategorySubset &ec);
private:
	std::bitset<EdgeCategorySize> bits;
};

struct EdgeCategoryCount {
	using Storage = std::array<unsigned char, EdgeCategorySize>;
public:
	EdgeCategoryCount();
	unsigned char sum() const;
	unsigned char &operator[](EdgeCategory cat);
	const unsigned char &operator[](EdgeCategory cat) const;
	friend std::ostream &operator<<(std::ostream &s, const EdgeCategoryCount &c);
	Storage::const_iterator begin() const;
	Storage::const_iterator end() const;
	friend bool operator==(const EdgeCategoryCount &lhs, const EdgeCategoryCount &rhs);
	friend bool operator!=(const EdgeCategoryCount &lhs, const EdgeCategoryCount &rhs);
	friend bool componentWiseLEQ(const EdgeCategoryCount &lhs, const EdgeCategoryCount &rhs);
private:
	Storage count;
};

} // namespace mod::lib::Stereo

#endif // MOD_LIB_STEREO_EDGECATEGORY_HPP