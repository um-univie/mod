#ifndef MOD_LIB_STEREO_CONFIGURATION_ANY_H
#define MOD_LIB_STEREO_CONFIGURATION_ANY_H

#include <mod/lib/Stereo/Configuration/Configuration.h>

namespace mod {
namespace lib {
namespace Stereo {

struct Any final : DynamicDegree {
	Any(const GeometryGraph &g, const EmbeddingEdge *b, const EmbeddingEdge *e);
	virtual std::unique_ptr<Configuration> cloneFree(const GeometryGraph &g) const override;
	// this should never be called, as the fixation is always free
	virtual std::unique_ptr<Configuration> clone(const GeometryGraph &g, const std::vector<std::size_t> &offsetMap) const override;
public: // checking
	// these should never be called, as the morphismStaticOk should be used first
	virtual bool morphismIso(const Configuration &cCodom, std::vector<std::size_t> &perm) const override;
	virtual bool morphismSpec(const Configuration &cCodom, std::vector<std::size_t> &perm) const override;
public: // IO
	virtual void printCoords(std::ostream &s, const std::vector<std::size_t> &vIds) const override;
private:
	virtual std::pair<std::string, bool> asPrettyStringImpl(std::function<std::size_t(const EmbeddingEdge&)> getNeighbourId) const override;
};

} // namespace Stereo
} // namespace lib
} // namespace mod
#endif /* MOD_LIB_STEREO_CONFIGURATION_ANY_H */

