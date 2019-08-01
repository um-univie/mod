#ifndef MOD_GRAPH_AUTOMORPHISM_H
#define MOD_GRAPH_AUTOMORPHISM_H

#include <mod/BuildConfig.hpp>
#include <mod/graph/Graph.hpp>

#include <boost/iterator/iterator_facade.hpp>

namespace mod {
namespace graph {

// rst-class: graph::Graph::Aut
// rst:
// rst:		A class representing an automorphism of a :class:`Graph`.
// rst:
// rst-class-start:

struct Graph::Aut {
	Aut(std::shared_ptr<Graph> g, LabelType lt, bool withStereo, std::size_t i);
	// rst: .. cpp:function:: Vertex operator[](Vertex v) const
	// rst:
	// rst:		:returns: the image of the given vertex under the permutation.
	Vertex operator[](Vertex v) const;
	MOD_DECL friend std::ostream &operator<<(std::ostream &s, const Aut &a);
private:
	std::shared_ptr<Graph> g;
	LabelType lt;
	bool withStereo;
	std::size_t i;
};
// rst-class-end:

// rst-class: graph::Graph::AutGroup
// rst:
// rst:		A class representing the automorphism group of a :class:`Graph`.
// rst:
// rst-class-start:

struct Graph::AutGroup {
	// rst-nested: graph::Graph::AutGroup::Gens
	// rst:
	// rst:		A random-access range of the generators for the automorphism group.
	// rst:
	// rst-nested-start:

	struct Gens {

		class iterator : public boost::iterator_facade<iterator, Aut, std::random_access_iterator_tag, Aut> {
			friend class Gens;
			iterator() = default;
			iterator(std::shared_ptr<Graph> g, LabelType lt, bool withStereo, std::size_t i);
		private:
			friend class boost::iterator_core_access;
			bool equal(const iterator &other) const;
			void increment();
			Aut dereference() const;
			std::size_t distance_to(const iterator &other) const;
			void advance(std::size_t n);
		private:
			std::shared_ptr<Graph> g;
			LabelType lt = LabelType::String;
			bool withStereo = false;
			std::size_t i = 0;
		};
		using const_iterator = iterator;
	private:
		friend class AutGroup;
		Gens(std::shared_ptr<Graph> g, LabelType lt, bool withStereo);
	public:
		iterator begin() const;
		iterator end() const;
		Aut operator[](std::size_t i) const;
		std::size_t size() const;
	private:
		std::shared_ptr<Graph> g;
		LabelType lt;
		bool withStereo;
	};

	// rst-nested-end:
public:
	// rst: .. function:: AutGroup(std::shared_ptr<const Graph> g, LabelSettings labelSettings)
	// rst:
	AutGroup(std::shared_ptr<Graph> g, LabelSettings labelSettings);
	// rst: .. function:: Gens gens() const
	// rst:
	// rst:		:returns: a range of the generators for the group.
	Gens gens() const;
	MOD_DECL friend std::ostream &operator<<(std::ostream &s, const AutGroup &a);
private:
	std::shared_ptr<Graph> g;
	LabelType lt;
	bool withStereo;
};
// rst-class-end:

} // namespace graph
} // namespace mod

#endif /* MOD_GRAPH_AUTOMORPHISM_H */