/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Matrix Class Implementation (matrix.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	MATRIX.CPP v0.1.0
////
////	CBLAS and LAPACK wrapper for Matrix. Implements BLAS Levels 2 and 3, and LAPACK.
////
////////////////////////////////////////////////////////////////


#include "algebra.h"
#define BLOCKSIZE 32


template<>
int Matrix<Real>::load(const char *filename) {
	FILE	*fp;

	if((fp = fopen(filename, "r")) == NULL) {
		printf("Cannot open input file!\n");
		return false;
	}
	
	for(unsigned int i=0; i<_m; i++) {
		for(unsigned int j=0; j<_n; j++) {
			fscanf(fp, "%lf\t", &(*this)[i][j]);
		}
	}

	fclose(fp);

	return true;
}


template<>
int Matrix<Real>::save(const char *filename) {
	FILE	*fp;

	if((fp = fopen(filename, "w")) == NULL) {
		printf("Cannot open output file!\n");
		return false;
	}
	
	for(unsigned int i=0; i<_m; i++) {
		for(unsigned int j=0; j<_n; j++) {
			fprintf(fp, "%.14lf\t", (*this)[i][j]);
		}
		fprintf(fp, "\n");
	}

	fclose(fp);

	return true;
}

//#ifdef ALGEBRA_USE_CBLAS
// NOTE: Right now only a few LAPACK routines are implemented

// The Intel MKL seems to not use the same function names as the actual CLAPACK
// library, so need to sneak in the MKL names instead.
#ifdef ALGEBRA_USE_MKL
#define USE_MATH_LIBS
#define dgesdd_ dgesdd
#define dgetrf_ dgetrf
#define dgetri_ dgetri
#define integer int
#define doublereal double
#endif

#ifdef ALGEBRA_USE_ATLAS
#ifdef ALGEBRA_USE_CLAPACK
#define USE_MATH_LIBS
#endif
#endif


#ifdef USE_MATH_LIBS

template<>
void multMV(Vector<double> &result, const Matrix<double> &a, const Vector<double> &v)
{
	unsigned int	m, n;
	const double	alpha = 1.0, beta = 0.0;

	m = a.m();
	n = a.n();

	ASSERT( (v.dim() == n) && (result.dim() == m) );

	cblas_dgemv(CblasRowMajor, CblasNoTrans,  m, n, 
				alpha, a.begin(), n, v.begin(), 1,
				beta,  result.begin(), 1);
	
}


template<>
void multMM(Matrix<double> &result, const Matrix<double> &a1, const Matrix<double> &a2)
{
	unsigned int			m, n, k;
	const double			alpha = 1.0, beta = 0.0;

	m = result.m();
	n = result.n();
	k = a1.n();


	ASSERT((k == a2.m()) && (m == a1.m() && n == a2.n()));
	
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, 
				alpha, a1.begin(), k, a2.begin(), n,
				beta,  result.begin(), n);
				
}


template<>
int invM(Matrix<double> &result, const Matrix<double> &M) {
	doublereal		*work = new doublereal[M.m()*BLOCKSIZE];
	integer			lwork = M.m()*BLOCKSIZE;
	integer			n, lda;
	integer			*ipiv = new integer[M.m()]; 
	integer			info;

//  the ALGEBRA_USE_MKL define's make the above code appear as:
/*	double		*work = new double[M.m()*BLOCKSIZE];
	int			lwork = M.m()*BLOCKSIZE;
	int			n, lda;
	int			*ipiv = new int[M.m()]; 
	int			info;		*/

	ASSERT(M.m() == M.n());

	if(result.begin() != M.begin()) result = M;

	n	= result.n();
	lda = n;

	dgetrf_(&n, &n, result.begin(), &lda,  &ipiv[0], &info);
	dgetri_(&n, result.begin(), &lda, &ipiv[0], &work[0], &lwork, &info);
	// former MKL function calls:
	//DGETRF(&n, &n, result.begin(), &lda,  &ipiv[0], &info);
	//DGETRI(&n, result.begin(), &lda, &ipiv[0], &work[0], &lwork, &info);

	delete[] work;
	delete[] ipiv;

	return info;
}

// NOTE: The pseudo inverse is not tested with non-square matrices!!
template<>
int pinvM(Matrix<double> &result, const Matrix<double> &M) {
	
	integer			lwork	= M.m()*M.m()*BLOCKSIZE;
	doublereal		*work	= new doublereal[lwork];
	integer			*iwork	= new integer[8*M.m()];
	integer			lda, ldu, ldvt;
	integer			info;
	integer			m		= M.m();
	integer			n		= M.n();
	char		jobu	= 'A';
	char		jobvt	= 'A';
	int			lds		= MAX(m, n);
	doublereal		*S		= new doublereal[MAX(M.m(), M.n())];
	Matrix<doublereal>	U(m, m, 0.0);
	Matrix<doublereal>	VT(n, n, 0.0);
	Matrix<double>	PS(lds, lds, 0.0);
	Matrix<double>	temp(m, n, 0.0);
	
//  the ALGEBRA_USE_MKL define's make the above code appear as:
/*	int			lwork	= M.m()*M.m()*BLOCKSIZE;
	double		*work	= new double[lwork];
	int			*iwork	= new int[8*M.m()];
	int			lda, ldu, ldvt;
	int			info;
	int			m		= M.m();
	int			n		= M.n();
	char		jobu	= 'A';
	char		jobvt	= 'A';
	int			lds		= MAX(m, n);
	double		*S		= new double[MAX(M.m(), M.n())];
	Matrix<double>	U(m, m, 0.0);
	Matrix<double>	VT(n, n, 0.0);
	Matrix<double>	PS(lds, lds, 0.0);
	Matrix<double>	temp(m, n, 0.0);	*/

	if(result.begin() != M.begin()) result = M;

	lda		= m;
	ldu		= m;
	ldvt	= n;

	transposeM(result, M);

	printf("\tPerforming SVD ..."); 
	dgesdd_(&jobu, &m, &n, result.begin(), &lda, S, U.begin(), &ldu, VT.begin(), &ldvt, &work[0], &lwork, &iwork[0], &info);
	// former MKL function call:
	//DGESDD(&jobu, &m, &n, result.begin(), &lda, S, U.begin(), &ldu, VT.begin(), &ldvt, 
	//		&work[0], &lwork, &iwork[0], &info);

	printf("\tInverting S ...");
	for(int i=0; i<lds; i++) {
		if(!zero(S[i])) PS[i][i] = 1.0/S[i];
	}

	printf("\tMultiplying ...");
	// NOTE: Couldn't be slower than this !!
	multMM(temp, PS, U);
	multMM(result, VT, temp); 

	delete[]	work;
	delete[]	S;

	return info;
}


#endif

