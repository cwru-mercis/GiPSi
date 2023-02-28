/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Linear Algebra and Geometry utility functions Header (algebra.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/
////	ALGEBRA.H v0.1.1
////
////	Linear Algebra definitions and inline utility functions
////
////////////////////////////////////////////////////////////////

#ifndef _ALGEBRA_H
#define _ALGEBRA_H
#include <math.h>
#include <float.h>

#ifndef ASSERT
#include <assert.h>
#define ASSERT(expr) assert(expr)
#endif

#ifndef ZERO
#define ZERO 1e-16
#endif

#ifndef EPSILON
#define EPSILON 1e-8
#endif

#ifdef WIN32
#define M_PI 3.1415926535
#endif


#define TORAD(angle)	(angle*M_PI/180.0)
#define TODEG(angle)	(angle*180.0/M_PI)
#define	MAX(a, b)		((a > b) ? (a) : (b))

typedef double Real;

// NOTE: Right now only MKL is used
// Need to predefine ALGEBRA_USE_CBLAS to use CBLAS and LAPACK
#ifdef ALGEBRA_USE_CBLAS
#include "MKL.h"
#else
// else it will default to general implementation
#define ALGEBRA_USE_GENERAL
#endif

#include "vector.h"
#include "matrix.h"
#include "sparse_matrix.h"


inline	bool	zero(Real x, const Real zeroval = ZERO) 
{
	return (x >= -zeroval) && (x <= zeroval);
}

inline	bool	zero(Vector<Real> v, const Real zeroval = ZERO) 
{	
	Real	l = v.length_sq();

	return (l >= -zeroval) && (l <= zeroval);
}

#ifndef min
inline	Real	min(Real	a,	Real b)
{
	if (a <= b)
		return a;
	else
		return b;
}
#endif

#ifndef max
inline	Real	max(Real	a,	Real b)
{
	if (b >= a)
		return b;
	else
		return a;
}
#endif


inline	Real	max(Real x, Real y, Real z) {
	if (x > y && x > z) 
		 return x;
	if (y > z) 
		return y;
  
	return z;
}


inline	Real	min(Real x, Real y, Real z) {
	if (x < y && x < z) 
		return x;
	if (y < z) 
		return y;
	return z;
}


inline int		LineIntersect(	Vector<Real> &A0, Vector<Real> &A1, Real &s,
								Vector<Real> &B0, Vector<Real> &B1, Real &t)
{
	// NOTE:	This is 2D only!

	Real dax = A1[0] - A0[0];
	Real day = A1[1] - A0[1];
	Real dbx = B1[0] - B0[0];
	Real dby = B1[1] - B0[1];

	Real d = dby * dax - dbx * day;

	Real a = dbx * (A0[1] - B0[1]) - dby * (A0[0] - B0[0]);
	Real b = dax * (A0[1] - B0[1]) - day * (A0[0] - B0[0]);


	if(zero(d))	return 0;

	s = a/d;
	t = b/d;

	if((s >= 0.0 && s <= 1.0) && (t >=0.0 && t <= 1.0)) {
		return 1;
	}
	else return 0;
}


inline int		TriangleIntersect(	Vector<Real> &A0, Vector<Real> &A1, Real &t,
									Vector<Real> &v0, Vector<Real> &v1, Vector<Real> &v2, 
									Real &u, Real &v)
{
	Vector<Real>		edge1(3, 0.0), edge2(3, 0.0), tvec(3, 0.0);
	Vector<Real>		dir(3, 0.0), pvec(3, 0.0), qvec(3, 0.0);
	Real				det,	inv_det;

	// find vectors for two edges sharing v0
	edge1 =  v1 - v0;
	edge2 =  v2 - v0;

	dir	= A1 - A0;

	// begin calculating determinant - also used to calculate U parameter
	crossVV(pvec, dir, edge2);

	// if determinant is near zero, ray lies in plane of triangle
	det = edge1 * pvec;

	// calculate distance from v0 to ray origin
	tvec = A0 - v0;
	inv_det = 1.0 / det;

	crossVV(qvec, tvec, edge1);
  
	if (det > EPSILON)
	{
		u = tvec * pvec;
		if (u < 0.0 || u > det)
			return 0;

		// calculate V parameter and test bounds
		v = dir * qvec;
		if (v < 0.0 || u + v > det)
			return 0;

	}
	else if(det < -EPSILON)
	{
		// calculate U parameter and test bounds
		u = tvec * pvec;
		if (u > 0.0 || u < det)
			return 0;

		// calculate V parameter and test bounds
		v = dir * qvec;
		if (v > 0.0 || u + v < det)
			return 0;
	}
	else return 0;  // ray is parallell to the plane of the triangle

	t = edge2 * qvec * inv_det;
	u *= inv_det;
	v *= inv_det;

	if(t >= -EPSILON && t <= 1.0+EPSILON)
		return 1;
	else return 0;
}



inline Real		TriangleArea(Vector<Real> &p0, Vector<Real> &p1, Vector<Real> &p2)
{
	// NOTE:	This is 2D only!

	Vector<Real> a(2, 0.0), b(2, 0.0), c(3, 0.0);

	a = p1-p0;
	b = p2-p1;

	crossVV(c, a, b);

	return c[2]/2.0;
}


inline Real		TetrahedraVolume(Vector<Real> &p0, Vector<Real> &p1, Vector<Real> &p2, Vector<Real> &p3)
{
	Vector<Real> a(3, 0.0), b(3, 0.0), c(3, 0.0), d(3, 0.0);

	a = p1-p0;
	b = p2-p1;

	crossVV(c, a, b);

	d = p3-p0;

	return (c * d)/6.0;
}

#endif

