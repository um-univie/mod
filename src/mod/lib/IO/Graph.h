#ifndef MOD_LIB_IO_GRAPH_H
#define	MOD_LIB_IO_GRAPH_H

#include <mod/lib/Graph/Base.h>

#include <istream>
#include <memory>
#include <ostream>
#include <string>

namespace mod {
namespace lib {
namespace Graph {
class Single;
} // namespace Graph
namespace IO {
namespace Graph {
namespace Read {
std::pair<std::unique_ptr<lib::Graph::GraphType>, std::unique_ptr<lib::Graph::PropStringType> > gml(std::istream &s, std::ostream &err);
std::pair<std::unique_ptr<lib::Graph::GraphType>, std::unique_ptr<lib::Graph::PropStringType> > dfs(const std::string &dfs, std::ostream &err);
std::pair<std::unique_ptr<lib::Graph::GraphType>, std::unique_ptr<lib::Graph::PropStringType> > smiles(const std::string &smiles, std::ostream &err);
} // namespace Read
namespace Write {

struct Options {

	Options() : edgesAsBonds(false), collapseHydrogens(false), raiseCharges(false),
	simpleCarbons(false), thick(false), withColour(false), withIndex(false), withTexttt(false) { }

	Options &Non() {
		return EdgesAsBonds(false).CollapseHydrogens(false).RaiseCharges(false).SimpleCarbons(false).Thick(false).WithColour(false).WithIndex(false);
	}

	Options &All() {
		return EdgesAsBonds(true).CollapseHydrogens(true).RaiseCharges(true).SimpleCarbons(true).Thick(true).WithColour(true).WithIndex(true);
	}

	Options &EdgesAsBonds(bool v) {
		edgesAsBonds = v;
		return *this;
	}

	Options &CollapseHydrogens(bool v) {
		collapseHydrogens = v;
		return *this;
	}

	Options &RaiseCharges(bool v) {
		raiseCharges = v;
		return *this;
	}

	Options &SimpleCarbons(bool v) {
		simpleCarbons = v;
		return *this;
	}

	Options &Thick(bool v) {
		thick = v;
		return *this;
	}

	Options &WithColour(bool v) {
		withColour = v;
		return *this;
	}

	Options &WithIndex(bool v) {
		withIndex = v;
		return *this;
	}

	Options &WithTexttt(bool v) {
		withTexttt = v;
		return *this;
	}

	operator std::string() const {
		auto toChar = [](bool b) {
			return b ? '1' : '0';
		};
		std::string res;
		res += toChar(edgesAsBonds);
		res += toChar(collapseHydrogens);
		res += toChar(raiseCharges);
		res += toChar(simpleCarbons);
		res += toChar(thick);
		res += toChar(withColour);
		res += toChar(withIndex);
		res += toChar(withTexttt);
		return res;
	}
public:
	bool edgesAsBonds;
	bool collapseHydrogens;
	bool raiseCharges;
	bool simpleCarbons;
	bool thick;
	bool withColour;
	bool withIndex;
	bool withTexttt;
};

// all return the filename _with_ extension
void gml(const lib::Graph::Single &g, bool withCoords, std::ostream &s);
std::string gml(const lib::Graph::Single &g, bool withCoords);
std::string dot(const lib::Graph::Single &g);
std::string coords(const lib::Graph::Single &g, bool collapseHydrogens);
std::pair<std::string, std::string> tikz(const lib::Graph::Single &g, const Options &options);
std::string pdf(const lib::Graph::Single &g, const Options &options);
std::string svg(const lib::Graph::Single &g, const Options &options);
std::pair<std::string, std::string> summary(const lib::Graph::Single &g, const Options &first, const Options &second);
} // namespace Write
} // namespace Graph
} // namespace IO
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_IO_GRAPH_H */

