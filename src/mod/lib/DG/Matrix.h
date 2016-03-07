#ifndef MOD_LIB_DG_MATRIX_H
#define	MOD_LIB_DG_MATRIX_H

#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

namespace mod {
namespace lib {
namespace DG {
class Hyper;

class Matrix {
public:

	class Col {
	public:
		Col();
		~Col();
		unsigned int getNameId() const;
		const std::vector<std::pair<unsigned int, int> > &getCoefs() const;
		void print(std::ostream &s) const;
	private:
		friend class Matrix;
		unsigned int nameId;
		std::vector<std::pair<unsigned int, int> > coefs;
	};
public:
	Matrix(); // empty
	Matrix(const Hyper &hyper); // from DG
	Matrix(std::istream &s); // from sparse matrix
	virtual ~Matrix();
	unsigned int addRow(const std::string &name);
	unsigned int addCol(const std::string &name);
	void addCoef(unsigned int col, unsigned int row, int coef);
	const std::vector<std::string> &getRowNames() const;
	std::vector<unsigned int> &getInputIds();
	const std::vector<unsigned int> &getInputIds() const;
	std::vector<std::string> &getColNames();
	const std::vector<std::string> &getColNames() const;
	std::vector<Col> &getCols();
	const std::vector<Col> &getCols() const;
	const std::string &getColName(unsigned int col) const;
	void print(std::ostream &s) const;
	void print() const;
private:
	std::vector<std::string> rowNames;
	std::vector<unsigned int> inputIds;
	std::vector<std::string> colNames;
	std::vector<Col> cols;
};

} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_MATRIX_H */
