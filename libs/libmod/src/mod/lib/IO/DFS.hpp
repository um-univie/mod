#ifndef MOD_LIB_IO_DFS_HPP
#define MOD_LIB_IO_DFS_HPP

#include <mod/lib/IO/Result.hpp>

#include <boost/variant/variant.hpp>

#include <iosfwd>
#include <map>
#include <memory>
#include <optional>
#include <string>

namespace mod::lib::IO::DFS {

void escapeLabel(std::ostream &s, const std::string &label, char escChar);

struct LabelVertex;
struct RingClosure;
struct Branch;

// x3 does not yet fully support std::variant
// https://github.com/boostorg/spirit/issues/321
using BaseVertex = boost::variant<LabelVertex, RingClosure>;

struct LabelVertex {
	std::string label;
	bool implicit = false;
	std::optional<int> id;
public: // set by Phase1
	int connectedComponentID;
	LabelVertex *ringClosure = nullptr; // if id turns out to be a ring-closure, then this gets set
public: // used by consumer
	int gVertexId = -1;
public:
	friend std::ostream &operator<<(std::ostream &s, const LabelVertex &lv);
};

struct RingClosure {
	// must use unsigned so the parser understands how to store it
	unsigned int id = std::numeric_limits<unsigned int>::max();
public:
	LabelVertex *other;
public:
	friend std::ostream &operator<<(std::ostream &s, const RingClosure &rc);
};

struct Vertex {
	BaseVertex vertex;
	std::vector<Branch> branches;
public:
	friend std::ostream &operator<<(std::ostream &s, const Vertex &v);
};

struct Edge {
	std::string label;
public:
	friend std::ostream &operator<<(std::ostream &s, const Edge &e);
};

using EVPair = std::pair<Edge, Vertex>;

struct Chain {
	Vertex head;
	std::vector<EVPair> tail;
	bool hasNonSmilesRingClosure; // only set when creating GraphDFS from a graph
public:
	friend std::ostream &operator<<(std::ostream &s, const Chain &c);
};

struct Branch {
	std::vector<EVPair> tail;
public:
	friend std::ostream &operator<<(std::ostream &s, const Branch &b);
};

struct Rule {
	std::optional<Chain> left, right;
public:
	friend std::ostream &operator<<(std::ostream &s, const Rule &r);
};

namespace Read {

struct GraphResult {
	// returns a pointer such that all resolved rings have stable pointers
	std::unique_ptr<Chain> ast;
	int numVertices;
	std::map<int, LabelVertex *> vertexFromId;
};

struct RuleResult {
	// the ast may be null if that side doesn't exist
	GraphResult left, right;
};

Result<GraphResult> graph(std::string_view data);
Result<RuleResult> rule(std::string_view data);

} // namespace Read
} // namespace mod::lib::IO::DF

#endif // MOD_LIB_IO_DFS_HPP