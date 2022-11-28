#include "TrigonalPlanar.hpp"

#include <mod/Error.hpp>

namespace mod::lib::Stereo {

TrigonalPlanar::TrigonalPlanar(const GeometryGraph &g, const std::array<EmbeddingEdge, 3> &edges, bool fixed)
: StaticDegree<3>(g.trigonalPlanar, edges), fixed(fixed) { }

std::unique_ptr<Configuration> TrigonalPlanar::cloneFree(const GeometryGraph &g) const {
	return std::make_unique<TrigonalPlanar>(g, edges, fixed);
}

std::unique_ptr<Configuration> TrigonalPlanar::clone(const GeometryGraph &g, const std::vector<std::size_t> &offsetMap) const {
	auto c = std::make_unique<TrigonalPlanar>(g, edges, fixed);
	for(auto &emb : c->edges) {
		if(emb.type == EmbeddingEdge::Type::Edge) {
			emb.offset = offsetMap[emb.offset];
		}
	}
	return c;
}

Fixation TrigonalPlanar::getFixation() const {
	return Fixation(fixed);
}

bool TrigonalPlanar::localPredIso(const Configuration &other) const {
	assert(dynamic_cast<const TrigonalPlanar*> (&other));
	const TrigonalPlanar &o = static_cast<const TrigonalPlanar&> (other);
	return this->fixed == o.fixed;
}

bool TrigonalPlanar::localPredSpec(const Configuration &other) const {
	assert(dynamic_cast<const TrigonalPlanar*> (&other));
	const TrigonalPlanar &o = static_cast<const TrigonalPlanar&> (other);
	// if we are free, it's fine, otherwise the other must be fixed
	return !this->fixed || o.fixed;
}

bool TrigonalPlanar::morphismStaticOk() const {
	return false;
}

bool TrigonalPlanar::morphismDynamicOk() const {
	return !this->fixed;
}

bool TrigonalPlanar::morphismIso(const Configuration &cCodom, std::vector<std::size_t> &perm) const {
	MOD_ABORT;
}

bool TrigonalPlanar::morphismSpec(const Configuration &cCodom, std::vector<std::size_t> &perm) const {
	MOD_ABORT;
}

std::string TrigonalPlanar::asRawStringImpl(std::function<std::size_t(const EmbeddingEdge&)> getNeighbourId) const {
	if(fixed) return "!";
	else return "";
}

std::pair<std::string, bool> TrigonalPlanar::asPrettyStringImpl(std::function<std::size_t(const EmbeddingEdge&)> getNeighbourId) const {
	std::string res = "\\modStereoShape{TrigonalPlanar";
	if(!fixed) return std::make_pair(res + "Sym}", false);
	res += "Fixed}";
	return std::make_pair(res, true);
}

} // namespace mod::lib::Stereo