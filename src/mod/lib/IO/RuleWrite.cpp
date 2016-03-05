#include "Rule.h"

#include <mod/Config.h>
#include <mod/Graph.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Chem/OBabel.h>
#include <mod/lib/IO/GraphWriteDetail.h>
#include <mod/lib/IO/MatchConstraint.h>
#include <mod/lib/Rule/Properties/String.h>
#include <mod/lib/Rule/Properties/Depiction.h>

#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace IO {
namespace Rule {
namespace Write {
namespace {

// returns the filename _without_ extension

const std::string &getFilePrefix(const lib::Rule::Real &r) {
	static std::map<unsigned int, std::string> cache;
	auto iter = cache.find(r.getId());
	if(iter == end(cache)) {
		std::string prefix = IO::getUniqueFilePrefix() + "r_" + boost::lexical_cast<std::string>(r.getId());
		return cache[r.getId()] = prefix;
	} else return iter->second;
}

void gmlSide(const lib::Rule::Real &r, std::ostream& s, lib::Rule::Membership printMembership, bool withCoords) {
	const auto &depict = r.getDepictionData();
	if(!depict.getHasCoordinates() && withCoords) MOD_ABORT;
	using Vertex = lib::Rule::Vertex;
	using Edge = lib::Rule::Edge;
	const lib::Rule::GraphType &core = r.getGraph();
	const lib::Rule::PropStringCore &labelState = r.getStringState();

	for(Vertex v : asRange(vertices(core))) {
		auto vMembership = core[v].membership;
		if(printMembership == lib::Rule::Membership::Context) {
			if(vMembership != lib::Rule::Membership::Context) continue;
			if(labelState.isChanged(v)) continue;
		} else {
			if(vMembership == lib::Rule::Membership::Context) {
				if(!labelState.isChanged(v)) continue;
			} else {
				if(printMembership != vMembership) continue;
			}
		}
		s << "\t\tnode [ id " << get(boost::vertex_index_t(), core, v) << " label \"";
		switch(printMembership) {
		case lib::Rule::Membership::Left:
			s << labelState.getLeft()[v];
			break;
		case lib::Rule::Membership::Context:
			s << labelState.getLeft()[v];
			break;
		case lib::Rule::Membership::Right:
			s << labelState.getRight()[v];
			break;
		}
		s << "\"";
		if(withCoords)
			s << " vis2d [ x " << depict.getX(v) << " y " << depict.getY(v) << " ]";
		s << " ]\n";
	}

	for(Edge e : asRange(edges(core))) {
		auto eMembership = core[e].membership;
		if(printMembership == lib::Rule::Membership::Context) {
			if(eMembership != lib::Rule::Membership::Context) continue;
			if(labelState.isChanged(e)) continue;
		} else {
			if(eMembership == lib::Rule::Membership::Context) {
				if(!labelState.isChanged(e)) continue;
			} else {
				if(printMembership != eMembership) continue;
			}
		}
		s << "\t\tedge [ source " << get(boost::vertex_index_t(), core, source(e, core))
				<< " target " << get(boost::vertex_index_t(), core, target(e, core))
				<< " label \"";
		switch(printMembership) {
		case lib::Rule::Membership::Left:
			s << labelState.getLeft()[e];
			break;
		case lib::Rule::Membership::Context:
			s << labelState.getLeft()[e];
			break;
		case lib::Rule::Membership::Right:
			s << labelState.getRight()[e];
			break;
		}
		s << "\" ]\n";
	}
}

void printEdgeStyle(std::ostream &s, lib::Rule::Membership eSide, int src, int tar) {
	s << "\t" << src << " -- " << tar << " [ ";
	switch(eSide) {
	case lib::Rule::Membership::Left:
		s << "style=dashed ";
		break;
	case lib::Rule::Membership::Right:
		s << "style=dotted ";
		break;
	default:
		break;
	}
}

} // namespace 

void gml(const lib::Rule::Real &r, bool withCoords, std::ostream &s) {
	s << "rule [" << std::endl;
	s << "\truleID \"" << r.getName() << "\"\n";
	{
		std::stringstream str;
		gmlSide(r, str, lib::Rule::Membership::Left, withCoords);
		if(str.str().size() > 0) s << "\tleft [\n" << str.str() << "\t]\n";
	}
	{
		std::stringstream str;
		gmlSide(r, str, lib::Rule::Membership::Context, withCoords);
		if(str.str().size() > 0) s << "\tcontext [\n" << str.str() << "\t]\n";
	}
	{
		std::stringstream str;
		gmlSide(r, str, lib::Rule::Membership::Right, withCoords);
		if(str.str().size() > 0) s << "\tright [\n" << str.str() << "\t]\n";
	}
	for(const auto &c : r.getLeftConstraints()) {
		lib::IO::MatchConstraint::Write::GMLPrintVisitor<lib::Rule::GraphType> printer(s, r.getGraph(), "\t");
		c->visit(printer);
	}
	s << "]";
}

std::string gml(const lib::Rule::Real &r, bool withCoords) {
	FileHandle s(getFilePrefix(r) + ".gml");
	gml(r, withCoords, s);
	return s;
}

std::string dotCombined(const lib::Rule::Real &r) {
	std::stringstream fileName;
	fileName << "r_" << r.getId() << "_combined.dot";
	FileHandle s(getUniqueFilePrefix() + fileName.str());
	std::string fileNoExt = s;
	fileNoExt.erase(fileNoExt.end() - 4, fileNoExt.end());
	using Vertex = lib::Rule::Vertex;
	using Edge = lib::Rule::Edge;
	const lib::Rule::GraphType &g = r.getGraph();
	const lib::Rule::PropStringCore &labelState = r.getStringState();
	s << "graph G {" << std::endl;
	for(Vertex v : asRange(vertices(g))) {
		auto membership = g[v].membership;
		s << "\t" << get(boost::vertex_index_t(), g, v) << " [ label=\"";
		switch(membership) {
		case lib::Rule::Membership::Left:
			s << labelState.getLeft()[v];
			break;
		case lib::Rule::Membership::Context:
			s << labelState.getLeft()[v] << " | " << labelState.getRight()[v];
			break;
		case lib::Rule::Membership::Right:
			s << labelState.getRight()[v];
			break;
		}
		s << "\"";
		switch(membership) {
		case lib::Rule::Membership::Left:
			s << " style=dashed";
			break;
		case lib::Rule::Membership::Right:
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
		case lib::Rule::Membership::Left:
			label = labelState.getLeft()[e];
			break;
		case lib::Rule::Membership::Context:
			label = labelState.getLeft()[e] + " | " + labelState.getRight()[e];
			break;
		case lib::Rule::Membership::Right:
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

std::string svgCombined(const lib::Rule::Real& r) {
	std::string fileNoExt = dotCombined(r);
	IO::post() << "gv ruleCombined \"" << fileNoExt << "\" svg" << std::endl;
	return fileNoExt;
}

std::string pdfCombined(const lib::Rule::Real& r) {
	std::string fileNoExt = svgCombined(r);
	IO::post() << "svgToPdf \"" << fileNoExt << "\"" << std::endl;
	return fileNoExt;
}

std::string dot(const lib::Rule::Real &r) {
	static std::set<unsigned int> cache;
	std::string fileNoExt = getFilePrefix(r);
	auto iter = cache.find(r.getId());
	if(iter != end(cache)) return fileNoExt;

	using Vertex = lib::Rule::Vertex;
	using Edge = lib::Rule::Edge;
	const lib::Rule::GraphType &g = r.getGraph();
	const lib::Rule::PropStringCore &labelState = r.getStringState();

	FileHandle s(fileNoExt + ".dot");
	s << "graph g {" << std::endl;
	s << getConfig().io.dotCoordOptions.get() << std::endl;
	for(Vertex v : asRange(vertices(g))) {
		unsigned int vId = get(boost::vertex_index_t(), g, v);
		s << vId << " [ label=\"";
		auto membership = g[v].membership;
		switch(membership) {
		case lib::Rule::Membership::Left:
			s << labelState.getLeft()[v];
			break;
		case lib::Rule::Membership::Context:
			s << labelState.getLeft()[v] << " | " << labelState.getRight()[v];
			break;
		case lib::Rule::Membership::Right:
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
		case lib::Rule::Membership::Left:
			s << labelState.getLeft()[e];
			break;
		case lib::Rule::Membership::Context:
			s << labelState.getLeft()[e] << " | " << labelState.getRight()[e];
			break;
		case lib::Rule::Membership::Right:
			s << labelState.getRight()[e];
			break;
		}
		s << "\" ];" << std::endl;
	}
	s << "}" << std::endl;
	return fileNoExt;
}

std::string coords(const lib::Rule::Real &r, unsigned int idOffset) {
	static std::map<unsigned int, std::string> cache;
	auto iter = cache.find(r.getId());
	if(iter != end(cache)) return iter->second;
	std::string fileNoExt = getFilePrefix(r);
	const auto &depict = r.getDepictionData();
	if(!depict.getHasCoordinates()) {
		if(idOffset != 0) {
			IO::log() << "Blame the lazy programmer. Offset " << idOffset << " not yet supported in dot coords." << std::endl;
			MOD_ABORT;
		}
		dot(r);
		IO::post() << "coordsFromGV rule \"" << fileNoExt << "\" noOverlay" << std::endl;
		fileNoExt = fileNoExt + ".coord";
		cache[r.getId()] = fileNoExt;
		return fileNoExt;
	} else {
		using Vertex = lib::Rule::Vertex;
		using Edge = lib::Rule::Edge;
		const auto &g = r.getGraph();
		std::string fileNoExt = getFilePrefix(r) + ".coord";
		FileHandle s(fileNoExt + ".tex");
		s << "% dummy" << std::endl;
		for(Vertex v : asRange(vertices(g))) {
			unsigned int vId = get(boost::vertex_index_t(), g, v);
			double x = depict.getX(v);
			double y = depict.getY(v);
			s << "\\coordinate[overlay] (v-coord-" << (vId + idOffset) << ") at (" << std::fixed << x << ", " << y << ") {};" << std::endl;
		}
		cache[r.getId()] = fileNoExt;
		return fileNoExt;
	}
}

std::pair<std::string, std::string> tikz(const std::string &fileCoordsNoExt, const lib::Rule::Real &r, unsigned int idOffset, const Options &options,
		const std::string &suffixL, const std::string &suffixK, const std::string &suffixR,
		std::function<bool(CoreVertex) > visible,
		std::function<std::string(CoreVertex) > vColour,
		std::function<std::string(CoreEdge) > eColour,
		std::function<bool(CoreVertex) > disallowCollapse) {
	std::string strOptions = options;
	std::string fileNoExt = IO::getUniqueFilePrefix() + "r_" + boost::lexical_cast<std::string>(r.getId());
	fileNoExt += +"." + strOptions;

	struct AdvOptions {

		AdvOptions(const lib::Rule::Real &r, unsigned int idOffset, std::function<bool(CoreVertex) > visible,
				std::function<std::string(CoreVertex) > vColour, std::function<std::string(CoreEdge) > eColour,
				std::function<bool(CoreVertex) > disallowCollapse)
		: idOffset(idOffset), changeColour(getConfig().rule.changeColour.get()), r(r), visible(visible), vColour(vColour), eColour(eColour),
		disallowCollapse_(disallowCollapse) { }

		bool isVisible(CoreVertex v) const {
			return visible(v);
		}

		std::string getColour(CoreVertex v) const {
			if(r.getGraph()[v].membership != lib::Rule::Membership::Context
					|| r.getStringState().isChanged(v)) return changeColour;
			else return vColour(v);
		}

		std::string getColour(CoreEdge e) const {
			if(r.getGraph()[e].membership != lib::Rule::Membership::Context
					|| r.getStringState().isChanged(e)) return changeColour;
			else return eColour(e);
		}

		bool disallowCollapse(CoreVertex v) const {
			if(r.getGraph()[v].membership != lib::Rule::Membership::Context)
				return true;
			for(auto e : asRange(out_edges(v, r.getGraph()))) {
				if(r.getGraph()[e].membership != lib::Rule::Membership::Context)
					return true;
			}
			return disallowCollapse_(v);
		}
	public:
		const unsigned int idOffset;
	private:
		const std::string changeColour;
		const lib::Rule::Real &r;
		std::function<bool(CoreVertex) > visible;
		std::function<std::string(CoreVertex) > vColour;
		std::function<std::string(CoreEdge) > eColour;
		std::function<bool(CoreVertex) > disallowCollapse_;
	};

	std::string fileCoords = fileCoordsNoExt + ".tex";
	{ // left
		FileHandle s(fileNoExt + "." + suffixL + ".tex");
		const auto &g = r.getLeft();
		const auto &depict = r.getDepictionData().getLeft();
		AdvOptions adv(r, idOffset, visible, vColour, eColour, disallowCollapse);
		IO::Graph::Write::tikz(s, options, g, depict, fileCoords, adv);
	}
	{ // context
		FileHandle s(fileNoExt + "." + suffixK + ".tex");
		const auto &g = r.getContext();
		const auto &depict = r.getDepictionData().getContext();

		struct EdgeVisible {

			EdgeVisible() : r(nullptr) { }

			EdgeVisible(const lib::Rule::Real &r) : r(&r) { }

			bool operator()(CoreEdge e) const {
				if(getConfig().rule.printChangedEdgesInContext.get()) return true;
				return !r->getStringState().isChanged(e);
			}
		private:
			const lib::Rule::Real *r;
		};
		boost::filtered_graph<lib::Rule::DPOProjection, EdgeVisible> gFiltered(g, EdgeVisible(r));
		AdvOptions adv(r, idOffset, visible, vColour, eColour, disallowCollapse);
		IO::Graph::Write::tikz(s, options, gFiltered, depict, fileCoords, adv);
	}
	{ // right
		FileHandle s(fileNoExt + "." + suffixR + ".tex");
		const auto &g = r.getRight();
		const auto &depict = r.getDepictionData().getRight();
		AdvOptions adv(r, idOffset, visible, vColour, eColour, disallowCollapse);
		IO::Graph::Write::tikz(s, options, g, depict, fileCoords, adv);
	}
	return std::make_pair(fileNoExt, fileCoordsNoExt);
}

std::pair<std::string, std::string> tikz(const lib::Rule::Real &r, unsigned int idOffset, const Options &options,
		const std::string &suffixL, const std::string &suffixK, const std::string &suffixR,
		std::function<bool(CoreVertex) > visible, std::function<std::string(CoreVertex) > vColour, std::function<std::string(CoreEdge) > eColour,
		std::function<bool(CoreVertex) > disallowCollapse) {
	std::string fileCoordsNoExt = coords(r, idOffset /*, options.collapseHydrogens*/);
	return tikz(fileCoordsNoExt, r, idOffset, options, suffixL, suffixK, suffixR, visible, vColour, eColour, disallowCollapse);
}

std::string pdf(const lib::Rule::Real &r, const Options &options,
		const std::string &suffixL, const std::string &suffixK, const std::string &suffixR,
		std::function<bool(CoreVertex) > visible, std::function<std::string(CoreVertex) > vColour, std::function<std::string(CoreEdge) > eColour) {
	std::string fileNoExt, fileCoordsNoExt;
	const unsigned int idOffset = 0;
	auto disallowCollapse = [](CoreVertex v) {
		return false;
	};
	std::tie(fileNoExt, fileCoordsNoExt) = tikz(r, idOffset, options, suffixL, suffixK, suffixR, visible, vColour, eColour, disallowCollapse);
	IO::post() << "compileTikz \"" << fileNoExt << "." << suffixL << "\" \"" << fileCoordsNoExt << "\"" << std::endl;
	IO::post() << "compileTikz \"" << fileNoExt << "." << suffixK << "\" \"" << fileCoordsNoExt << "\"" << std::endl;
	IO::post() << "compileTikz \"" << fileNoExt << "." << suffixR << "\" \"" << fileCoordsNoExt << "\"" << std::endl;
	return fileNoExt;
}

std::string pdfCombined(const lib::Rule::Real &r, const Options &options) {
	MOD_ABORT;
}

std::pair<std::string, std::string> summary(const lib::Rule::Real &r) {
	GraphPrinter first;
	GraphPrinter second;
	second.setReactionDefault();
	return summary(r, first.getOptions(), second.getOptions());
}

std::pair<std::string, std::string> summary(const lib::Rule::Real &r, const Options &first, const Options &second) {
	auto visible = [](CoreVertex) {
		return true;
	};
	auto vColour = [](CoreVertex) -> std::string {
		return "";
	};
	auto eColour = [](CoreEdge) -> std::string {
		return "";
	};
	std::string graphLike = pdf(r, first, "L", "K", "R", visible, vColour, eColour);
	std::string molLike = pdf(r, second, "L", "K", "R", visible, vColour, eColour);
	std::string combined = pdfCombined(r /*, Options().EdgesAsBonds().RaiseCharges()*/);
	std::string constraints = getUniqueFilePrefix() + "r_" + boost::lexical_cast<std::string>(r.getId()) + "_constraints.tex";
	{
		FileHandle s(constraints);
		for(const auto &c : r.getLeftConstraints())
			s << *c << '\n';
	}
	IO::post() << "summaryRule \"" << r.getName() << "\" \"" << graphLike << "\" \"" << molLike << "\" \"" << combined << "\" \"" << constraints << "\"" << std::endl;
	return std::make_pair(graphLike, molLike);
}

} // namespace Write
} // namespace Rule
} // namespace IO
} // namespace lib
} // namespace mod
