/* PC1D Semiconductor Device Simulator
Copyright (C) 2003 University of New South Wales
Authors: Paul A. Basore, Donald A. Clugston

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
///////////////////////////////////////////////////////////////////////////
// CMatrix
// Band diagonal matrix solver (bandwidth 11).
//

const int MAX_SPARSE_ELEMENTS=30;

/*********
//------------------------------------------------------------------------
// CVector, CSquareMatrix : Simple vector and matrix classes
//

class CVector {
    friend class CSquareMatrix;
public:
    CVector();
    CVector(int numberofelements);
    ~CVector();
    double & operator [](int d) { return data[d]; };
    void SetDimension(int numberofelements);
    void MakeZero();
private:
    int n;      // the vector dimension
    double *data;
};

const int MAX_SQUAREMATRIX_SIZE=MAX_SPARSE_ELEMENTS;

// a normal (non-sparse) square matrix
class CSquareMatrix {
public:
	CSquareMatrix(int matrixsize);
    ~CSquareMatrix();
    CSquareMatrix & operator =(unsigned int num);
    static void mult(CSquareMatrix & mat1, CVector &vect, CVector &resultvect);
    double * operator [](int row) { return m[row]; };
    void LUDecompose();
    void LUSolve(CVector &vect);

// Implementation
private:
    static void ludecom(double **a, int n, int *indx);
    static void lusolve(double **a, int n, int *indx, double b[]);

private:
	int n; // the matrix dimension
    double *m[MAX_SQUAREMATRIX_SIZE]; // the matrix data
    int *indx;  // the row permutation effected by the partial pivoting
};
********/

//------------------------------------------------------------------------
// CMatrix : Band diagonal matrix of bandwidth 11
//


const int bw_1 = 10;	// bandwidth minus 1, last column index
const int diag = 5;		// column index for diagonal matrix element

typedef double matrix_row[bw_1+1]; 


class CMatrix
{
// Types
	typedef double row[bw_1+1]; 
	typedef row* m_ptr;		// Points to 2D matrix
	typedef double* v_ptr;	// Points to 1D vector
// Attributes
public:
	row *m;	// 2D matrix MAXELEMENTS * bw in size.
	double *v;		// 1D vector MAXELEMENTS long.
private:	
	double *allocatedv;		// actual memory adr which was allocated for v.
	int m_matrixsize;
	int m_requestedbandwidth; // 3 for equilibrium solve, 11 for steady-state
// Operations
public:
	CMatrix();
	~CMatrix();
	CMatrix & operator =(CMatrix &m);
// Implementation
public:
	void SetBandMatrixSize(int matrix_size, int bandwidth_required) { m_matrixsize=matrix_size; m_requestedbandwidth=bandwidth_required; };
	int GetBandwidth() { return m_requestedbandwidth; };
	int GetMatrixSize() { return m_matrixsize; };
	void LU_decompose();	// Factors the matrix into its lower and upper triangles
	void LU_solve();	// Solves the two triangular linear systems using 
							//  a front solve and then a back solve.
/***
	void lu_decom(int n_1);	// Factors the matrix into its lower and upper triangles
	void lu_solve(int n_1);	// Solves the two triangular linear systems using 
							//  a front solve and then a back solve.

    void CorrectForSparseElements(int n_1); // Updates the solve value to include
                            // sparse elements (elements which aren't on the band diagonal).
    void ClearSparseElements() { m_NumberOfSparseElements=0; };
    void Add3SparseElements(int row, int colm, double val1, double val2, double val3);
                            // put extra values at [row][col], [r+1][c], [r+2][c]
	void IterativelyImproveSolution(row *originalmatrix, int n_1, double *originalvector);
private:
    int m_NumberOfSparseElements;
    int m_sparserow[MAX_SPARSE_ELEMENTS];
    int m_sparsecol[MAX_SPARSE_ELEMENTS];
    double m_sparseval[MAX_SPARSE_ELEMENTS][3];
****/
};

