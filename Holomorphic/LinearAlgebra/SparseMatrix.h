// SparseMatrix.h: interface for the CSparseMatrix class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SPARSEMATRIX_H_
#define _SPARSEMATRIX_H_

#include <vector>
using std::vector;

namespace MeshLib{

struct _Entry
{
	int i;
	int j;
	double val;
};

//sparse matrix 

class CSparseMatrix  
{
protected:
	int m_nRows;
	int m_nCols;

	bool* m_bAccessed;
	int	  m_nHashCode;

	vector<_Entry>* m_pEntries;	
	int CalcHashCode(int nRow, int nCol);

public:
	CSparseMatrix(int nRows, int nCols, int nnz = 0);	// nnz is only used as a hint
	void AddElement(int nRow, int nCol, double dVal);	// throws TOO_MANY_ELEMENTS
	// a faster call, no check for duplication index
	void AddElementTail(int nRow, int nCol, double dVal);
		
	// add by Mac for CG solver
	bool CGSolver( double b[], double x[], double eps, int& itrs );
	bool CGSolverStable( double b[], double x[], double eps, int& itrs );
	bool CGSolver2Matlab(double b[], double x[], double eps, int& itrs);

	void Multiply(double iVector[], double oVector[]);
	void TransMul(double iVector[], double oVector[]);

	int GetCols()
	{
		return m_nCols;
	}
	int GetRows()
	{
		return m_nRows;
	}

	virtual ~CSparseMatrix();
};

}

#endif 