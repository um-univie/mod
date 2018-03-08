#ifndef MOD_LIB_STEREO_CONFIGURATION_TETRAHEDRAL_H
#define MOD_LIB_STEREO_CONFIGURATION_TETRAHEDRAL_H

#include <mod/lib/Stereo/Configuration/Configuration.h>

namespace mod {
namespace lib {
namespace Stereo {

struct Tetrahedral final : StaticDegree<4> {
	Tetrahedral(const GeometryGraph &g, const std::array<EmbeddingEdge, 4> &edges, bool fixed);
	virtual std::unique_ptr<Configuration> cloneFree(const GeometryGraph &g) const override;
	virtual std::unique_ptr<Configuration> clone(const GeometryGraph &g, const std::vector<std::size_t> &offsetMap) const override;
	virtual Fixation getFixation() const override;
public: // checking
	virtual bool localPredIso(const Configuration &other) const override;
	virtual bool localPredSpec(const Configuration &other) const override;
	virtual bool morphismStaticOk() const override;
	virtual bool morphismDynamicOk() const override;
	virtual bool morphismIso(const Configuration &cCodom, std::vector<std::size_t> &perm) const override;
	virtual bool morphismSpec(const Configuration &cCodom, std::vector<std::size_t> &perm) const override;
public: // IO
	virtual IO::Graph::Write::EdgeFake3DType getEdgeDepiction(std::size_t i) const override;
	virtual void printCoords(std::ostream &s, const std::vector<std::size_t> &vIds) const override;
	virtual std::string getEdgeAnnotation(std::size_t i) const override;
private:
	virtual std::string asRawStringImpl(std::function<std::size_t(const EmbeddingEdge&)> getNeighbourId) const override;
	virtual std::pair<std::string, bool> asPrettyStringImpl(std::function<std::size_t(const EmbeddingEdge&)> getNeighbourId) const override;
private:
	bool fixed;
};

} // namespace Stereo
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_STEREO_CONFIGURATION_TETRAHEDRAL_H */