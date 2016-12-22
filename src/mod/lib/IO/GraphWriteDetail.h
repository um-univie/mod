#ifndef MOD_LIB_IO_GRAPHWRITEDETAIL_H
#define MOD_LIB_IO_GRAPHWRITEDETAIL_H

#include <mod/Error.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/IO/Graph.h>
#include <mod/lib/IO/IO.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/lexical_cast.hpp>

namespace {
namespace Loc {
static constexpr unsigned int
		/*                      */T = 16,
		/*        */T_left = 32, /*     */T_right = 8,
		//
		/*   */TL = 64, /*                     */TR = 4,
		//
		/**/L_up = 128, /*                        */R_up = 2,
		//
		//
		L = 256, /*                                   */R = 1,
		//
		//
		/**/L_down = 512, /*                      */R_down = 32768,
		//
		/*   */BL = 1024, /*                   */BR = 16384,
		//
		/*        */B_left = 2048, /*   */B_right = 8192,
		/*                      */B = 4096
		;
static constexpr unsigned int
		R_all = Loc::R | Loc::R_up | Loc::R_down,
		L_all = Loc::L | Loc::L_up | Loc::L_down,
		T_all = Loc::T | Loc::T_left | Loc::T_right,
		B_all = Loc::B | Loc::B_left | Loc::B_right,
		R_side = R_all | TR | T_right | BR | B_right,
		L_side = L_all | TL | T_left | BL | B_left,
		T_side = T_all | TR | R_up | TL | L_up,
		B_side = B_all | BR | R_down | BL | L_down
		;
} // namespace Loc
} // namespace

