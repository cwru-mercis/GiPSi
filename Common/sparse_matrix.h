/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Sparse Matrix Operations (sparse_matrix.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	SPARSE_MATRIX.H v0.0.1
////
////	Sparse Matrix template library
////
////////////////////////////////////////////////////////////////


#ifndef _SPARSE_MATRIX_H
#define _SPARSE_MATRIX_H

#include "algebra.h"
#include <cstdlib>
#include <iostream>
#include <sstream>


template<class T> class CRSMatrix;

template<class T> void multMV(Vector<T> &result, CRSMatrix<T> &A, Vector<T> &x);


// General mxn dimensional Compressed Row Storage Sparse Matrix Template
template <class T> 
class CRSMatrix {
public:
// Constructors
	CRSMatrix() :	data(NULL), row_ptr(NULL), col_ind(NULL),
					_m(0), _n(0), _nnz(0) {}
    
	CRSMatrix(	unsigned int m, unsigned int n, unsigned int nnz) {
		this->init(m, n, nnz);
	}
    
	CRSMatrix(	unsigned int m, unsigned int n, unsigned int nnz,
				T *val,	unsigned int *col_ind, unsigned int *row_ptr) {
		this->init(m, n, nnz);

		for(int i = 0; i<nnz; i++) {
			this->data[i]		= val[i];
			this->col_ind[i]	= col_ind[i];
		}

		for(int i = 0; i<m; i++) {
			this->row_ptr[i]	= row_ptr[i];
		}
	}

	CRSMatrix( Matrix<T> &A, T zeroval = (T) 0) {
		unsigned int nonzero = 0;
		int i;

		for(i=0; i<A.m(); i++)
			for(int j=0; j<A.n(); j++) 
				if(!zero(A[i][j], zeroval)) nonzero++;
			
		init(A.m(), A.n() , nonzero);

		unsigned int	idx = 0, row = 0;
		bool			first;

		for(i=0; i<A.m(); i++) {
			first = true;
			for(int j=0; j<A.n(); j++) { 
				if(!zero(A[i][j], zeroval)) {
					data[idx]		= A[i][j];
					col_ind[idx]	= j;
					if(first)	{
						row_ptr[i]	= idx;
						first		= false;
					}
						
					idx++;
				}
			}
		}

	}

	// Copy constructor
	CRSMatrix(const CRSMatrix<T> &A) {
		this->init(A.m(), A.n(), A.nnz());

		for(int i = 0; i<nnz; i++) {
			data[i]		= A.data[i];
			col_ind[i]	= A.col_ind[i];
		}

		for(int i = 0; i<m; i++) {
			row_ptr[i]	= A.row_ptr[i];
		}
	}

// Destructor
	~CRSMatrix<T>() {
		if (data != NULL)		delete[] data;
		if (row_ptr != NULL)	delete[] row_ptr;
		if (col_ind != NULL)	delete[] col_ind;
	}


// Query functions
	unsigned int	m()		const { return _m; }
	unsigned int	n()		const { return _n; }
	unsigned int	nnz()	const { return _nnz; }
	unsigned int	size()	const { return _m * _n; }
	T*				begin() const { return data; }
	T*				end()	const { return data + _nnz; }
	bool			empty() const { return (data == NULL && _nnz == 0); }

	const T& operator() (unsigned int i, unsigned int j) const {
		for(int k = row_ptr[i]; k<row_ptr[i+1]; k++) {
			if(col_ind[k] == j)
				return data[k];
		}

		return (T) 0;
	}

	T&	operator() (unsigned int i, unsigned int j) {
		for(int k = row_ptr[i]; k<row_ptr[i+1]; k++) {
			if(col_ind[k] == j)
				return data[k];
		}

		printf("CSRMatrix access error: element (%d, %d) does not exist!\n", i, j);
		exit(1);

		return data[0];
	}

	int	save(const char *filename);

protected:
	T				*data;		// Component array
	unsigned int	_m, _n;		// Dimensions of the matrix
	unsigned int	_nnz;		// Number of non-zero elements
	unsigned int	*row_ptr;	// Row pointers
	unsigned int	*col_ind;	// Column indices

	void init(unsigned int m, unsigned int n, unsigned int nnz) {
		data	= new T[nnz];
		col_ind	= new unsigned int[nnz];
		row_ptr	= new unsigned int[m+1];

		_m			= m;
		_n			= n;
		_nnz		= nnz;
		row_ptr[m]	= nnz;		// Carrying on with the tradition
	}
	
#if defined(_MSC_VER) && _MSC_VER <= 1200
	friend	void	multMV(Vector<T> &result, CRSMatrix<T> &A, Vector<T> &x);
#else
	friend	void	multMV<>(Vector<T> &result, CRSMatrix<T> &A, Vector<T> &x);
#endif
};	


template<class T>
void multMV(Vector<T> &result, CRSMatrix<T> &A, Vector<T> &x) {
	T sum;

	for(int i=0; i<A.m(); i++) {
		sum = (T) 0;

		for(int j=A.row_ptr[i]; j<A.row_ptr[i+1]; j++) {
			sum += A.data[j] * x[A.col_ind[j]];
		}

		result[i] = sum;
	}
}



inline int CRSMatrix<Real>::save(const char *filename) {
	FILE	*fp;

	// Save position info
	if((fp = fopen(filename, "w")) == NULL) {
		printf("Cannot open output file!\n");
		return false;
	}
	
	for(unsigned int i=0; i<_m; i++) {
		for(unsigned int j=row_ptr[i]; j<row_ptr[i+1]; j++) {
			fprintf(fp, "%d\t%d\t%.14lf\n", i+1, col_ind[j]+1, data[j]);
		}
	}

	fclose(fp);

	return true;
}


#endif
