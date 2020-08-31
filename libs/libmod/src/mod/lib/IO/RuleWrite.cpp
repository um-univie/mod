#include "Rule.hpp"

#include <mod/Config.hpp>
#include <mod/graph/Printer.hpp>
#include <mod/lib/Chem/MoleculeUtil.hpp>
#include <mod/lib/Chem/OBabel.hpp>
#include <mod/lib/IO/FileHandle.hpp>
#include <mod/lib/IO/GraphWriteDetail.hpp>
#include <mod/lib/IO/MorphismConstraints.hpp>
#include <mod/lib/IO/Term.hpp>
#include <mod/lib/Rules/Real.hpp>
#include <mod/lib/Rules/Properties/String.hpp>
#include <mod/lib/Rules/Properties/Depiction.hpp>
#include <mod/lib/Rules/Properties/Term.hpp>
#include <mod/lib/Term/WAM.hpp>

#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace IO {
namespace Rules {
namespace Write {
namespace {

// returns the filename _without_ extension

const std::string &getFilePrefix(const lib::Rules::Real &r) {
	static std::map<unsigned int, std::string> cache;
	auto iter = cache.find(r.getId());
	if(iter == end(cache)) {
		std::string prefix = IO::getUniqueFilePrefix() + "r_" + boost::lexical_cast<std::string>(r.getId());
		return cache[r.getId()] = prefix;
	} else return iter->second;
}

void gmlSide(const lib::Rules::Real &r, std::ostream &s, lib::Rules::Membership printMembership, bool withCoords) {
	if(withCoords) {
		const auto &depict = r.getDepictionData();
		if(!depict.getHasCoordinates()) MOD_ABORT;
	}
	using Vertex = lib::Rules::Vertex;
	using Edge = lib::Rules::Edge;
	const lib::Rules::GraphType &core = r.getGraph();
	const lib::Rules::PropStringCore &labelState = r.getStringState();

	for(Vertex v : asRange(vertices(core))) {
		auto vMembership = core[v].membership;
		if(printMembership == lib::Rules::Membership::Context) {
			if(vMembership != lib::Rules::Membership::Context) continue;
			if(labelState.isChanged(v)) continue;
		} else {
			if(vMembership == lib::Rules::Membership::Context) {
				if(!labelState.isChanged(v)) continue;
			} else {
				if(printMembership != vMembership) continue;
			}
		}
		s << "\t\tnode [ id " << get(boost::vertex_index_t(), core, v) << " label \"";
		switch(printMembership) {
		case lib::Rules::Membership::Left:
			s << labelState.getLeft()[v];
			break;
		case lib::Rules::Membership::Context:
			s << labelState.getLeft()[v];
			break;
		case lib::Rules::Membership::Right:
			s << labelState.getRight()[v];
			break;
		}
		s << "\"";
		if(withCoords) {
			const auto &depict = r.getDepictionData();
			s << " vis2d [ x " << depict.getX(v, true) << " y " << depict.getY(v, true) << " ]";
		}
		s << " ]\n";
	}

	for(Edge e : asRange(edges(core))) {
		auto eMembership = core[e].membership;
		if(printMembership == lib::Rules::Membership::Context) {
			if(eMembership != lib::Rules::Membership::Context) continue;
			if(labelState.isChanged(e)) continue;
		} else {
			if(eMembership == lib::Rules::Membership::Context) {
				if(!labelState.isChanged(e)) continue;
			} else {
				if(printMembership != eMembership) continue;
			}
		}
		s << "\t\tedge [ source " << get(boost::vertex_index_t(), core, source(e, core))
		  << " target " << get(boost::vertex_index_t(), core, target(e, core))
		  << " label \"";
		switch(printMembership) {
		case lib::Rules::Membership::Left:
			s << labelState.getLeft()[e];
			break;
		case lib::Rules::Membership::Context:
			s << labelState.getLeft()[e];
			break;
		case lib::Rules::Membership::Right:
			s << labelState.getRight()[e];
			break;
		}
		s << "\" ]\n";
	}
}

void printEdgeStyle(std::ostream &s, lib::Rules::Membership eSide, int src, int tar) {
	s << "\t" << src << " -- " << tar << " [ ";
	switch(eSide) {
	case lib::Rules::Membership::Left:
		s << "style=dashed ";
		break;
	case lib::Rules::Membership::Right:
		s << "style=dotted ";
		break;
	default:
		break;
	}
}

} // namespace 

void gml(const lib::Rules::Real &r, bool withCoords, std::ostream &s) {
	s << "rule [" << std::endl;
	s << "\truleID \"" << r.getName() << "\"\n";
	if(r.getLabelType()) {
		s << "\tlabelType \"";
		switch(r.getLabelType().get()) {
		case LabelType::String:
			s << "string";
			break;
		case LabelType::Term:
			s << "term";
			break;
		}
		s << "\"\n";
	}
	{
		std::stringstream str;
		gmlSide(r, str, lib::Rules::Membership::Left, withCoords);
		if(str.str().size() > 0) s << "\tleft [\n" << str.str() << "\t]\n";
	}
	{
		std::stringstream str;
		gmlSide(r, str, lib::Rules::Membership::Context, withCoords);
		if(str.str().size() > 0) s << "\tcontext [\n" << str.str() << "\t]\n";
	}
	{
		std::stringstream str;
		gmlSide(r, str, lib::Rules::Membership::Right, withCoords);
		if(str.str().size() > 0) s << "\tright [\n" << str.str() << "\t]\n";
	}
	auto printer = lib::IO::MatchConstraint::Write::makeGMLPrintVisitor(s, get_left(r.getDPORule()), "\t");
	for(const auto &c : r.getDPORule().leftMatchConstraints) {
		c->accept(printer);
	}
	s << "]";
}

std::string gml(const lib::Rules::Real &r, bool withCoords) {
	FileHandle s(getFilePrefix(r) + ".gml");
	gml(r, withCoords, s);
	return s;
}

std::string dotCombined(const lib::Rules::Real &r) {
	std::stringstream fileName;
	fileName << "r_" << r.getId() << "_combined.dot";
	FileHandle s(getUniqueFilePrefix() + fileName.str());
	std::string fileNoExt = s;
	fileNoExt.erase(fileNoExt.end() - 4, fileNoExt.end());
	using Vertex = lib::Rules::Vertex;
	using Edge = lib::Rules::Edge;
	const lib::Rules::GraphType &g = r.getGraph();
	const lib::Rules::PropStringCore &labelState = r.getStringState();
	s << "graph G {" << std::endl;
	for(Vertex v : asRange(vertices(g))) {
		auto membership = g[v].membership;
		s << "\t" << get(boost::vertex_index_t(), g, v) << " [ label=\"";
		switch(membership) {
		case lib::Rules::Membership::Left:
			s << labelState.getLeft()[v];
			break;
		case lib::Rules::Membership::Context:
			s << labelState.getLeft()[v] << " | " << labelState.getRight()[v];
			break;
		case lib::Rules::Membership::Right:
			s << labelState.getRight()[v];
			break;
		}
		s << "\"";
		switch(membership) {
		case lib::Rules::Membership::Left:
			s << " style=dashed";
			break;
		case lib::Rules::Membership::Right:
			s << " style=dotted";
			break;
		default:
			break;
		}
		s << " ]" << std::endl;
	}

	for(Edge e : asRange(edges(g))) {
		auto membership = g[e].membership;
		auto vSrcId = get(boost::vertex_index_t(), g, source(e, g));
		auto vTarId = get(boost::vertex_index_t(), g, target(e, g));
		std::string label;
		switch(membership) {
		case lib::Rules::Membership::Left:
			label = labelState.getLeft()[e];
			break;
		case lib::Rules::Membership::Context:
			label = labelState.getLeft()[e] + " | " + labelState.getRight()[e];
			break;
		case lib::Rules::Membership::Right:
			label = labelState.getRight()[e];
			break;
		}
		switch(label[0]) {
			//		case '=': // fall through to make two edges
			//			//						assert(false);
			//			printEdgeStyle(s, membership, vSrcId, vTarId);
			//			s << "]" << std::endl;
			//		case '-': // print the rest of the label
			//			printEdgeStyle(s, membership, vSrcId, vTarId);
			//			s << "label=\"" << (label.c_str() + 1) << "\" ]" << std::endl;
			//			break;
		default:
			printEdgeStyle(s, membership, vSrcId, vTarId);
			s << "label=\"" << label << "\" ]" << std::endl;
			break;
		}
	}
	s << "}" << std::endl;
	return fileNoExt;
}

std::string svgCombined(const lib::Rules::Real &r) {
	std::string fileNoExt = dotCombined(r);
	IO::post() << "gv ruleCombined \"" << fileNoExt << "\" svg" << std::endl;
	return fileNoExt;
}

std::string pdfCombined(const lib::Rules::Real &r) {
	std::string fileNoExt = svgCombined(r);
	IO::post() << "svgToPdf \"" << fileNoExt << "\"" << std::endl;
	return fileNoExt;
}

std::string dot(const lib::Rules::Real &r) {
	static std::set<unsigned int> cache;
	std::string fileNoExt = getFilePrefix(r);
	auto iter = cache.find(r.getId());
	if(iter != end(cache)) return fileNoExt;

	using Vertex = lib::Rules::Vertex;
	using Edge = lib::Rules::Edge;
	const lib::Rules::GraphType &g = r.getGraph();
	const lib::Rules::PropStringCore &labelState = r.getStringState();

	FileHandle s(fileNoExt + ".dot");
	s << "graph g {" << std::endl;
	s << getConfig().io.dotCoordOptions.get() << std::endl;
	for(Vertex v : asRange(vertices(g))) {
		unsigned int vId = get(boost::vertex_index_t(), g, v);
		s << vId << " [ label=\"";
		auto membership = g[v].membership;
		switch(membership) {
		case lib::Rules::Membership::Left:
			s << labelState.getLeft()[v];
			break;
		case lib::Rules::Membership::Context:
			s << labelState.getLeft()[v] << " | " << labelState.getRight()[v];
			break;
		case lib::Rules::Membership::Right:
			s << labelState.getRight()[v];
			break;
		}
		s << "\" ];" << std::endl;
	}
	for(Edge e : asRange(edges(g))) {
		unsigned int vSrcId = get(boost::vertex_index_t(), g, source(e, g));
		unsigned int vTarId = get(boost::vertex_index_t(), g, target(e, g));
		s << vSrcId << " -- " << vTarId << " [ label=\"";
		auto membership = g[e].membership;
		switch(membership) {
		case lib::Rules::Membership::Left:
			s << labelState.getLeft()[e];
			break;
		case lib::Rules::Membership::Context:
			s << labelState.getLeft()[e] << " | " << labelState.getRight()[e];
			break;
		case lib::Rules::Membership::Right:
			s << labelState.getRight()[e];
			break;
		}
		s << "\" ];" << std::endl;
	}
	s << "}" << std::endl;
	return fileNoExt;
}

struct CoordsCacheEntry {
	std::size_t id;
	bool collapseHydrogens;
	int rotation;
	bool mirror;
public:

