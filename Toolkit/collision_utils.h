/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Collision Detection and Response Utility Definition (collision_utils.h).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	COLLISION_UTILS.H v0.1.0
////
////		Collision Utilitis.
////
////////////////////////////////////////////////////////////////

#ifndef _COLLISION_UTILS_H
#define _COLLISION_UTILS_H

#include <math.h>
#include "algebra.h"

/** Helper functions. */

/** copy a point, i.e., a three-dimensional vector, src into dst */
static inline void vector_copy(float dst[], float src[]) {
  assert(dst != NULL);
  assert(src != NULL);

  for (int i = 0; i < 3; i++)
    dst[i] = src[i];
}

/** copy a three-dimensional int vector, src into dst */
static inline void vector_copy(int dst[], int src[]) {
  assert(dst != NULL);
  assert(src != NULL);

  for (int i = 0; i < 3; i++)
    dst[i] = src[i];
}

/** copy a point, i.e., a three-dimensional from Vector<Real> src into array of float dst */
static inline void vector_copy(float dst[], Vector<Real> src) {
  assert(dst != NULL);

  for (int i = 0; i < 3; i++)
    dst[i] = src[i];
}

/** copy a point, i.e., a three-dimensional from array of float src into Vector<Real> dst  */
static inline void vector_copy(Vector<Real> *dst, float src[]) {
  assert(src != NULL);

  for (int i = 0; i < 3; i++)
    dst[i] = src[i];
}

/* Calculate and return in ptpos the convex combination of the three points v0, v1, v2 with weights u for v1 and v for v2 */
static void convex_combination(Vector<Real> *ptpos, float v0[], float v1[], float v2[], float u, float v) {
  for (int i = 0; i < 3; i++) 
    ptpos[i] = (1. - u - v) * v0[i] + u * v1[i] + v * v2[i];
}

/* Subtracts the second point from the first one */
static void vector_subtract(float a[], float b[]) {
  for (int i = 0; i < 3; i++)
    a[i] -= b[i];
}

/* Check if a collision has occurred */
static int check_collision(int collided, float t) {
  return collided && t >= 0. && t <= 1.;
}

/** Multiplex function returns the first argument if select=1, otherwise return second argument */
static int multiplex(int arg0, int arg1, int select) {
  return select ? arg0 : arg1;
}


/** Header of triangle-triangle intersection test. */
int coplanar_tri_tri(float N[3],float V0[3],float V1[3],float V2[3],
                     float U0[3],float U1[3],float U2[3]);

int tri_tri_intersect(float V0[3],float V1[3],float V2[3],
                      float U0[3],float U1[3],float U2[3]);

int NoDivTriTriIsect(float V0[3],float V1[3],float V2[3],
                     float U0[3],float U1[3],float U2[3]);

inline void isect2(float VTX0[3],float VTX1[3],float VTX2[3],float VV0,float VV1,float VV2,
	    float D0,float D1,float D2,float *isect0,float *isect1,float isectpoint0[3],float isectpoint1[3]);

inline int compute_intervals_isectline(float VERT0[3],float VERT1[3],float VERT2[3],
				       float VV0,float VV1,float VV2,float D0,float D1,float D2,
				       float D0D1,float D0D2,float *isect0,float *isect1,
				       float isectpoint0[3],float isectpoint1[3]);

int tri_tri_intersect_with_isectline(float V0[3],float V1[3],float V2[3],
				     float U0[3],float U1[3],float U2[3],int *coplanar,
				     float isectpt1[3],float isectpt2[3]);

int intersect_triangle(float orig[3], float dir[3],
                   float vert0[3], float vert1[3], float vert2[3],
                   float *t, float *u, float *v);


/** Orientation between triangles whose vertices are v0, v1, and v2 */
typedef enum { none , v2v0_v2v1, v1v0_v1v2, v0v1_v0v2 } GiPSi_triangle_intersection;

inline int compute_intervals_isectline_detail(
        float VERT0[3], /**< First triangle vertex */
        float VERT1[3], /**< Second triangle vertex */
        float VERT2[3], /**< Third triangle vertex */
        float VV0,      /**< Projection point of V0 on the plane */
        float VV1,		/**< Projection point of V1 on the plane */
        float VV2,		/**< Projection point of V2 on the plane */
        float D0,		/**< Distance v0 from the plane */
        float D1,		/**< Distance v1 from the plane */
        float D2,		/**< Distance v2 from the plane */
        float D0D1,		/**< Multiplication of D0 and D1 */
        float D0D2,		/**< Multiplication of D0 and D2 */
        float *isect0,	/**< isect 0 */
        float *isect1,	/**< isect 1 */
		float isectpoint0[3], /**< Intersection point 0 */
        float isectpoint1[3], /**< Intersection point 1 */
        GiPSi_triangle_intersection *orient /**< Orientation */);

int tri_tri_intersect_with_isectline_detail(
					float V0[3],		/**< First vertex of triangle T1 */
					float V1[3],		/**< Second vertex of triangle T1 */
					float V2[3],		/**< Third vertex of triangle T1 */
				    float U0[3],		/**< First vertex of triangle T2 */
					float U1[3],		/**< Second vertex of triangle T2 */
					float U2[3],		/**< Third vertex of triangle T2 */
					int *coplanar,		/**< coplanar variable */
					int *codeT1,		/**< inside/outside vertices referenced by T1 plane */
					int *codeT2,		/**< inside/outside vertices referenced by T1 plane */
					int *codeTT1,		/**< vertices are not count for border/non-colliding of T1 */
					int *codeTT2,		/**< vertices are not count for border/non-colliding of T2 */
					float nv[3],		/**< The normal vector of T1 */
					float nu[3],		/**< The normal vector of T2 */
				    float isectpt1[3],	/**< intersection points 1 */
					float isectpt2[3]	/**< intersection points 2 */);

int intersect_LineTriangle(
					float R0[3],		/**< First vertex of Line */
					float R1[3],		/**< Second vertex of Line */					
				    float T0[3],		/**< First vertex of triangle T2 */
					float T1[3],		/**< Second vertex of triangle T2 */
					float T2[3],		/**< Third vertex of triangle T2 */
					float IPt[3]		/**< intersection points */);

#endif