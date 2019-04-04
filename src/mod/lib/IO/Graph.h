#ifndef MOD_LIB_IO_GRAPH_H
#define MOD_LIB_IO_GRAPH_H

#include <mod/lib/Graph/GraphDecl.h>

#include <istream>
#include <memory>
#include <ostream>
#include <string>

namespace mod {
namespace lib {
namespace Graph {
struct DepictionData;
struct LabelledGraph;
struct PropStereo;
struct PropString;
struct Single;
} // namespace Graph
namespace IO {
namespace Graph {
namespace Read {

struct Data {
	Data();
	Data(std::unique_ptr<lib::Graph::GraphType> graph, std::unique_ptr<lib::Graph::PropString> label);
	Data(Data &&other);
	~Data();
public:
	std::unique_ptr<lib::Graph::GraphType> g;
	std::unique_ptr<lib::Graph::PropString> pString;
	std::unique_ptr<lib::Graph::PropStereo> pStereo;
	std::map<int, std::size_t> externalToInternalIds;
};

Data gml(std::istream &s, std::ostream &err);
Data dfs(const std::string &dfs, std::ostream &err);
Data smiles(const std::string &smiles, std::ostream &err);
} // namespace Read
namespace Write {

enum class EdgeFake3DType {
	None, WedgeSL, WedgeLS, HashSL, HashLS
};

EdgeFake3DType invertEdgeFake3DType(EdgeFake3DType t);

struct Options {
	Options() = default;

	Options &Non() {
		return EdgesAsBonds(false).CollapseHydrogens(false).RaiseIsotopes(false).RaiseCharges(false).SimpleCarbons(false).Thick(false).WithColour(false).WithIndex(false);
	}

	Options &All() {
		return EdgesAsBonds(true).CollapseHydrogens(true).RaiseIsotopes(true).RaiseCharges(true).SimpleCarbons(true).Thick(true).WithColour(true).WithIndex(true);
	}

	Options &EdgesAsBonds(bool v) {
		edgesAsBonds = v;
		return *this;
	}

	Options &CollapseHydrogens(bool v) {
		collapseHydrogens = v;
		return *this;
	}

	Options &RaiseIsotopes(bool v) {
		raiseIsotopes = v;
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

	Options &WithRawStereo(bool v) {
		withRawStereo = v;
		return *this;
	}

	Options &WithPrettyStereo(bool v) {
		withPrettyStereo = v;
		return *this;
	}

	Options &Rotation(int degrees) {
		rotation = degrees;
		return *this;
	}

	Options &Mirror(bool v) {
		mirror = v;
		return *this;
	}

	std::string getStringEncoding() const {
		auto toChar = [](bool b) {
			return b ? '1' : '0';
		};
		std::string res;
		res += toChar(edgesAsBonds);
		res += toChar(collapseHydrogens);
		char raise = 0;
		if(raiseCharges) raise += 1;
		if(raiseIsotopes) raise += 2;
		res += '0' + raise;
		res += toChar(simpleCarbons);
		res += toChar(thick);
		res += toChar(withColour);
		res += toChar(withIndex);
		res += toChar(withTexttt);
		char stereo = 0;
		if(withRawStereo) stereo += 1;
		if(withPrettyStereo) stereo += 2;
		if(stereo != 0) res += '0' + stereo;
		if(rotation != 0 || mirror) {
			res += "_";
			res += std::to_string(rotation);
			if(mirror) {
				res += "_";
				res += toChar(mirror);
			}
		}
		return res;
	}

	friend bool operator==(const Options &a, const Options &b) {
		return a.getStringEncoding() == b.getStringEncoding();
	}
public:
	bool edgesAsBonds = false;
	bool collapseHydrogens = false;
	bool raiseCharges = false;
	bool raiseIsotopes = false;
	bool simpleCarbons = false;
	bool thick = false;
	bool withColour = false;
	bool withIndex = false;
	bool withTexttt = false;
	bool withRawStereo = false;
	bool withPrettyStereo = false;
	int rotation = 0;
	bool mirror = false;
};

// all return the filename _with_ extension
void gml(const lib::Graph::LabelledGraph &gLabelled, const lib::Graph::DepictionData &depict, const std::size_t gId, bool withCoords, std::ostream &s);
std::string gml(const lib::Graph::Single &g, bool withCoords);
std::string dot(const lib::Graph::LabelledGraph &gLabelled, const std::size_t gId);
std::string coords(const lib::Graph::LabelledGraph &gLabelled, const lib::Graph::DepictionData &depict, const std::size_t gId, const Options &options, bool asInline);
std::pair<std::string, std::string>
tikz(const lib::Graph::LabelledGraph &gLabelled, const lib::Graph::DepictionData &depict, const std::size_t gId, const Options &options,
		bool asInline, const std::string &idPrefix);
std::string pdf(const lib::Graph::LabelledGraph &gLabelled, const lib::Graph::DepictionData &depict, const std::size_t gId, const Options &options);
std::string svg(const lib::Graph::LabelledGraph &gLabelled, const lib::Graph::DepictionData &depict, const std::size_t gId, const Options &options);
std::pair<std::string, std::string> summary(const lib::Graph::Single &g, const Options &first, const Options &second);
void termState(const lib::Graph::Single &g);

// simplified interface for lib::Graph::Single
void gml(const lib::Graph::Single &g, bool withCoords, std::ostream &s);
std::string tikz(const lib::Graph::Single &g, const Options &options, bool asInline, const std::string &idPrefix);
std::string pdf(const lib::Graph::Single &g, const Options &options);
std::string svg(const lib::Graph::Single &g, const Options &options);

} // namespace Write
} // namespace Graph
} // namespace IO
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_GRAPH_H */

