/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for Simple Mass-Spring-Damper Model (msd.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Paul Jacobs, Suriya Natsupakpong.
*/

////	MSD_HAPTICS.C v0.1.0
////
////	Defines:
////		BuildHapticLowerOrderModelFromMSD	-	Generator function to linearize 
////                                                  MSD models
////
////////////////////////////////////////////////////////////////

#include "msd_haptics.h"

double inline Aentry(double k, double lzero, double pi, double qi, double norm)
{
	return k*(1.0 - (lzero/norm)*((norm*norm - (pi - qi)*(pi - qi))/(norm*norm)));	
}

double inline Bentry(double k, double lzero, double pi, double pj, double qi, double qj, double norm)
{
	return k*(lzero/norm)*(((qi - pi)*(qj - pj))/(norm*norm));
}

double inline Centry(double b, double wvqp, double vi, double wi, double pi, double qi, double norm)
{
	return b*( (wi - vi)*((qi-pi)/(norm*norm)) + wvqp*(((norm*norm) - (2*(qi-pi)*(qi-pi))) /(norm*norm*norm*norm)));
}

double inline Dentry(double b, double wvqp, double vj, double wj, double pi, double pj, double qi, double qj, double norm)
{
	return b*( (wj - vj)*((qi-pi)/(norm*norm)) - wvqp * (2*(qi-pi)*(qj-pj))/(norm*norm*norm*norm));
}
 
double inline Eentry(double b, double pi, double qi, double norm)
{
	return b*( (qi - pi)*(qi - pi) ) / (norm * norm);
}

double inline Fentry(double b, double pi, double pj, double qi, double qj, double norm)
{
	return b*( (qi - pi) * (qj - pj) ) / (norm * norm);
}

void SplatMatrix(Matrix<Real>& result, const Matrix<Real>& a, unsigned int r, unsigned int c, double alpha)
{
    for(unsigned int i=0; i < a.m(); i++)
	  for(unsigned int j=0; j < a.n(); j++)
		  result[r+i][c+j] += a[i][j]*alpha;			  
}

Matrix<Real> BuildKMatrix(const Vector<Real>& p, const Vector<Real>& q, double k, double lzero)
{
    Vector<Real> t = p;
    t -= q;
    double norm = t.length();
    
	Matrix<Real> m(3,3,0.0);

	if (norm>0){
    
    m[0][0] = Aentry(k, lzero, p[0], q[0], norm);
    m[0][1] = Bentry(k, lzero, p[0], p[1], q[0], q[1], norm);
    m[0][2] = Bentry(k, lzero, p[0], p[2], q[0], q[2], norm);

    m[1][0] = m[0][1];
    m[1][1] = Aentry(k, lzero, p[1], q[1], norm);
    m[1][2] = Bentry(k, lzero, p[1], p[2], q[1], q[2], norm);

    m[2][0] = m[0][2];
    m[2][1] = m[1][2];
    m[2][2] = Aentry(k, lzero, p[2], q[2], norm);
	}

    return m;
}

Matrix<Real> BuildBMatrix(Vector<Real>& p, Vector<Real>& q, double b)
{
	Vector<Real> t = p;
	t -= q;
	double norm = t.length();

	Matrix<Real> m(3,3,0.0);
	
	if (norm>0){
	m[0][0] = Eentry(b, p[0], q[0], norm);
	m[0][1] = Fentry(b, p[0], p[1], q[0], q[1], norm);
	m[0][2] = Fentry(b, p[0], p[2], q[0], q[2], norm);

	m[1][0] = m[0][1];
	m[1][1] = Eentry(b, p[1], q[1], norm);
	m[1][2] = Fentry(b, p[1], p[2], q[1], q[2], norm);

	m[2][0] = m[0][2];
	m[2][1] = m[1][2];
	m[2][2] = Eentry(b, p[2], q[2], norm);
	}

	return m;
} 

Matrix<Real> BuildVMatrix(Vector<Real>& p, Vector<Real>& q,
                          Vector<Real>& v, Vector<Real>& w,
                          double b)
{
	Vector<Real> t = q - p;
	double norm = t.length();
	Vector<Real> d = w - v;
	double wvqp = d*t;

	Matrix<Real> m(3,3,0.0);

	if (norm>0){
	m[0][0] = Centry(b, wvqp, v[0], w[0], p[0], q[0], norm);
	m[0][1] = Dentry(b, wvqp, v[1], w[1], p[0], p[1], q[0], q[1], norm);
	m[0][2] = Dentry(b, wvqp, v[2], w[2], p[0], p[2], q[0], q[2], norm);

	m[1][0] = Dentry(b, wvqp, v[0], w[0], p[1], p[0], q[1], q[0], norm);
	m[1][1] = Centry(b, wvqp, v[1], w[1], p[1], q[1], norm);
	m[1][2] = Dentry(b, wvqp, v[2], w[2], p[1], p[2], q[1], q[2], norm);

	m[2][0] = Dentry(b, wvqp, v[0], w[0], p[2], p[0], q[2], q[0], norm);
	m[2][1] = Dentry(b, wvqp, v[1], w[1], p[2], p[1], q[2], q[1], norm);
	m[2][2] = Centry(b, wvqp, v[2], w[2], p[2], q[2], norm);
	}

	return m;        
}

void AddKMatrixToA(GiPSiLowOrderLinearHapticModel* model, 
				int ind1, int ind2, Vector<Real> p, Vector<Real> q, double k, double lzero)
{
	Matrix<Real> mK = BuildKMatrix(p, q, k, lzero);

	SplatMatrix(*(model->A12), mK, ind1 *3, ind1 *3, -1.0);
	SplatMatrix(*(model->A12), mK, ind1 *3, ind2 *3, 1.0);
	SplatMatrix(*(model->A12), mK, ind2 *3, ind1 *3, 1.0);
	SplatMatrix(*(model->A12), mK, ind2 *3, ind2 *3, -1.0);
}

void AddBMatrixToA(GiPSiLowOrderLinearHapticModel* model, 
				int ind1, int ind2, Vector<Real> p, Vector<Real> q, double b)
{		
	Matrix<Real> mB = BuildBMatrix(p, q, b);	

	SplatMatrix(*(model->A11), mB, ind1 *3, ind1 *3, -1.0);
	SplatMatrix(*(model->A11), mB, ind1 *3, ind2 *3, 1.0);
	SplatMatrix(*(model->A11), mB, ind2 *3, ind1 *3, 1.0);
	SplatMatrix(*(model->A11), mB, ind2 *3, ind2 *3, -1.0);
}

void AddVMatrixToA(GiPSiLowOrderLinearHapticModel* model,  
				int ind1, int ind2, Vector<Real> p, Vector<Real> q, Vector<Real> v, Vector<Real> w, double b)
{	
	Matrix<Real> mV = BuildVMatrix(p, q, v, w, b);

	SplatMatrix(*(model->A12), mV, ind1 *3, ind1 *3, -1.0);
	SplatMatrix(*(model->A12), mV, ind1 *3, ind2 *3, 1.0);
	SplatMatrix(*(model->A12), mV, ind2 *3, ind1 *3, 1.0);
	SplatMatrix(*(model->A12), mV, ind2 *3, ind2 *3, -1.0);

}

void AddInternalSpringEntries(GiPSiLowOrderLinearHapticModel* model, int ind1, int ind2, 
								Vector<Real> p, Vector<Real> q, Vector<Real> v, Vector<Real> w, 
								double k, double lzero, double b)
{
    AddKMatrixToA(model, ind1, ind2, p, q, k, lzero);
    AddVMatrixToA(model, ind1, ind2, p, q, v, w, b);
    AddBMatrixToA(model, ind1, ind2, p, q, b);
}

// ind is an index of another node (not a boundary node)
void AddBoundarySpringEntries(GiPSiLowOrderLinearHapticModel* model, int ind, 
								Vector<Real> p, Vector<Real> q, Vector<Real> v, Vector<Real> w, 
								double k, double lzero, double b)
{    
	Matrix<Real> mK = BuildKMatrix(p, q, k, lzero);
	Matrix<Real> mB = BuildBMatrix(p, q, b);	
	Matrix<Real> mV = BuildVMatrix(p, q, v, w, b);
	
	SplatMatrix(*(model->A11), mB, ind *3, ind *3, -1.0);		
	SplatMatrix(*(model->A12), mV, ind *3, ind *3, -1.0);
	SplatMatrix(*(model->A12), mK, ind *3, ind *3, -1.0);
}

// ind is an index of another node (not a contact node)
void AddContactSpringEntries(GiPSiLowOrderLinearHapticModel* model, int ind, 
								Vector<Real> p, Vector<Real> q, Vector<Real> v, Vector<Real> w, 
								double k, double lzero, double b)
{
    Matrix<Real> mK = BuildKMatrix(p, q, k, lzero);
    Matrix<Real> mB = BuildBMatrix(p, q, b);
    Matrix<Real> mV = BuildVMatrix(p, q, v, w, b);

	SplatMatrix(*(model->A11), mB, ind *3, ind *3, -1.0);
	SplatMatrix(*(model->A12), mV, ind *3, ind *3, -1.0);
	SplatMatrix(*(model->A12), mK, ind *3, ind *3, -1.0);	

	SplatMatrix((*(model->B1)), mB, ind *3, 0, 1.0);
	SplatMatrix((*(model->B1)), mV, ind *3, 3, 1.0);
	SplatMatrix((*(model->B1)), mK, ind *3, 3, 1.0);
	
	SplatMatrix((*(model->C11)), mB, 0, ind *3, -1.0);
	SplatMatrix((*(model->C12)), mV, 0, ind *3, -1.0);
	SplatMatrix((*(model->C12)), mK, 0, ind *3, -1.0);	
	
	SplatMatrix((*(model->D))  , mB, 0, 0, -1.0);
	SplatMatrix((*(model->D))  , mV, 0, 3, -1.0); 	
	SplatMatrix((*(model->D))  , mK, 0, 3, -1.0);
}

void AddContactSpringEntries(GiPSiLowOrderLinearHapticModel* model, 
								Vector<Real> p, Vector<Real> q, Vector<Real> v, Vector<Real> w, 
								double k, double lzero, double b)
{
    Matrix<Real> mK = BuildKMatrix(p, q, k, lzero);
    Matrix<Real> mB = BuildBMatrix(p, q, b);
    Matrix<Real> mV = BuildVMatrix(p, q, v, w, b);
	
	SplatMatrix((*(model->D))  , mB, 0, 0, -1.0);
	SplatMatrix((*(model->D))  , mV, 0, 3, -1.0); 	
	SplatMatrix((*(model->D))  , mK, 0, 3, -1.0);
}

///// for implicit //////////////////////////////////////
void AddKMatrixToA(Matrix<Real>* model, int ind1, int ind2, Vector<Real> p, Vector<Real> q, double k, double lzero)
{
	Matrix<Real> mK = BuildKMatrix(p, q, k, lzero);

	SplatMatrix( *model, mK, ind1 *3, ind1 *3, -1.0);
	SplatMatrix( *model, mK, ind1 *3, ind2 *3, 1.0);
	SplatMatrix( *model, mK, ind2 *3, ind1 *3, 1.0);
	SplatMatrix( *model, mK, ind2 *3, ind2 *3, -1.0);
}
void AddBMatrixToA(Matrix<Real>* model, int ind1, int ind2, Vector<Real> p, Vector<Real> q, double b)
{		
	Matrix<Real> mB = BuildBMatrix(p, q, b);	

	SplatMatrix( *model, mB, ind1 *3, ind1 *3, -1.0);
	SplatMatrix( *model, mB, ind1 *3, ind2 *3, 1.0);
	SplatMatrix( *model, mB, ind2 *3, ind1 *3, 1.0);
	SplatMatrix( *model, mB, ind2 *3, ind2 *3, -1.0);
}

void AddVMatrixToA(Matrix<Real>* model, int ind1, int ind2, Vector<Real> p, Vector<Real> q, Vector<Real> v, Vector<Real> w, double b)
{	
	Matrix<Real> mV = BuildVMatrix(p,q,v,w,b);

	SplatMatrix( *model, mV, ind1 *3, ind1 *3, -1.0);
	SplatMatrix( *model, mV, ind1 *3, ind2 *3, 1.0);
	SplatMatrix( *model, mV, ind2 *3, ind1 *3, 1.0);
	SplatMatrix( *model, mV, ind2 *3, ind2 *3, -1.0);
}

void AddInternalSpringEntries(Matrix<Real>* A11, Matrix<Real>* A12, int ind1, int ind2, 
								Vector<Real> p, Vector<Real> q, Vector<Real> v, Vector<Real> w, 
								double k, double lzero, double b)
{
	AddBMatrixToA(A11, ind1, ind2, p, q, b);   
	AddKMatrixToA(A12, ind1, ind2, p, q, k, lzero);
    AddVMatrixToA(A12, ind1, ind2, p, q, v, w, b);   
}
///// end for implicit //////////////////////////////////////