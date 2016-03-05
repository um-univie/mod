#ifndef JLA_BOOST_TAGGEDLIST_HPP
#define	JLA_BOOST_TAGGEDLIST_HPP

namespace jla_boost {

struct TaggedListEmpty {
};

template<typename Tag, typename T, typename Tail = TaggedListEmpty>
struct TaggedList {
	TaggedList() = default;

	template<typename U>
	TaggedList(U &&u) : value(u) { }

	template<typename U>
	TaggedList(U &&u, const Tail &tail) : value(std::move(u)), tail(tail) { }
public:
	T value;
	Tail tail;
};

//------------------------------------------------------------------------------
// get
//------------------------------------------------------------------------------

namespace detail {

template<typename Data, typename UTag>
struct TaggedListLookup;

template<>
template<typename Tag, typename T, typename Tail>
struct TaggedListLookup<TaggedList<Tag, T, Tail>, Tag> {

	static T &get(TaggedList<Tag, T, Tail> &data) {
		return data.value;
	}

	static const T &get(const TaggedList<Tag, T, Tail> &data) {
		return data.value;
	}
};

template<>
template<typename Tag, typename T, typename Tail, typename UTag>
struct TaggedListLookup<TaggedList<Tag, T, Tail>, UTag> {
	static auto get(TaggedList<Tag, T, Tail> &data) -> decltype(TaggedListLookup<Tail, UTag>::get(data.tail)) {
		return TaggedListLookup<Tail, UTag>::get(data.tail);
	}

	static auto get(const TaggedList<Tag, T, Tail> &data) -> decltype(TaggedListLookup<Tail, UTag>::get(data.tail)) {
		return TaggedListLookup<Tail, UTag>::get(data.tail);
	}
};

} // namespace detail

template<typename Data, typename UTag>
auto get(UTag, Data &data) -> decltype(detail::TaggedListLookup<Data, UTag>::get(data)) {
	return detail::TaggedListLookup<Data, UTag>::get(data);
}

template<typename Data, typename UTag>
auto get(UTag, const Data &data) -> decltype(detail::TaggedListLookup<Data, UTag>::get(data)) {
	return detail::TaggedListLookup<Data, UTag>::get(data);
}

//------------------------------------------------------------------------------
// Concat
//------------------------------------------------------------------------------

template<typename Prefix, typename Suffix>
struct TaggedListConcat;

template<>
template<typename Suffix>
struct TaggedListConcat<TaggedListEmpty, Suffix> {
	using type = Suffix;
};

template<>
template<typename Tag, typename T, typename Tail, typename Suffix>
struct TaggedListConcat<TaggedList<Tag, T, Tail>, Suffix> {
	using type = TaggedList<Tag, T, typename TaggedListConcat<Tail, Suffix>::type>;
};

} // namespace jla_boost

#endif	/* JLA_BOOST_TAGGEDLIST_HPP */