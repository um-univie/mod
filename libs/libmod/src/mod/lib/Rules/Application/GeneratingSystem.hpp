#ifndef MOD_LIB_STATESPACE_GENERATING_SYSTEM_HPP
#define MOD_LIB_STATESPACE_GENERATING_SYSTEM_HPP

#include <perm_group/transversal/transversal.hpp>
#include <perm_group/group/generated.hpp>
#include <perm_group/group/stabilizer_chain.hpp>

namespace perm_group {

template<typename Transversal, typename BasePointProvider = base_point_first_moved>
struct GeneratingSystem {
	BOOST_CONCEPT_ASSERT((TransversalConcept<Transversal>));
public: // Group
	using allocator = typename Transversal::allocator;
	using perm = typename allocator::perm;
	using pointer = typename allocator::pointer;
	using const_pointer = typename allocator::const_pointer;
	using Chain = stabilizer_chain<Transversal, BasePointProvider>;
private:
private:

	struct DupChecker {

		DupChecker(const GeneratingSystem *owner) : owner(owner) { }

		template<typename Iter>
		bool operator()(Iter first, Iter last, const perm &p) const {
			return owner->is_member(p);
		}
	public:
		const GeneratingSystem *owner;
	};
public:

	explicit GeneratingSystem(const allocator &alloc) : gen(alloc, DupChecker(this)), bpp(BasePointProvider()) { }

	explicit GeneratingSystem(const allocator &alloc, BasePointProvider bpp) : gen(alloc, DupChecker(this)), bpp(bpp) { }

	explicit GeneratingSystem(std::size_t n) : GeneratingSystem(allocator(n)) { }

	GeneratingSystem(GeneratingSystem &&other) = delete;
	GeneratingSystem &operator=(GeneratingSystem &&other) = delete;

	template<typename UPerm>
	void add_generator(UPerm &&perm) {
		gen.add_generator(std::forward<UPerm>(perm), [this](auto first, auto lastOld, auto lastNew) {
			if(!chain) {
				auto toFix = bpp(**lastOld);
				chain.reset(new Chain(toFix, get_allocator(), bpp));
			}
			chain->add_generators(first, lastOld, lastNew);
		});
	}

	bool is_member(const perm &p) const {
		if(!chain) {
			assert(generators().size() == 1);
			return p == *generators().begin();
		} else {
			return chain->is_member_of_parent(p);
		}
	}
public: // GroupConcept

	decltype(auto) degree() const {
		return gen.degree();
	}

	decltype(auto) generators() const {
		return gen.generators();
	}

	decltype(auto) generator_ptrs() const {
		return gen.generator_ptrs();
	}

	decltype(auto) get_allocator() const {
		return gen.get_allocator();
	}

	const Chain* getChain() {
		return &(*chain);
	}

private:
	generated_group<allocator, DupChecker> gen;
	std::unique_ptr<Chain> chain;
	BasePointProvider bpp;
};

} // namespace perm_group

#endif /* PERM_GROUP_GROUP_GENERATING_SYSTEM_HPP */
