// SpMatrix2.cpp: implementation of the CSpMatrix2 class.
//
//////////////////////////////////////////////////////////////////////

#include "SparseMatrix.h"
#include "Matlab.h"
#include <assert.h>
#include <math.h>

using namespace MeshLib;

inline bool isZero(double x)
{
	return (fabs(x) < 1e-9);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSparseMatrix::CSparseMatrix(int nRows, int nCols, int nnz /* = 0 */)
{
	m_nRows = nRows;
	m_nCols = nCols;
	assert(nRows > 0);
	assert(nCols > 0);
	m_pEntries = new vector<_Entry>();
	if (nnz > 0)
		m_pEntries->reserve(nnz);

	if (nnz > 1024)
	{
		m_nHashCode = nnz;
	}
	else
	{
		m_nHashCode = (nRows > nCols)? nRows * 6: nCols * 6;
	}

	// prime will be better
	if (m_nHashCode % 2 == 0)
		m_nHashCode++;

	m_bAccessed = new bool[m_nHashCode];
	memset(m_bAccessed, 0, sizeof(bool) * m_nHashCode);
}

CSparseMatrix::~CSparseMatrix()
{
	if (m_pEntries != NULL)
		delete m_pEntries;

	delete [] m_bAccessed;
}

int CSparseMatrix::CalcHashCode(int nRow, int nCol)
{
	return (nRow * 37 + nCol * 103) % m_nHashCode;
}

void CSparseMatrix::AddElement(int nRow, int nCol, double dVal)
{
	_Entry entry;
	assert(nRow >= 0 && nRow < m_nRows);
	assert(nCol >= 0 && nCol < m_nCols);
	bool bFound = false;
	
	int hash = CalcHashCode(nRow, nCol);
	if (m_bAccessed[hash])
	{
		for (int i = 0; i < (int)m_pEntries->size(); i++)
		{
			_Entry& e = m_pEntries->at(i);
			if (e.i == nRow && e.j == nCol)
			{
				e.val += dVal;
				if (isZero(e.val))
				{
					if (m_pEntries->size() > 0)	// Copy last to current
					{
						m_pEntries->at(i) = m_pEntries->at(m_pEntries->size() - 1);
					}
					m_pEntries->resize(m_pEntries->size() - 1);
				}
				bFound = true;
				break;
			}
		}
	}
	if (!bFound)
	{
		m_bAccessed[hash] = true;
		entry.i = nRow;
		entry.j = nCol;
		entry.val = dVal;
		m_pEntries->push_back(entry);
	}
}

// No Checking
void CSparseMatrix::AddElementTail(int nRow, int nCol, double dVal)
{
	_Entry entry;
	assert(nRow >= 0 && nRow < m_nRows);
	assert(nCol >= 0 && nCol < m_nCols);

	entry.i = nRow;
	entry.j = nCol;
	entry.val = dVal;
	m_bAccessed[CalcHashCode(nRow, nCol)] = true;

	m_pEntries->push_back(entry);
//	printf("(%d,%d) %f\n", nRow, nCol, dVal);
}

bool CSparseMatrix::CGSolver(double b[], double x[], double eps, int& itrs )
{
	if( GetRows()!=GetCols() )
		return false;

	int size = this->GetRows();
	// initialize
	double alpha, beta;
	double* r = new double[size];
	double* p = new double[size];
	double* temp = new double[size];
	double* multi = new double[size];
	assert( r != NULL && p != NULL && temp != NULL && multi != NULL );

	// x_0 = b_0
	for( int i=0; i<size; i++ )
		x[i] = b[i];

	// r_0 = b - A*x_0
    Multiply( x, multi );
	
	//spM->TimesVDirect( x, size, multi, size );

	for( int i=0; i<size; i++ )
		r[i] = b[i] - multi[i];

	//for( int i=0; i<size; i++ )
	//x[i] = multi[i];
	//  p_1 = r_0
	for( int i=0; i<size; i++ )
		p[i] = r[i];

	double numerator, denominator;

	int step = 0;

	double norm_b = 0.0;
	for( int i=0; i<size; i++ )
		norm_b += b[i]*b[i];
	norm_b = sqrt( norm_b );

	double norm_r = 0.0;

	while( true )
	{
		if( itrs>=0 )
			if( step>=itrs )
				break;
		// ||r||/||b|| < threshold, then break
		norm_r = 0.0;
		for( int i=0; i<size; i++ )
			norm_r += r[i]*r[i];
		norm_r = sqrt( norm_r );

		if( norm_r/norm_b<eps )
			break;

		// \alpha_k = r_{k-1}^T*r_{k-1}/p_k^T(A*p_k) 
		numerator = 0.0;
		for( int i=0; i<size; i++ )
			numerator += r[i]*r[i];


		//spM->TimesVDirect( p, size, multi, size );
		this->Multiply( p, multi );

		denominator = 0.0;
		for( int i=0; i<size; i++ )
			denominator += p[i]*multi[i];

		alpha = numerator/denominator;

		// x_k = x_{k-1} + \alpha_k * p_k
		for( int i=0; i<size; i++ )
			x[i] = x[i] + alpha*p[i];

		for( int i=0; i<size; i++ )
			temp[i] = r[i];		// temp ---- r_{k-1}

		// r_k = r_{k-1} - \alpha_k*(A*p_k)
		for( int i=0; i<size; i++ )
			r[i] = temp[i] - alpha*multi[i];

		// \beta = r_k^T*r_k/(r_{k-1}^T*r_{k-1})
		numerator = 0.0;
		for( int i=0; i<size; i++ )
			numerator += r[i]*r[i];
		denominator = 0.0;
		for( int i=0; i<size; i++ )
			denominator += temp[i]*temp[i];
		beta = numerator/denominator;

		// p_{k+1} = r_k + \beta*p_k
		for( int i=0; i<size; i++ )
			p[i] = r[i]+beta*p[i];

		step++;
	}

	//spM->TimesVDirect( x, size, multi, size );
	this->Multiply( x, multi );

	double sum = 0.0;
	for( int i=0; i<size; i++ )
		sum += (multi[i]-b[i])*(multi[i]-b[i]);
	sum = sqrt( sum );

	
	printf( "CG iter_num: %d error: %.10f\n", step, norm_r/norm_b );

	delete []r;
	delete []p;
	delete []multi;
	delete []temp;

	return true;
}



void CSparseMatrix::Multiply(double iVector[], double oVector[])
{
	memset(oVector, 0, sizeof(double) * 	m_nRows);
	for (int i = 0; i < (int)m_pEntries->size(); i++)
	{
		_Entry& entry = m_pEntries->at(i);
		oVector[entry.i] += iVector[entry.j] * entry.val;
	}
}

void CSparseMatrix::TransMul(double iVector[], double oVector[])
{
	memset(oVector, 0, sizeof(double) * m_nCols);
	for (int i = 0; i < (int)m_pEntries->size(); i++)
	{
		_Entry& entry = m_pEntries->at(i);
		oVector[entry.j] += iVector[entry.i] * entry.val;
	}
}

bool CSparseMatrix::CGSolver2Matlab(double b[], double x[], double eps, int& itrs)
{
#ifndef NO_MATLAB
	InitMatlabEngine();
	Engine* ep = GetEngine();
	if (ep == NULL)
		return (false);

	mxArray* bb = NULL;
	bb = mxCreateDoubleMatrix(m_nRows, 1, mxREAL);
	
	double* bbr = mxGetPr(bb);
	for (int i = 0; i < m_nRows; i++)
		bbr[i] = b[i];
	
	engPutVariable(ep, "b", bb);	
	mxDestroyArray(bb);
	
	mxArray* AA = NULL;
	// statistic total non-zeros
	int tnz = (int) m_pEntries->size();
	// Count nnz in each column
	int* pNumInCol = new int[m_nCols];
	memset(pNumInCol, 0, sizeof(int) * m_nCols);
	for (int i = 0; i < (int)m_pEntries->size(); i++)
	{
		pNumInCol[m_pEntries->at(i).j]++;
	}

	AA = mxCreateSparse(m_nRows, m_nCols, tnz, mxREAL);

	double* AAr = mxGetPr(AA);
	
	mwIndex* ir = (mwIndex*)mxGetIr(AA);
	mwIndex* jc = (mwIndex*)mxGetJc(AA);

	// update jc
	jc[0] = 0;
	for (int i = 1; i <= m_nCols; i++)
	{
		jc[i] = jc[i - 1] + pNumInCol[i - 1];
	}
	
	// fill data
	// pNumInCol now represents # of elements currently filled
	memset(pNumInCol, 0, sizeof(int) * m_nCols);
	for (int i = 0; i < (int)m_pEntries->size(); i++)
	{
		_Entry & e = m_pEntries->at(i);
		int t = (int)( pNumInCol[e.j] + jc[e.j] );
		AAr[t] = e.val;
		ir[t] = e.i;
		pNumInCol[e.j]++;
	}

	delete [] pNumInCol;

 	engPutVariable(ep, "A", AA);
	mxDestroyArray(AA);
	
//	engEvalString(ep, "x = A\\b;");
	engEvalString(ep, "x = (A'*A)\\(A'*b);");
	mxArray* xx = NULL;
	xx = engGetVariable(ep, "x");
	assert(xx != NULL);
	double* xxr = mxGetPr(xx);
	memcpy(x, xxr, sizeof(double) * m_nCols);
	
	mxDestroyArray(xx);
//	CloseMatlabEngine();
#else
	assert(false);
#endif
	return (true);
}

bool CSparseMatrix::CGSolverStable(double b[], double x[], double eps, int& itrs )
{
	if( GetRows()!=GetCols() )
		return false;

	int size = this->GetRows();
	// initialize
	double alpha, beta;
	double  sum;
	double* r = new double[size];
	double* p = new double[size];
	double* multi = new double[size];
	double* temp = new double[size];

	// x_0 = b_0
	for( int i=0; i<size; i++ )
		x[i] = b[i];

	// r_0 = b - A*x_0
	sum = 0.0;
	for( int i=0; i<size; i++)
		sum += x[i];
	for( int i=0; i<size; i++ )
	{
		temp[i] = -(sum-x[i]);
	}
	//spM->TimesVDirect( temp, size, multi, size );
	this->Multiply( temp, multi ); 


	sum = 0.0;
	for( int i=0; i<size; i++)
		sum += multi[i];
	for( int i=0; i<size; i++ )
	{
		temp[i] = -(sum-multi[i]);
	}

	for( int i=0; i<size; i++ )
		r[i] = b[i] - temp[i];

	//  p_1 = r_0
	for( int i=0; i<size; i++ )
		p[i] = r[i];

	double numerator, denominator;

	int step = 0;

	double norm_b = 0.0;
	for( int i=0; i<size; i++ )
		norm_b += multi[i]*multi[i];
	norm_b = sqrt( norm_b );

	double norm_r = 0.0;

	while( true )
	{
		if( itrs>=0 )
			if( step>=itrs )
				break;
		// ||r||/||b|| < threshold, then break
		norm_r = 0.0;
		for( int i=0; i<size; i++ )
			norm_r += r[i]*r[i];
		norm_r = sqrt( norm_r );

		if( norm_r/norm_b<eps )
			break;

		//fprintf( stderr, "%d - norm_r/norm_b = %.10f\n", step, norm_r/norm_b );

		// \alpha_k = r_{k-1}^T*r_{k-1}/p_k^T(A*p_k) 

		numerator = 0.0;
		for( int i=0; i<size; i++ )
			numerator += r[i]*r[i];

		sum = 0.0;
		for( int i=0; i<size; i++ )
			sum += p[i];
		for( int i=0; i<size; i++ )
		{
			temp[i] = -(sum-p[i]);
		}
		//spM->TimesVDirect( temp, size, multi, size );
		this->Multiply( temp, multi );
		sum = 0.0;
		for( int i=0; i<size; i++ )
			sum += multi[i];
		for( int i=0; i<size; i++ )
		{
			temp[i] = -(sum-multi[i]);
		}		// temp --- A*p_k

		denominator = 0.0;
		for( int i=0; i<size; i++ )
			denominator += p[i]*temp[i];

		alpha = numerator/denominator;

		// x_k = x_{k-1} + \alpha_k * p_k
		for( int i=0; i<size; i++ )
			x[i] = x[i] + alpha*p[i];

		for( int i=0; i<size; i++ )
			multi[i] = r[i];		// multi ---- r_{k-1}

		// r_k = r_{k-1} - \alpha_k*(A*p_k)
		for( int i=0; i<size; i++ )
			r[i] = multi[i] - alpha*temp[i];

		// \beta = r_k^T*r_k/(r_{k-1}^T*r_{k-1})
		numerator = 0.0;
		for( int i=0; i<size; i++ )
			numerator += r[i]*r[i];
		denominator = 0.0;
		for( int i=0; i<size; i++ )
			denominator += multi[i]*multi[i];
		beta = numerator/denominator;

		// p_{k+1} = r_k + \beta*p_k
		for( int i=0; i<size; i++ )
			p[i] = r[i]+beta*p[i];

		step++;
	}


	//printf( "CG iter_num: %d error: %.10f\n", step, norm_r/norm_b );

	delete []r;
	delete []p;
	delete []temp;
	delete []multi;

	return true;
}

