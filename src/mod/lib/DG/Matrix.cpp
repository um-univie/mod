#include "Matrix.h"

#include <mod/Graph.h>
#include <mod/lib/Chem/SBML.h>
#include <mod/lib/DG/Hyper.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Rule/Base.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <sstream>

namespace mod {
namespace lib {
namespace DG {

Matrix::Matrix() { }

Matrix::Matrix(const Hyper &hyper) {
	const HyperGraphType &dgHyper = hyper.getGraph();
	using Vertex = HyperVertex;
	using Edge = HyperEdge;

	// translate all the vertices from the hyper graph
	std::map<int, int> vertexToRow;

	for(Vertex v : asRange(vertices(dgHyper))) {
		if(dgHyper[v].kind != HyperVertexKind::Vertex) continue;
		int id = get(boost::vertex_index_t(), dgHyper, v);
		const lib::Graph::Single *g = dgHyper[v].graph;
		// see if it's an input graph (we don't do that any more)
		//		for(const std::shared_ptr<mod::Graph> input : hyper.getNonHyper().getInputs()) {
		//			if(&input->getGraph() == g) {
		//				inputIds.push_back(rowNames.size());
		//				break;
		//			}
		//		}
		vertexToRow.insert(std::make_pair(id, rowNames.size()));
		rowNames.push_back(g->getName());
	}

	// translate all the edges from the hyper graph

	for(Vertex v : asRange(vertices(dgHyper))) {
		if(dgHyper[v].kind != HyperVertexKind::Edge) continue;
		assert(dgHyper[v].rules.size() == 1);
		const Rule::Base *r = dgHyper[v].rules.front();
		std::stringstream name;
		name << "r_" << r->getId();
		unsigned int col = addCol(name.str());
		// subtract those we use

		for(Edge e : asRange(in_edges(v, dgHyper))) {
			Vertex vUsed = source(e, dgHyper);
			int idUsed = get(boost::vertex_index_t(), dgHyper, vUsed);
			int row = vertexToRow[idUsed];
			addCoef(col, row, -1);
		}

		// add those we produce

		for(Edge e : asRange(out_edges(v, dgHyper))) {
			Vertex vProd = target(e, dgHyper);
			int idProd = get(boost::vertex_index_t(), dgHyper, vProd);
			int row = vertexToRow[idProd];
			addCoef(col, row, 1);
		}
	}
}

Matrix::Matrix(std::istream& s) {
	unsigned int numRows, numInputs, numColNames, numCols;
	s >> numRows >> numInputs >> numColNames >> numCols;
	assert(s.good());
	for(unsigned int i = 0; i < numRows; i++) {
		std::string rowName;
		s >> rowName;
		assert(s.good());
		rowNames.push_back(rowName);
	}
	for(unsigned int i = 0; i < numInputs; i++) {
		unsigned int inputId;
		s >> inputId;
		assert(s.good());
		if(inputId >= rowNames.size()) {
			IO::log() << "Input row id out of bounds " << inputId << std::endl
					<< "\tshould be below " << rowNames.size() << std::endl;
			exit(1);
		}
		inputIds.push_back(inputId);
	}
	for(unsigned int i = 0; i < numColNames; i++) {
		std::string colName;
		s >> colName;
		assert(s.good());
		colNames.push_back(colName);
	}

	for(unsigned int i = 0; i < numCols; i++) {
		unsigned int nameId;
		s >> nameId;
		assert(s.good());
		if(nameId >= colNames.size()) {
			IO::log() << "Column name id out of bounds " << nameId << std::endl
					<< "\tshould be below " << colNames.size() << std::endl;
			exit(1);
		}
		addCol(colNames[nameId]);
	}

	while(true) {
		unsigned int col, numCoefs;
		s >> col >> numCoefs;
		if(!s.good()) break;
		if(col >= cols.size()) {
			IO::log() << "Column index out of bounds: " << col << std::endl
					<< "\tshould be below " << cols.size() << std::endl;
			exit(1);
		}
		for(unsigned int i = 0; i < numCoefs; i++) {
			unsigned int row;
			int coef;
			s >> row >> coef;
			assert(s.good());
			if(row >= rowNames.size()) {
				IO::log() << "Row index out of bounds: " << col << std::endl
						<< "\tshould be below " << rowNames.size() << std::endl;
				exit(1);
			}
			addCoef(col, row, coef);
		}
	}
}

Matrix::~Matrix() { }

unsigned int Matrix::addRow(const std::string &name) {
	rowNames.push_back(name);
	return rowNames.size() - 1;
}

unsigned int Matrix::addCol(const std::string& name) {
	cols.push_back((Col()));
	Col &col = cols.back();
	unsigned int nameId;
	for(nameId = 0; nameId < colNames.size(); nameId++) {
		if(colNames[nameId].compare(name) == 0) break;
	}
	if(nameId == colNames.size()) colNames.push_back(name);
	else IO::log() << "nameId=" << nameId << "\tname=" << colNames[nameId] << "\treused" << std::endl;
	col.nameId = nameId;
	return cols.size() - 1;
}

void Matrix::addCoef(unsigned int col, unsigned int row, int coef) {
	assert(col < cols.size());
	unsigned int i;
	for(i = 0; i < cols[col].coefs.size(); i++) {
		if(cols[col].coefs[i].first == row) break;
	}
	if(i == cols[col].coefs.size()) {
		cols[col].coefs.push_back(std::make_pair(row, coef));
		return;
	}
	cols[col].coefs[i].second += coef;
}

const std::vector<std::string> &Matrix::getRowNames() const {
	return rowNames;
}

std::vector<unsigned int> &Matrix::getInputIds() {
	return inputIds;
}

const std::vector<unsigned int> &Matrix::getInputIds() const {
	return inputIds;
}

std::vector<std::string> &Matrix::getColNames() {
	return colNames;
}

const std::vector<std::string> &Matrix::getColNames() const {
	return colNames;
}

std::vector<Matrix::Col> &Matrix::getCols() {
	return cols;
}

const std::vector<Matrix::Col> &Matrix::getCols() const {
	return cols;
}

const std::string &Matrix::getColName(unsigned int col) const {
	assert(col < cols.size());
	assert(cols[col].nameId < colNames.size());
	return colNames[cols[col].nameId];
}

void Matrix::print(std::ostream& s) const {
	s << rowNames.size() << "\t" << inputIds.size() << "\t"
			<< colNames.size() << "\t" << cols.size() << std::endl;
	for(const std::string &str : rowNames) s << str << " ";
	s << std::endl;
	for(unsigned int inputId : inputIds) s << inputId << " ";
	s << std::endl;
	for(const std::string &str : colNames) s << str << " ";
	s << std::endl;
	for(const Col &col : getCols()) s << col.nameId << " ";
	s << std::endl;

	for(unsigned int i = 0; i < cols.size(); i++) {
		s << i << "\t";
		cols[i].print(s);
	}
}

void Matrix::print() const {
	IO::FileHandle s(IO::getUniqueFilePrefix() + "dgMatrix.sparseStoi");
	print(s);
}

//--------------------------------------------------------------------------
// Col
//--------------------------------------------------------------------------

Matrix::Col::Col() { }

Matrix::Col::~Col() { }

unsigned int Matrix::Col::getNameId() const {
	return nameId;
}

const std::vector<std::pair<unsigned int, int> > &Matrix::Col::getCoefs() const {
	return coefs;
}

void Matrix::Col::print(std::ostream& s) const {
	s << coefs.size() << "\t";
	typedef std::pair<unsigned int, int> P;
	for(const P &p : coefs) s << p.first << " " << p.second << "\t";
	s << std::endl;
}

} // namespace DG
} // namespace lib
} // namespace mod