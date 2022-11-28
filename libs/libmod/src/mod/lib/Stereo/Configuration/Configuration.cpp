#include "Configuration.hpp"

#include <mod/Error.hpp>

namespace mod::lib::Stereo {

// Fixation
//------------------------------------------------------------------------------

Fixation::Fixation(bool f) : simple(f) { }

bool Fixation::asSimple() const {
	return simple;
}

bool operator==(const Fixation &lhs, const Fixation &rhs) {
	return lhs.simple == rhs.simple;
}

bool operator!=(const Fixation &lhs, const Fixation &rhs) {
	return !(lhs == rhs);
}

Fixation Fixation::free() {
	return Fixation(false);
}

Fixation Fixation::simpleFixed() {
	return Fixation(true);
}

std::ostream &operator<<(std::ostream &s, const Fixation &f) {
	if(f.simple) s << "Simple";
	else s << "Free";
	return s;
}

// Configuration
//------------------------------------------------------------------------------

Configuration::Configuration(GeometryGraph::Vertex vGeometry, const EmbeddingEdge *first, const EmbeddingEdge *last)
: vGeometry(vGeometry), numLonePairs(0), hasRadical(false) {
	for(; first != last; ++first) {
		switch(first->type) {
		case EmbeddingEdge::Type::Edge: break;
		case EmbeddingEdge::Type::LonePair: 
			++numLonePairs;
			break;
		case EmbeddingEdge::Type::Radical:
			assert(!hasRadical);
			hasRadical = true;
			break;
		}
	}
}

Configuration::~Configuration() { }

GeometryGraph::Vertex Configuration::getGeometryVertex() const {
	return vGeometry;
}

Fixation Configuration::getFixation() const {
	return Fixation::free();
}

std::size_t Configuration::degree() const {
	return end() - begin();
}

std::string Configuration::asRawString(std::function<std::size_t(const EmbeddingEdge&)> getNeighbourId) const {
	std::string res;
	res += getGeometryGraph().getGraph()[getGeometryVertex()].name;
	res += '[';
	bool first = true;
	for(const auto &emb : * this) {
		if(!first) res += ", ";
		first = false;
		switch(emb.type) {
		case EmbeddingEdge::Type::LonePair:
			res += "e";
			break;
		case EmbeddingEdge::Type::Radical:
			MOD_ABORT;
		case EmbeddingEdge::Type::Edge:
			res += std::to_string(getNeighbourId(emb));
			break;
		}
	}
	res += ']';
	res += asRawStringImpl(getNeighbourId);
	return res;
}

std::string Configuration::asPrettyString(std::function<std::size_t(const EmbeddingEdge&)> getNeighbourId) const {
	std::string res;
	bool printEdges;
	std::tie(res, printEdges) = asPrettyStringImpl(getNeighbourId);
	if(printEdges) {
		res += "$[";
		bool first = true;
		for(const auto &emb : * this) {
			if(!first) res += ", ";
			first = false;
			switch(emb.type) {
			case EmbeddingEdge::Type::LonePair:
				res += "e";
				break;
			case EmbeddingEdge::Type::Radical:
				MOD_ABORT;
				break;
			case EmbeddingEdge::Type::Edge:
				res += std::to_string(getNeighbourId(emb));
				break;
			}
		}
		res += "]$";
	}
	return res;
}

std::string Configuration::asRawStringImpl(std::function<std::size_t(const EmbeddingEdge&)> getNeighbourId) const {
	return "";
}

// DynamicDegree
//------------------------------------------------------------------------------

DynamicDegree::DynamicDegree(GeometryGraph::Vertex vGeometry, const EmbeddingEdge *b, const EmbeddingEdge *e)
: Configuration(vGeometry, b, e) {
	std::copy(b, e, std::back_inserter(edges));
}

} // namespace mod::lib::Stereo