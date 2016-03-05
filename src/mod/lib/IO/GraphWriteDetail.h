#ifndef MOD_LIB_IO_GRAPHWRITEDETAIL_H
#define	MOD_LIB_IO_GRAPHWRITEDETAIL_H

#include <mod/Error.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/IO/Graph.h>
#include <mod/lib/IO/IO.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace IO {
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
				isSimpleCarbon[get(boost::vertex_index_t(), g, v)] = depict.getCharge(v) == 0;
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
	// bit 0: right
	// bit 1: left
	// bit 2: up
	// bit 3: down
	std::vector<unsigned char> auxLabelBlocked(num_vertices(g), 0);
	if(depict.getHasCoordinates()) { // stuff where we absolutely need coordinates right now
		// see where we can have aux data
		for(Vertex v : asRange(vertices(g))) {
			unsigned int vId = get(boost::vertex_index_t(), g, v);
			if(!isVisible[vId]) continue;
			double x = depict.getX(v, !options.collapseHydrogens);
			double y = depict.getY(v, !options.collapseHydrogens);
			for(Vertex vAdj : asRange(adjacent_vertices(v, g))) {
				unsigned int vAdjId = get(boost::vertex_index_t(), g, vAdj);
				if(!isVisible[vAdjId]) continue;
				double xAdj = depict.getX(vAdj, !options.collapseHydrogens);
				double yAdj = depict.getY(vAdj, !options.collapseHydrogens);
				unsigned char &blocked = auxLabelBlocked[vId];
				double eps = 0.00001;
				if(xAdj > x + eps) blocked |= 1; // not right
				else if(xAdj < x - eps) blocked |= 2; // not left
				if(yAdj > y + eps) blocked |= 4; // not up
				else if(yAdj < y - eps) blocked |= 8; // not down
			}
		}
	} else { // we don't have coordinates, block all
		for(unsigned char &c : auxLabelBlocked) c = ~0;
	}

	{ // tikz code
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
			unsigned char auxBlocked = auxLabelBlocked[vId];
			unsigned int hCount = implicitHydrogenCount[vId];
			bool allAuxBlocked = (auxBlocked & (1 + 2 + 4 + 8)) == (1 + 2 + 4 + 8);
			bool hInLabel = (!options.collapseHydrogens
							|| allAuxBlocked);
			bool auxRightFree = (auxBlocked & 1) == 0;
			bool chargeInAux = auxRightFree && !hInLabel && hCount > 0;

			std::string labelNoAux = escapeForLatex(depict.getVertexLabelNoCharge(v));
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

			bool isAuxVertical = (auxBlocked & (1 + 2)) == (1 + 2);

			auxBlocked &= (1 + 2 + 4 + 8); // make sure only the 4 LSB are set

			assert(auxBlocked >> 4 == 0);

			unsigned char auxHPosition = 0;
			if((auxBlocked & 1) == 0) auxHPosition = 1;
			else if((auxBlocked & 2) == 0) auxHPosition = 2;
			else if((auxBlocked & 4) == 0) auxHPosition = 4;
			else if((auxBlocked & 8) == 0) auxHPosition = 8;
			if(!hasAuxHydrogen) auxHPosition = 0;

			assert(auxHPosition >> 4 == 0);

			unsigned char auxIndexPosition = auxBlocked | auxHPosition;
			if((auxIndexPosition & 2) == 0) auxIndexPosition = 2;
			else if((auxIndexPosition & 1) == 0) auxIndexPosition = 1;
			else if((auxIndexPosition & 8) == 0) auxIndexPosition = 8;
			else if((auxIndexPosition & 4) == 0) auxIndexPosition = 4;
			else auxIndexPosition = 0;
			assert(auxIndexPosition >> 4 == 0);

			if(auxHPosition != 0) {
				s << "\\node[modStyleGraphVertex" << colourString << ", at=(v-" << (vId + advOptions.idOffset) << ".";
				if(auxHPosition == 1) s << "east), anchor=west";
				else if(auxHPosition == 2) s << "west), anchor=east";
				else if(auxHPosition == 4) s << "north), anchor=south, yshift=1pt";
				else if(auxHPosition == 8) s << "south), anchor=north, yshift=-1pt";
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
			if(options.withIndex) {
				s << "\\node[outer sep=0, inner sep=1" << colourString << ", at=(v-" << (vId + advOptions.idOffset) << ".";
				if(auxIndexPosition == 1) s << "east), anchor=west";
				else if(auxIndexPosition == 2) s << "west), anchor=east";
				else if(auxIndexPosition == 4) s << "north), anchor=south";
				else if(auxIndexPosition == 8) s << "south), anchor=north";
				else {
					s << ".-135), anchor=45";
				}
				s << "] (v-" << (vId + advOptions.idOffset) << "-auxId) {";
				s << textModifiersBegin;
				s << indexString;
				s << textModifiersEnd << "};\n";
				createDummy("-auxId", true);
			}
		}

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
		}
		s << "\\end{tikzpicture}" << std::endl;
	}
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

#endif	/* MOD_LIB_IO_GRAPHWRITEDETAIL_H */

