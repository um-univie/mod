#ifndef MOD_LIB_GRAPH_MERGE_H
#define MOD_LIB_GRAPH_MERGE_H

#include <mod/lib/Graph/Base.h>
#include <mod/lib/Graph/Properties/Molecule.h>

namespace mod {
namespace lib {
namespace Graph {
struct Single;

class Merge : public Base {
	Merge(const Merge&) = delete;
	Merge &operator=(const Merge&) = delete;
	Merge(Merge&&) = delete;
	Merge &operator=(Merge&&) = delete;
public:
	Merge();
	~Merge();
	Merge *clone() const;
	const GraphType &getGraph() const;
	const PropStringType &getStringState() const;
	const PropMolecule &getMoleculeState() const;
	void printName(std::ostream &s) const;
	void mergeWith(const Single &g);
	void lock();
	bool isLocked() const;
	void clearCache() const;
private:
	void calcCache() const;
	void doMerge(const Graph::Single &src) const;
private:
	bool locked;
	mutable std::unique_ptr<GraphType> graphBoost;
	mutable std::unique_ptr<PropStringType> pString;
	mutable std::unique_ptr<PropMolecule> moleculeState;
public:
	static bool equal(const Merge &g1, const Merge &g2);
	friend bool equal(const Merge *g1, const Merge *g2);
};

struct MergeLess {
	bool operator()(const Merge *g1, const Merge *g2) const;
};

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_MERGE_H */