	friend bool operator<(const CoordsCacheEntry &a, const CoordsCacheEntry &b) {
		return std::tie(a.id, a.collapseHydrogens, a.rotation, a.mirror)
		       < std::tie(b.id, b.collapseHydrogens, b.rotation, b.mirror);
	}
};

namespace {

bool disallowVertexCollapse(const lib::Rules::Real &r, const CoreVertex v,
                            std::function<bool(CoreVertex)> disallowCollapse) {
	if(getConfig().rule.collapseChangedHydrogens.get())
		return disallowCollapse(v);
	if(r.getGraph()[v].membership != lib::Rules::Membership::Context)
		return true;
	for(const auto e : asRange(out_edges(v, r.getGraph()))) {
		if(r.getGraph()[e].membership != lib::Rules::Membership::Context)
			return true;
	}
	return disallowCollapse(v);
}

} // namespace

std::string coords(const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
                   std::function<bool(CoreVertex)> disallowCollapse_) {
	static std::map<CoordsCacheEntry, std::string> cache;
	const auto iter = cache.find({r.getId(), options.collapseHydrogens, options.rotation, options.mirror});
	if(iter != end(cache)) return iter->second;
	std::string fileNoExt = getFilePrefix(r);
	const auto &depict = r.getDepictionData();
	if(!depict.getHasCoordinates()) {
		if(idOffset != 0) {
			IO::log() << "Blame the lazy programmer. Offset " << idOffset << " not yet supported in dot coords."
			          << std::endl;
			MOD_ABORT;
		}
		dot(r);
		IO::post() << "coordsFromGV rule \"" << fileNoExt << "\" noOverlay" << std::endl;
		fileNoExt = fileNoExt + "_coord";
		cache[{r.getId(), true, options.rotation, options.mirror}
		] = fileNoExt;
		cache[{r.getId(), false, options.rotation, options.mirror}
		] = fileNoExt;
		return fileNoExt;
	} else {
		const auto &g = r.getGraph();
		const std::string molString = options.collapseHydrogens ? "_mol" : "";
		const std::string fileNoExt = getFilePrefix(r) + molString + "_coord";
		FileHandle s(fileNoExt + ".tex");
		s << "% dummy\n";
		const bool useCollapsedCoords = [&]() {
			if(!options.collapseHydrogens) return false;
			for(const auto v : asRange(vertices(g))) {
				if(disallowCollapse_(v)) return false;
			}
			return true;
		}();

		for(const auto v : asRange(vertices(g))) {
			const auto vId = get(boost::vertex_index_t(), g, v);
			const bool printCoord = [&]() {
				if(!useCollapsedCoords) return true;
				if(disallowVertexCollapse(r, v, disallowCollapse_)) return true;
				return !Chem::isCollapsible(v, g, depict, depict, [&depict](const auto v) {
					return depict.hasImportantStereo(v);
				});
			}();
			if(!printCoord) continue;
			double x, y;
			std::tie(x, y) = pointTransform(
					depict.getX(v, !useCollapsedCoords),
					depict.getY(v, !useCollapsedCoords),
					options.rotation, options.mirror);
			s << "\\coordinate[overlay] (v-coord-" << (vId + idOffset) << ") at (" << std::fixed << x << ", " << y
			  << ") {};" << std::endl;
		}
		if(options.collapseHydrogens && !useCollapsedCoords) {
			// don't cache these as the user predicate influences it
		} else {
			cache[{r.getId(), options.collapseHydrogens, options.rotation, options.mirror}
			] = fileNoExt;
		}
		return fileNoExt;
	}
}

namespace {

template<lib::Rules::Membership membership>
struct AdvOptions {
	AdvOptions(const lib::Rules::Real &r, unsigned int idOffset, const BaseArgs &args,
	           std::function<bool(CoreVertex)> disallowCollapse)
			: idOffset(idOffset), changeColour(changeColourFromMembership()), r(r), args(args),
			  disallowCollapse_(disallowCollapse) {}

private:
	static std::string changeColourFromMembership() {
		std::string side = []() {
			switch(membership) {
			case lib::Rules::Membership::Left:
				return getConfig().rule.changeColourL.get();
			case lib::Rules::Membership::Context:
				return getConfig().rule.changeColourK.get();
			case lib::Rules::Membership::Right:
				return getConfig().rule.changeColourR.get();
			}
		}();
		if(side.empty()) return getConfig().rule.changeColour.get();
		else return side;
	}

public:
	std::string getColour(CoreVertex v) const {
		bool isChanged = r.getGraph()[v].membership != lib::Rules::Membership::Context
		                 || r.getStringState().isChanged(v);
		if(isChanged) {
			return changeColour;
		} else return args.vColour(v);
	}