namespace mod {
namespace lib {
namespace IO {

constexpr double pi = 3.14159265358979323846;

inline std::pair<double, double> pointRotation(double xRaw, double yRaw, int rotation) {
	double angle = rotation * pi / 180;
	auto s = std::sin(angle);
	auto c = std::cos(angle);
	double x = xRaw * c - yRaw * s;
	double y = xRaw * s + yRaw * c;
	return std::make_pair(x, y);
}

namespace Graph {
namespace Write {

template<typename Graph, typename Depict, typename AdvOptions>
void tikz(std::ostream &s, const Options &options, const Graph &g, const Depict &depict, const std::string &fileCoords, const AdvOptions &advOptions) {
	typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
	typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;

	// set up data
	std::vector<bool> isVisible(num_vertices(g), true);
	std::vector<bool> isSimpleCarbon(num_vertices(g), false);
	std::vector<unsigned int> implicitHydrogenCount(num_vertices(g), 0);
	{ // visibility from adv
		for(Vertex v : asRange(vertices(g))) {
			unsigned int vId = get(boost::vertex_index_t(), g, v);
			isVisible[vId] = advOptions.isVisible(v);
		}
	}
	{ // stuff not depending on coordinates being available now
		if(options.simpleCarbons) {
			for(Vertex v : asRange(vertices(g))) {
				if(depict.getAtomId(v) != AtomIds::Carbon) continue;
				unsigned int adjCount = 0;
				for(Vertex vAdj : asRange(adjacent_vertices(v, g))) {
					// for now we allow it even though the edges are not bonds
					auto adjAtomId = depict.getAtomId(vAdj);
					switch(adjAtomId) {
					case AtomIds::Carbon:
					case AtomIds::Oxygen:
					case AtomIds::Nitrogen:
					case AtomIds::Sulfur:
						adjCount++;
					}
				}
				if(adjCount < 2) continue;
				if(depict.getCharge(v) != 0) continue;
				if(depict.getRadical(v)) continue;
				isSimpleCarbon[get(boost::vertex_index_t(), g, v)] = true;
			}
		}

		if(options.collapseHydrogens) { // collapse most hydrogens to it's neighbour
			for(Vertex v : asRange(vertices(g))) {
				unsigned int vId = get(boost::vertex_index_t(), g, v);
				if(!isVisible[vId]) continue;
				if(Chem::isCollapsible(v, g, depict, depict)) {
					assert(out_degree(v, g) == 1);
					Edge e = *out_edges(v, g).first;
					Vertex vAdj = target(e, g); // TODO: just use adjacent_vertices, we don't need the edge
					unsigned int vAdjId = get(boost::vertex_index_t(), g, vAdj);
					if(!isVisible[vAdjId]) continue;
					if(advOptions.disallowCollapse(v)) continue;
					implicitHydrogenCount[vAdjId]++;
					isVisible[vId] = false;
				}
			}
		}
	}

	auto areAllBlocked = [](unsigned int b) {
		return (b & (Loc::R_down * 2 - 1)) == (Loc::R_down * 2 - 1);
	};
	std::vector<unsigned int> auxLabelBlocked(num_vertices(g), 0);
	if(depict.getHasCoordinates()) { // stuff where we absolutely need coordinates right now
		// see where we can have aux data
		for(Vertex v : asRange(vertices(g))) {
			unsigned int vId = get(boost::vertex_index_t(), g, v);
			if(!isVisible[vId]) continue;
			double x, y;
			std::tie(x, y) = pointRotation(
					depict.getX(v, !options.collapseHydrogens),
					depict.getY(v, !options.collapseHydrogens),
					options.rotation);
			for(Vertex vAdj : asRange(adjacent_vertices(v, g))) {
				unsigned int vAdjId = get(boost::vertex_index_t(), g, vAdj);
				if(!isVisible[vAdjId]) continue;
				double xAdj, yAdj;
				std::tie(xAdj, yAdj) = pointRotation(
						depict.getX(vAdj, !options.collapseHydrogens),
						depict.getY(vAdj, !options.collapseHydrogens),
						options.rotation);
				auto &blocked = auxLabelBlocked[vId];
				constexpr double eps = 0.000001;
				double xDiff = xAdj - x;
				double yDiff = yAdj - y;
				double angle = std::atan2(yDiff, xDiff); // [-pi; pi]
				auto check = [&](std::size_t i, double lower, double upper) {
					//					s << "% " << vId << "(" << x << ", " << y << "): " << vAdjId << "(" << xAdj << ", " << yAdj << "), "
					//							<< " a=" << angle << ", a d=" << (angle * 180 / pi)
					//							<< " l=" << lower << ", l d=" << (lower * 180 / pi)
					//							<< " u=" << upper << ", u d=" << (upper * 180 / pi) << "\n";
					if(angle >= lower && angle <= upper) {
						blocked |= (1 << i);
						//						s << "%     blocked=" << i << "\n";
					}
				};
				// take the first and last explicitly
				check(0, -pi / 8 - eps, pi / 8 + eps);
				check(15, -2 * pi / 8 - eps, eps);
				// now shift to get contiguous in the rest of the range
				if(angle < 0) angle += 2 * pi; // [0; 2*pi]
				for(std::size_t i = 1; i <= 14; ++i) {
					check(i, (i - 1) * pi / 8 - eps, (i + 1) * pi / 8 + eps);
				}
			}
		}
	} else { // we don't have coordinates, block all
		for(auto &c : auxLabelBlocked) c = ~0L;
	}

	// Tikz code
	// ---------------------------------------------------------------------------
	std::string textModifiersBegin, textModifiersEnd;
	if(options.thick) {
		textModifiersBegin += "\\textbf{";
		textModifiersEnd += "}";
	}
	if(options.withTexttt) {
		textModifiersBegin += "\\texttt{";
		textModifiersEnd += "}";
	}
	s << "\\begin{tikzpicture}[remember picture, scale=\\modGraphScale";
	s << R"XXX(, baseline={([yshift={-0.5ex}]current bounding box)})XXX";
	if(options.thick) s << ", thick";
	s << ", solid"; // circumvent the strange inherited 'dashed' http://tex.stackexchange.com/questions/115887/pattern-in-addplot-inherits-dashed-option-from-previous-draw
	s << "]" << std::endl;
	s << "\\input{\\modInputPrefix/" << fileCoords << "}" << std::endl;

	for(Vertex v : asRange(vertices(g))) {
		unsigned int vId = get(boost::vertex_index_t(), g, v);
		if(!isVisible[vId]) continue;
		unsigned char atomId = depict.getAtomId(v);

		auto createDummy = [&s, vId, &advOptions, &textModifiersBegin, &textModifiersEnd](std::string suffix, bool subscript) {
			// create dummy vertex to make sure the bounding box is large enough
			s << "\\node[modStyleGraphVertex, at=(v-" << (vId + advOptions.idOffset) << suffix << ")";
			if(subscript) s << ", text depth=.25ex";
			s << "] {\\phantom{" << textModifiersBegin << "H";
			if(subscript) s << "$_2$";
			s << textModifiersEnd << "}};\n";
		};

		std::string colourString = advOptions.getColour(v);
		if(options.withColour && colourString.empty()) {
			switch(atomId) {
			case AtomIds::Hydrogen:
				colourString = "gray";
				break;
			case AtomIds::Nitrogen:
				colourString = "blue";
				break;
			case AtomIds::Oxygen:
				colourString = "red";
				break;
			case AtomIds::Phosphorus:
				colourString = "orange";
				break;
			case AtomIds::Sulfur:
				colourString = "olive";
				break;
			}
		}
		if(!colourString.empty()) colourString = ", text=" + colourString;

		std::string indexString;
		if(options.withIndex) {
			indexString = "{\\tiny $\\langle " + boost::lexical_cast<std::string>(get(boost::vertex_index_t(), g, v)) + "\\rangle$}";
		}

		s << "\\node[modStyleGraphVertex";
		s << colourString;
		if(isSimpleCarbon[vId]) {
			if(indexString.empty()) s << ", modStyleCarbon";
			else s << ", modStyleCarbonIndex";
		}
		s << ", at=(v-coord-" << (vId + advOptions.idOffset) << ")] (v-" << (vId + advOptions.idOffset) << ") ";
		if(isSimpleCarbon[vId]) { // Simple Cs
			s << "{" << textModifiersBegin << indexString << textModifiersEnd << "};\n";
			if(!indexString.empty()) createDummy("", false);
			continue;
		}
		char charge = depict.getCharge(v);
		auto auxBlocked = auxLabelBlocked[vId];
		unsigned int hCount = implicitHydrogenCount[vId];
		bool allAuxBlocked = areAllBlocked(auxBlocked);
		bool hInLabel = (!options.collapseHydrogens
				|| allAuxBlocked);
		bool auxRightFree = (auxBlocked & Loc::R_all) == 0;
		bool chargeInAux = auxRightFree && !hInLabel && hCount > 0;

		std::string labelNoAux = escapeForLatex(depict.getVertexLabelNoChargeRadical(v));
		std::string chargeString;
		if(charge != 0) {
			if(options.raiseCharges) chargeString += "$^{";
			if(charge != 1 && charge != -1) chargeString += boost::lexical_cast<std::string>(std::abs(charge));
			if(charge < 0) chargeString += '-';
			else chargeString += '+';
			if(options.raiseCharges) chargeString += "}$";
		}
		std::string hString;
		if(hCount > 0) {
			hString += "H";
			if(hCount > 1) hString += "$_{" + boost::lexical_cast<std::string>(hCount) + "}$";
		}

		// handle H_2 special
		if(hCount == 1 && charge == 0 && atomId == 1) {
			s << "{" << textModifiersBegin;
			s << indexString << "H$_2$";
			s << textModifiersEnd << "};\n";
			createDummy("", true);
			continue;
		}
		// handle water special
		if(hCount == 2 && charge == 0 && atomId == 8) {
			s << "{" << textModifiersBegin;
			s << "H$_2$O" << indexString;
			s << textModifiersEnd << "};\n";
			createDummy("", true);
			continue;
		}

		s << "{" << textModifiersBegin;
		s << labelNoAux;
		if(hInLabel) s << hString;
		s << textModifiersEnd << "};\n";
		if(!chargeInAux && charge != 0) {
			s << "\\node[modStyleGraphVertex" << colourString << ", at=(v-" << (vId + advOptions.idOffset) << ".east), anchor=west] {";
			s << textModifiersBegin;
			s << chargeString;
			s << textModifiersEnd << "};\n";
		}
		if(hInLabel && hCount > 1) createDummy("", true);
		else if(!indexString.empty()) createDummy("", false);
		bool hasAuxHydrogen = !hInLabel && hCount != 0;

		bool isAuxVertical = (auxBlocked & Loc::R_all) != 0 && (auxBlocked & Loc::L_all) != 0;

		decltype(auxBlocked) auxHPosition = -1;
		/**/ if((auxBlocked & Loc::R_all) == 0) auxHPosition = Loc::R_all;
		else if((auxBlocked & Loc::L_all) == 0) auxHPosition = Loc::L_all;
		else if((auxBlocked & Loc::T_all) == 0) auxHPosition = Loc::T_all;
		else if((auxBlocked & Loc::B_all) == 0) auxHPosition = Loc::B_all;
		// if we have something like:
		//              y
		//             /
		//            xH
		// then make it
		//              y
		//             /
		//           Hx
		//
		if(/**/ auxHPosition == Loc::R_all
				&& (auxBlocked & Loc::R_side) != 0
				&& (auxBlocked & Loc::L_side) == 0
				) {
			auxHPosition = Loc::L_all;
		}
		// if we have something like:
		//      Y_
		//        \_H
		//          x
		// then make it
		//      Y_
		//        \_
		//          x
		//          H
		if(/**/ auxHPosition == Loc::T_all
				&& (auxBlocked & Loc::T_side) != 0
				&& (auxBlocked & Loc::B_side) == 0
				) {
			auxHPosition = Loc::B_all;
		}
		if(!hasAuxHydrogen) auxHPosition = -1;
		//		auto printBlocked = [&s](unsigned int b) {
		//			auto at = [&b](int i) {
		//				return bool((b & (1 << i)) != 0);
		//			};
		//			s << "% " << at(6) << " " << at(5) << at(4) << at(3) << " " << at(2) << "\n";
		//			s << "% " << at(7) << "     " << at(1) << "\n";
		//			s << "% " << at(8) << "     " << at(0) << "\n";
		//			s << "% " << at(9) << "     " << at(15) << "\n";
		//			s << "% " << at(10) << " " << at(11) << at(12) << at(13) << " " << at(14) << "\n";
		//		};

		//		printBlocked(auxBlocked);
		//		printBlocked(auxHPosition);
		if(auxHPosition != -1) auxBlocked |= auxHPosition;
		//		printBlocked(auxBlocked);

		if(auxHPosition != -1) {
			s << "\\node[modStyleGraphVertex" << colourString << ", at=(v-" << (vId + advOptions.idOffset) << ".";
			/**/ if(auxHPosition == Loc::R_all) s << "east), anchor=west";
			else if(auxHPosition == Loc::L_all) s << "west), anchor=east";
			else if(auxHPosition == Loc::T_all) s << "north), anchor=south, yshift=1pt";
			else if(auxHPosition == Loc::B_all) s << "south), anchor=north, yshift=-1pt";
			else MOD_ABORT;
			s << "] (v-" << (vId + advOptions.idOffset) << "-aux) {";
			s << textModifiersBegin;
			if(!isAuxVertical) s << hString;
			else s << "H";
			if(chargeInAux) s << chargeString;
			s << textModifiersEnd << "};\n";
			createDummy("-aux", hCount > 1);
			if(isAuxVertical && hCount > 1) {
				s << "\\node[modStyleGraphVertex" << colourString << ", at=(v-" << (vId + advOptions.idOffset) << "-aux.east), anchor=west] {";
				s << textModifiersBegin;
				s << "$_{" << hCount << "}$";
				s << textModifiersEnd << "};\n";
			}
		}
		if(depict.getRadical(v)) {
			std::size_t auxRadicalPosition;
			// first try the x/y axis
			/**/ if((auxBlocked & Loc::R) == 0 && charge == 0) auxRadicalPosition = 0;
			else if((auxBlocked & Loc::T) == 0) auxRadicalPosition = 4;
			else if((auxBlocked & Loc::L) == 0) auxRadicalPosition = 8;
			else if((auxBlocked & Loc::B) == 0) auxRadicalPosition = 12;
			else {
				// If there is a charge, then don't put it on R, R_up, or TR.
				auxRadicalPosition = charge == 0 ? 0 : 3;
				for(; auxRadicalPosition < 16; ++auxRadicalPosition) {
					if((auxBlocked & (1 << auxRadicalPosition)) == 0) break;
				}
			}
			if(auxRadicalPosition == 16) auxRadicalPosition = 12;
			auxBlocked |= (1 << auxRadicalPosition);
			double atAngle = auxRadicalPosition * 22.5;
			s << "\\node[outer sep=0, inner sep=1, minimum size=0, fill=black, circle, "
					<< "at=(v-" << (vId + advOptions.idOffset) << "." << atAngle << "), "
					<< "shift=(" << atAngle << ":3pt)] {};\n";
		}
		if(options.withIndex) {
			std::size_t auxIndexPosition;
			// first try the x/y axis
			/**/ if((auxBlocked & Loc::R) == 0 && charge == 0) auxIndexPosition = 0;
			else if((auxBlocked & Loc::T) == 0) auxIndexPosition = 4;
			else if((auxBlocked & Loc::L) == 0) auxIndexPosition = 8;
			else if((auxBlocked & Loc::B) == 0) auxIndexPosition = 12;
			else {
				// If there is a charge, then don't put it on R, R_up, or TR.
				auxIndexPosition = charge == 0 ? 0 : 3;
				for(; auxIndexPosition < 16; ++auxIndexPosition) {
					if((auxBlocked & (1 << auxIndexPosition)) == 0) break;
				}
			}
			if(auxIndexPosition == 16) auxIndexPosition = 10;
			auxBlocked |= (1 << auxIndexPosition);
			double atAngle = auxIndexPosition * 22.5;
			s << "\\node[outer sep=0, inner sep=1" << colourString
					<< ", at=(v-" << (vId + advOptions.idOffset)
					<< "." << atAngle << "), anchor=" << (atAngle + 180)
					<< "] (v-" << (vId + advOptions.idOffset) << "-auxId) {";
			s << textModifiersBegin;
			s << indexString;
			s << textModifiersEnd << "};\n";
			createDummy("-auxId", true);
		}
	} // foreach vertex

	for(Edge e : asRange(edges(g))) {
		unsigned int fromId = get(boost::vertex_index_t(), g, source(e, g));
		unsigned int toId = get(boost::vertex_index_t(), g, target(e, g));
		if(!isVisible[fromId]) continue;
		if(!isVisible[toId]) continue;
		std::string colourString = advOptions.getColour(e);
		if(!colourString.empty()) colourString += ", text=" + colourString;
		BondType bType = depict.getBondData(e);
		if(!options.edgesAsBonds || bType == BondType::Invalid) s << "\\modDrawGraphEdge";
		else {
			switch(bType) {
			case BondType::Invalid:
				MOD_ABORT;
				break;
			case BondType::Single:
				s << "\\modDrawSingleBond";
				break;
			case BondType::Aromatic:
				s << "\\modDrawAromaticBond";
				break;
			case BondType::Double:
				s << "\\modDrawDoubleBond";
				break;
			case BondType::Triple:
				s << "\\modDrawTripleBond";
				break;
			}
		}
		unsigned int fromOffset = 1, toOffset = 1;
		if(isSimpleCarbon[fromId]) fromOffset = 0;
		if(isSimpleCarbon[toId]) toOffset = 0;
		s << "{" << (fromId + advOptions.idOffset) << "}{" << (toId + advOptions.idOffset) << "}{" << fromOffset << "}{" << toOffset << "}";
		if(!options.edgesAsBonds || bType == BondType::Invalid)
			s << "{" << textModifiersBegin << escapeForLatex(depict.getEdgeLabel(e)) << textModifiersEnd << "}";
		s << "{" << colourString << "}";
		s << "" << std::endl;
	} // foreach edge
	s << "\\end{tikzpicture}" << std::endl;
}

struct DefaultAdvancedOptions {

	template<typename T>
	std::string getColour(T) const {
		return "";
	}

	template<typename T>
	bool isVisible(T) const {
		return true;
	}

	template<typename T>
	bool disallowCollapse(T) const {
		return false;
	}

	unsigned int idOffset = 0;
};

template<typename Graph, typename Depict>
void tikz(std::ostream &s, const Options &options, const Graph &g, const Depict &depict, const std::string & fileCoords) {
	DefaultAdvancedOptions adv;
	tikz(s, options, g, depict, fileCoords, adv);
}

} // namespace Write
} // namespace Graph
} // namespace IO
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_GRAPHWRITEDETAIL_H */