	std::string getColour(CoreEdge e) const {
		bool isChanged = r.getGraph()[e].membership != lib::Rules::Membership::Context
		                 || r.getStringState().isChanged(e);
		if(isChanged) {
			return changeColour;
		} else return args.eColour(e);
	}

	bool isVisible(CoreVertex v) const {
		return args.visible(v);
	}

	std::string getShownId(CoreVertex v) const {
		return boost::lexical_cast<std::string>(get(boost::vertex_index_t(), r.getGraph(), v));
	}

	bool overwriteWithIndex(CoreVertex) const {
		return false;
	}

	lib::IO::Graph::Write::EdgeFake3DType getEdgeFake3DType(CoreEdge e, bool withHydrogen) const {
		const auto get = [&](const auto &depict) {
			return depict.getEdgeFake3DType(e, withHydrogen);
		};
		const auto &depict = r.getDepictionData();
		switch(membership) {
		case lib::Rules::Membership::Left:
			return get(depict.getLeft());
		case lib::Rules::Membership::Context:
			return get(depict.getContext());
		case lib::Rules::Membership::Right:
			return get(depict.getRight());
		}
	}

	std::string getOpts(CoreVertex v) const {
		return std::string();
	}

private:

	template<typename F>
	std::string getStereoStringVertex(const CoreVertex v, const F f) const {
		const auto apply = [&](const auto &lg) {
			const auto &conf = *get_stereo(lg)[v];
			const auto getNeighbourId = [&](const lib::Stereo::EmbeddingEdge &emb) {
				const auto &g = get_graph(lg);
				return get(boost::vertex_index_t(), g, target(emb.getEdge(v, g), g));
			};
			std::string res = f(conf, getNeighbourId);
			if(!get_stereo(r.getDPORule()).inContext(v)) {
				res = "{\\color{" + getConfig().rule.changeColourL.get() + "}" + res + "}";
			}
			return res;
		};

		const auto &lr = r.getDPORule();
		switch(membership) {
		case lib::Rules::Membership::Left:
			return apply(get_labelled_left(lr));
		case lib::Rules::Membership::Right:
			return apply(get_labelled_right(lr));
		case lib::Rules::Membership::Context:
			if(get_stereo(lr).inContext(v)) {
				const auto &lg = get_labelled_left(r.getDPORule());
				const auto &conf = *get_stereo(lg)[v];
				const auto getNeighbourId = [&](const lib::Stereo::EmbeddingEdge &emb) {
					const auto &g = get_graph(lg);
					return get(boost::vertex_index_t(), g, target(emb.getEdge(v, g), g));
				};
				return f(conf, getNeighbourId);
			} else return "";
		}
	}

public:

	std::string getRawStereoString(CoreVertex v) const {
		return getStereoStringVertex(v, [&](const auto &conf, auto getNId) {
			return conf.asRawString(getNId);
		});
	}

	std::string getPrettyStereoString(CoreVertex v) const {
		return getStereoStringVertex(v, [&](const auto &conf, auto getNId) {
			return conf.asPrettyString(getNId);
		});
	}

	std::string getStereoString(CoreEdge e) const {
		const auto apply = [&](const auto &lg) {
			const auto cat = get_stereo(lg)[e];
			std::string res = boost::lexical_cast<std::string>(cat);
			if(!get_stereo(r.getDPORule()).inContext(e)) {
				res = "{\\color{" + getConfig().rule.changeColourL.get() + "}" + res + "}";
			}
			return res;
		};

		const auto &lr = r.getDPORule();
		switch(membership) {
		case lib::Rules::Membership::Left:
			return apply(get_labelled_left(lr));
		case lib::Rules::Membership::Right:
			return apply(get_labelled_right(lr));
		case lib::Rules::Membership::Context:
			if(get_stereo(lr).inContext(e)) {
				const auto &lg = get_labelled_left(r.getDPORule());
				const auto cat = get_stereo(lg)[e];
				return boost::lexical_cast<std::string>(cat);
			} else return "";
		}
	}

	std::string getEdgeAnnotation(CoreEdge) const {
		return "";
	}

	bool disallowCollapse(CoreVertex v) const {
		return disallowVertexCollapse(r, v, disallowCollapse_);
	}

public:
	const unsigned int idOffset;
private:
	const std::string changeColour;
	const lib::Rules::Real &r;
	const BaseArgs &args;
	std::function<bool(CoreVertex)> disallowCollapse_;
};

} // namespace

std::pair<std::string, std::string>
tikz(const std::string &fileCoordsNoExt, const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
     const std::string &suffixL, const std::string &suffixK, const std::string &suffixR, const BaseArgs &args,
     std::function<bool(CoreVertex)> disallowCollapse) {
	std::string strOptions = options.getStringEncoding();
	std::string fileNoExt = IO::getUniqueFilePrefix() + "r_" + boost::lexical_cast<std::string>(r.getId());
	fileNoExt += "_" + strOptions;

	std::string fileCoords = fileCoordsNoExt + ".tex";
	{ // left
		FileHandle s(fileNoExt + "_" + suffixL + ".tex");
		const auto &g = get_left(r.getDPORule());
		const auto &depict = r.getDepictionData().getLeft();
		const auto adv = AdvOptions<lib::Rules::Membership::Left>(r, idOffset, args, disallowCollapse);
		IO::Graph::Write::tikz(s, options, g, depict, fileCoords, adv, jla_boost::Nop<>(), "");
	}
	{ // context
		FileHandle s(fileNoExt + "_" + suffixK + ".tex");
		const auto &g = get_context(r.getDPORule());
		const auto &depict = r.getDepictionData().getContext();

		struct EdgeVisible {
			EdgeVisible() = default;

			EdgeVisible(const lib::Rules::Real &r) : r(&r) {}

			bool operator()(const CoreEdge e) const {
				if(getConfig().rule.printChangedEdgesInContext.get()) return true;
				return !r->getStringState().isChanged(e);
			}

		private:
			const lib::Rules::Real *r = nullptr;
		};
		boost::filtered_graph<lib::Rules::DPOProjection, EdgeVisible> gFiltered(g, EdgeVisible(r));
		const auto adv = AdvOptions<lib::Rules::Membership::Context>(r, idOffset, args, disallowCollapse);
		IO::Graph::Write::tikz(s, options, gFiltered, depict, fileCoords, adv, jla_boost::Nop<>(), "");
	}
	{ // right
		FileHandle s(fileNoExt + "_" + suffixR + ".tex");
		const auto &g = get_right(r.getDPORule());
		const auto &depict = r.getDepictionData().getRight();
		const auto adv = AdvOptions<lib::Rules::Membership::Right>(r, idOffset, args, disallowCollapse);
		IO::Graph::Write::tikz(s, options, g, depict, fileCoords, adv, jla_boost::Nop<>(), "");
	}
	return std::make_pair(fileNoExt, fileCoordsNoExt);
}

std::pair<std::string, std::string> tikz(const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
                                         const std::string &suffixL, const std::string &suffixK,
                                         const std::string &suffixR, const BaseArgs &args,
                                         std::function<bool(CoreVertex)> disallowCollapse) {
	std::string fileCoordsNoExt = coords(r, idOffset, options, disallowCollapse);
	return tikz(fileCoordsNoExt, r, idOffset, options, suffixL, suffixK, suffixR, args, disallowCollapse);
}

std::string pdf(const lib::Rules::Real &r, const Options &options,
                const std::string &suffixL, const std::string &suffixK, const std::string &suffixR,
                const BaseArgs &args) {
	std::string fileNoExt, fileCoordsNoExt;
	const unsigned int idOffset = 0;
	std::tie(fileNoExt, fileCoordsNoExt) = tikz(r, idOffset, options, suffixL, suffixK, suffixR, args,
	                                            jla_boost::AlwaysFalse());
	IO::post() << "compileTikz \"" << fileNoExt << "_" << suffixL << "\" \"" << fileCoordsNoExt << "\"\n";
	IO::post() << "compileTikz \"" << fileNoExt << "_" << suffixK << "\" \"" << fileCoordsNoExt << "\"\n";
	IO::post() << "compileTikz \"" << fileNoExt << "_" << suffixR << "\" \"" << fileCoordsNoExt << "\"\n";
	IO::post().flush();
	return fileNoExt;
}

std::pair<std::string, std::string>
tikzTransitionState(const std::string &fileCoordsNoExt, const lib::Rules::Real &r, unsigned int idOffset,
                    const Options &options,
                    const std::string &suffix, const BaseArgs &args) {
	MOD_ABORT;
}

std::pair<std::string, std::string>
tikzTransitionState(const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
                    const std::string &suffix, const BaseArgs &args) {
	MOD_ABORT;
}

std::string pdfTransitionState(const lib::Rules::Real &r, const Options &options,
                               const std::string &suffix, const BaseArgs &args) {
	std::string fileNoExt, fileCoordsNoExt;
	const unsigned int idOffset = 0;
	std::tie(fileNoExt, fileCoordsNoExt) = tikzTransitionState(r, idOffset, options, suffix, args);
	IO::post() << "compileTikz \"" << fileNoExt << "_" << suffix << "\" \"" << fileCoordsNoExt << "\"" << std::endl;
	return fileNoExt;
}

std::string pdfCombined(const lib::Rules::Real &r, const Options &options) {
	MOD_ABORT;
}

std::pair<std::string, std::string> summary(const lib::Rules::Real &r) {
	graph::Printer first;
	graph::Printer second;
	second.setReactionDefault();
	return summary(r, first.getOptions(), second.getOptions());
}

std::pair<std::string, std::string> summary(const lib::Rules::Real &r, const Options &first, const Options &second) {
	auto visible = jla_boost::AlwaysTrue();
	auto vColour = jla_boost::Nop<std::string>();
	auto eColour = jla_boost::Nop<std::string>();
	const BaseArgs args{visible, vColour, eColour};
	std::string graphLike = pdf(r, first, "L", "K", "R", args);
	std::string molLike = first == second ? "" : pdf(r, second, "L", "K", "R", args);
	std::string combined = getConfig().rule.printCombined.get()
	                       ? pdfCombined(r /*, Options().EdgesAsBonds().RaiseCharges()*/)
	                       : "";
	std::string constraints =
			getUniqueFilePrefix() + "r_" + boost::lexical_cast<std::string>(r.getId()) + "_constraints.tex";
	{
		FileHandle s(constraints);
		auto printer = lib::IO::MatchConstraint::Write::makeTexPrintVisitor(s, get_left(r.getDPORule()));
		for(const auto &c : r.getDPORule().leftMatchConstraints) {
			c->accept(printer);
			s << "\n";
		}
	}
	IO::post() << "summaryRule \"" << r.getName() << "\" \"" << graphLike << "\" \"" << molLike << "\" \"" << combined
	           << "\" \"" << constraints << "\"" << std::endl;
	if(molLike.empty())
		return std::make_pair(graphLike, graphLike);
	else
		return std::make_pair(graphLike, molLike);
}

void termState(const lib::Rules::Real &r) {
	using Vertex = lib::Rules::Vertex;
	using Edge = lib::Rules::Edge;
	using Membership = lib::Rules::Membership;
	using namespace lib::Term;
	IO::post() << "summarySubsection \"Term State for " << r.getName() << "\"" << std::endl;
	FileHandle s(getUniqueFilePrefix() + "termState.tex");
	s << "\\begin{verbatim}\n";
	const auto &termState = r.getTermState();
	if(isValid(termState)) {
		std::unordered_map<Address, std::set<std::pair<Vertex, Membership> > > addrToVertex;
		std::unordered_map<Address, std::set<std::pair<Edge, Membership> > > addrToEdge;
		std::unordered_map<Address, std::set<std::string> > addrToConstraintInfo;
		auto insertVertex = [&addrToVertex](std::size_t addr, Vertex v, Membership membership) {
			Address a{AddressType::Heap, addr};
			addrToVertex[a].insert({v, membership});
		};
		auto insertEdge = [&addrToEdge](std::size_t addr, Edge e, Membership membership) {
			Address a{AddressType::Heap, addr};
			addrToEdge[a].insert({e, membership});
		};
		for(Vertex v : asRange(vertices(r.getGraph()))) {
			switch(r.getGraph()[v].membership) {
			case Membership::Left:
				insertVertex(termState.getLeft()[v], v, Membership::Left);
				break;
			case Membership::Right:
				insertVertex(termState.getRight()[v], v, Membership::Right);
				break;
			case Membership::Context:
				insertVertex(termState.getLeft()[v], v, Membership::Left);
				insertVertex(termState.getRight()[v], v, Membership::Right);
				break;
			}
		}
		for(Edge e : asRange(edges(r.getGraph()))) {
			switch(r.getGraph()[e].membership) {
			case Membership::Left:
				insertEdge(termState.getLeft()[e], e, Membership::Left);
				break;
			case Membership::Right:
				insertEdge(termState.getRight()[e], e, Membership::Right);
				break;
			case Membership::Context:
				insertEdge(termState.getLeft()[e], e, Membership::Left);
				insertEdge(termState.getRight()[e], e, Membership::Right);
				break;
			}
		}

		struct Visitor : lib::GraphMorphism::Constraints::AllVisitor<lib::Rules::SideGraphType> {

			Visitor(std::unordered_map<Address, std::set<std::string> > &addrMap, const lib::Rules::GraphType &g)
					: addrMap(addrMap), g(g) {}

			virtual void
			operator()(const lib::GraphMorphism::Constraints::VertexAdjacency<lib::Rules::SideGraphType> &c) override {
				const auto vStr = boost::lexical_cast<std::string>(get(boost::vertex_index_t(), g, c.vConstrained));
				for(const auto a : c.vertexTerms) {
					Address addr{AddressType::Heap, a};
					std::string msg = "VertexAdj(" + vStr + ", " + side + ", V)";
					addrMap[addr].insert(std::move(msg));
				}
				for(const auto a : c.edgeTerms) {
					Address addr{AddressType::Heap, a};
					std::string msg = "VertexAdj(" + vStr + ", " + side + ", E)";
					addrMap[addr].insert(std::move(msg));
				}
			}

			virtual void
			operator()(const lib::GraphMorphism::Constraints::ShortestPath<lib::Rules::SideGraphType> &c) override {}
		private:
			int counter = 0;
		public:
			std::unordered_map<Address, std::set<std::string> > &addrMap;
			const lib::Rules::GraphType &g;
			std::string side;
		};
		Visitor vis(addrToConstraintInfo, r.getGraph());
		vis.side = "L";
		for(const auto &c : r.getDPORule().leftMatchConstraints)
			c->accept(vis);
		vis.side = "R";
		for(const auto &c : r.getDPORule().rightMatchConstraints)
			c->accept(vis);

		lib::IO::Term::Write::wam(getMachine(termState), lib::Term::getStrings(), s, [&](Address addr, std::ostream &s) {
			s << "        ";
			bool first = true;
			for(auto vm : addrToVertex[addr]) {
				if(!first) s << ", ";
				first = false;
				s << "v(" << get(boost::vertex_index_t(), r.getGraph(), vm.first) << ", ";
				switch(vm.second) {
				case Membership::Left:
					s << "L";
					break;
				case Membership::Right:
					s << "R";
					break;
				case Membership::Context:
					s << "K";
					break;
				}
				s << ")";
			}
			for(auto em : addrToEdge[addr]) {
				if(!first) s << ", ";
				first = false;
				s << "e("
				  << get(boost::vertex_index_t(), r.getGraph(), source(em.first, r.getGraph()))
				  << ", "
				  << get(boost::vertex_index_t(), r.getGraph(), target(em.first, r.getGraph()))
				  << ", ";
				switch(em.second) {
				case Membership::Left:
					s << "L";
					break;
				case Membership::Right:
					s << "R";
					break;
				case Membership::Context:
					s << "K";
					break;
				}
				s << ")";
			}
			for(auto &msg : addrToConstraintInfo[addr]) {
				if(!first) s << ", ";
				first = false;
				s << msg;
			}
		});
	} else {
		std::string msg = "Parsing failed for rule '" + r.getName() + "'. " + termState.getParsingError();
		throw TermParsingError(std::move(msg));
	}
	s << "\\end{verbatim}\n";
	IO::post() << "summaryInput \"" << std::string(s) << "\"" << std::endl;
}

} // namespace Write
} // namespace Rules
} // namespace IO
} // namespace lib
} // namespace mod