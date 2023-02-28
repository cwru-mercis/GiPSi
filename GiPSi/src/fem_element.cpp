/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is FEM Element Implementations (fem_element.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	FEM_ELEMENT.CPP v0.0
////
////	FEM Element implementations
////
////////////////////////////////////////////////////////////////


#include "fem.h"
#include <math.h>


////////////////////////////////////////////////////////////////
//
//	Tetrahedra3DFEMElement::computeBeta()
//
//		Calculates reference to current frame matrix beta
//
void Tetrahedra3DFEMElement::computeBeta(	Vector<Real> &p0, Vector<Real> &p1, 
											Vector<Real> &p2, Vector<Real> &p3)
{
	int				i;
	Matrix<Real>	NodePos(4, 4, 1.0);

	for(i=0; i<3; i++)	NodePos[i][0] = p0[i];
	for(i=0; i<3; i++)	NodePos[i][1] = p1[i];
	for(i=0; i<3; i++)	NodePos[i][2] = p2[i];
	for(i=0; i<3; i++)	NodePos[i][3] = p3[i];

	// Last row of NodePos is all ones!

	if(invM(beta, NodePos) != 0)
		printf("FEMElement: Cannot invert beta!\n");
}



////////////////////////////////////////////////////////////////
//
//	Tetrahedra3DFEMElement::computeVolume()
//
//		Computes the volume of the 3D tetrahedra element
//
Real Tetrahedra3DFEMElement::computeVolume(	Vector<Real> &p0, Vector<Real> &p1, 
											Vector<Real> &p2, Vector<Real> &p3)
{
	static Vector<Real>		r(3), a(3), b(3), c(3);

	a = p1 - p0;
	b = p2 - p1;
	c = p3 - p0;

	crossVV(r, a, b);
	
	volume = -1.0/6.0 * (r * c);

	if(volume <= -0.00001)
		printf("DEBUG: Negative volume!\n");

	return volume;
}



////////////////////////////////////////////////////////////////
//
//	Tetrahedra3DFEMElement::computeStrain()
//
//		Computes the strain tensor of the 3D tetrahedra element
//
void Tetrahedra3DFEMElement::computeStrain(	Vector<Real> &p0, Vector<Real> &p1, 
											Vector<Real> &p2, Vector<Real> &p3)
{
	static		Matrix<Real>	P(4, 4, 1.0), Pbeta(4, 4, 0.0);
	
	// Compute Strain:
	//		Strain_AB = 1/2 * (P_nm * beta_nA * P_mt * beta_tB - delta_AB) 

	P[0][0] = p0[0];		P[0][1] = p1[0];		P[0][2] = p2[0];		P[0][3] = p3[0];
	P[1][0] = p0[1];		P[1][1] = p1[1];		P[1][2] = p2[1];		P[1][3] = p3[1];
	P[2][0] = p0[2];		P[2][1] = p1[2];		P[2][2] = p2[2];		P[2][3] = p3[2];

	multMM(Pbeta, P, beta);

	strain[0][0] = 0.5 * (Pbeta[0][0] * Pbeta[0][0] + Pbeta[1][0] * Pbeta[1][0] + Pbeta[2][0] * Pbeta[2][0] + Pbeta[3][0] * Pbeta[3][0] - 1.0);
	strain[1][1] = 0.5 * (Pbeta[0][1] * Pbeta[0][1] + Pbeta[1][1] * Pbeta[1][1] + Pbeta[2][1] * Pbeta[2][1] + Pbeta[3][1] * Pbeta[3][1] - 1.0);
	strain[2][2] = 0.5 * (Pbeta[0][2] * Pbeta[0][2] + Pbeta[1][2] * Pbeta[1][2] + Pbeta[2][2] * Pbeta[2][2] + Pbeta[3][2] * Pbeta[3][2] - 1.0);

	strain[0][1] = strain[1][0] = 0.5 * (Pbeta[0][0] * Pbeta[0][1] + Pbeta[1][0] * Pbeta[1][1] + Pbeta[2][0] * Pbeta[2][1] + Pbeta[3][0] * Pbeta[3][1]);
	strain[0][2] = strain[2][0] = 0.5 * (Pbeta[0][0] * Pbeta[0][2] + Pbeta[1][0] * Pbeta[1][2] + Pbeta[2][0] * Pbeta[2][2] + Pbeta[3][0] * Pbeta[3][2]);
	strain[1][2] = strain[2][1] = 0.5 * (Pbeta[0][1] * Pbeta[0][2] + Pbeta[1][1] * Pbeta[1][2] + Pbeta[2][1] * Pbeta[2][2] + Pbeta[3][1] * Pbeta[3][2]);
}


////////////////////////////////////////////////////////////////
//
//	Tetrahedra3DFEMElement::computeStrain()
//
//		Computes the strain tensor of the 3D tetrahedra element
//
void Tetrahedra3DFEMElement::computeStrainVelocity(	Vector<Real> &p0, Vector<Real> &p1, 
													Vector<Real> &p2, Vector<Real> &p3,
													Vector<Real> &v0, Vector<Real> &v1, 
													Vector<Real> &v2, Vector<Real> &v3)
{
	static		Matrix<Real>	P(4, 4, 1.0), Pbeta(4, 4, 0.0);
	static		Matrix<Real>	V(4, 4, 1.0), Vbeta(4, 4, 0.0);

	P[0][0] = p0[0];		P[0][1] = p1[0];		P[0][2] = p2[0];		P[0][3] = p3[0];
	P[1][0] = p0[1];		P[1][1] = p1[1];		P[1][2] = p2[1];		P[1][3] = p3[1];
	P[2][0] = p0[2];		P[2][1] = p1[2];		P[2][2] = p2[2];		P[2][3] = p3[2];

	multMM(Pbeta, P, beta);

	V[0][0] = v0[0];		V[0][1] = v1[0];		V[0][2] = v2[0];		V[0][3] = v3[0];
	V[1][0] = v0[1];		V[1][1] = v1[1];		V[1][2] = v2[1];		V[1][3] = v3[1];
	V[2][0] = v0[2];		V[2][1] = v1[2];		V[2][2] = v2[2];		V[2][3] = v3[2];

	multMM(Vbeta, V, beta);

	strain_velocity[0][0] = (Vbeta[0][0] * Pbeta[0][0] + Vbeta[1][0] * Pbeta[1][0] + Vbeta[2][0] * Pbeta[2][0] + Vbeta[3][0] * Pbeta[3][0]);
	strain_velocity[1][1] = (Vbeta[0][1] * Pbeta[0][1] + Vbeta[1][1] * Pbeta[1][1] + Vbeta[2][1] * Pbeta[2][1] + Vbeta[3][1] * Pbeta[3][1]);
	strain_velocity[2][2] = (Vbeta[0][2] * Pbeta[0][2] + Vbeta[1][2] * Pbeta[1][2] + Vbeta[2][2] * Pbeta[2][2] + Vbeta[3][2] * Pbeta[3][2]);

	strain_velocity[0][1] = strain_velocity[1][0] = 0.5 * (	Vbeta[0][0] * Pbeta[0][1] + Vbeta[1][0] * Pbeta[1][1] + Vbeta[2][0] * Pbeta[2][1] + Vbeta[3][0] * Pbeta[3][1] +
															Pbeta[0][0] * Vbeta[0][1] + Pbeta[1][0] * Vbeta[1][1] + Pbeta[2][0] * Vbeta[2][1] + Pbeta[3][0] * Vbeta[3][1]);
	strain_velocity[0][2] = strain_velocity[2][0] = 0.5 * (	Vbeta[0][0] * Pbeta[0][2] + Vbeta[1][0] * Pbeta[1][2] + Vbeta[2][0] * Pbeta[2][2] + Vbeta[3][0] * Pbeta[3][2] +
															Pbeta[0][0] * Vbeta[0][2] + Pbeta[1][0] * Vbeta[1][2] + Pbeta[2][0] * Vbeta[2][2] + Pbeta[3][0] * Vbeta[3][2]);
	strain_velocity[1][2] = strain_velocity[2][1] = 0.5 * (	Vbeta[0][1] * Pbeta[0][2] + Vbeta[1][1] * Pbeta[1][2] + Vbeta[2][1] * Pbeta[2][2] + Vbeta[3][1] * Pbeta[3][2] +
															Pbeta[0][1] * Vbeta[0][2] + Pbeta[1][1] * Vbeta[1][2] + Pbeta[2][1] * Vbeta[2][2] + Pbeta[3][1] * Vbeta[3][2]);
}



////////////////////////////////////////////////////////////////
//
//	Tetrahedra3DFEMElement::computeStress()
//
//		Updates the stress tensor of the 3D tetrahedra element
//
void Tetrahedra3DFEMElement::computeStress()
{
	static int				A, B;		// index variables
	Real					trace_strain =	traceM(strain);
	Real					trace_strain_velocity = traceM(strain_velocity);

	// Compute Stress:
	//		stress_AB = (lamda * strain_kk * delta_AB + 2 * mu * strain_AB) 
	//					+ (phi * strain_velocity_kk + 2 * nu * strain_velocity_AB)
	for(A=0; A<3; A++)
		for(B=0; B<3; B++) {
			stress[A][B] =	2.0 * Mu * strain[A][B];
			stress[A][B] += 2.0 * Nu * strain_velocity[A][B];
		}

	stress[0][0] += Lambda * trace_strain + Phi * trace_strain_velocity;
	stress[1][1] += Lambda * trace_strain + Phi * trace_strain_velocity;
	stress[2][2] += Lambda * trace_strain + Phi * trace_strain_velocity;
}


////////////////////////////////////////////////////////////////
//
//	Tetrahedra3DFEMElement::computeForces()
//
//		Computes the nodal forces excerted by the 3D tetrahedra element
//
void Tetrahedra3DFEMElement::computeForces(	Vector<Real> &p0, Vector<Real> &p1, 
											Vector<Real> &p2, Vector<Real> &p3)
{
	static	Matrix<Real>	BBS(4, 4, 0.0), BS(4, 3, 0.0), BST(3, 4, 0.0), beta43(4, 3, 0.0);
	int		i,j;

	// Calculate new force acting on each node i 
	// f_[i] = -V/2 (P_[j] * beta_jm * beta_ik * stress_kl)

	for(i = 0; i<4; i++)
		for(j = 0; j<3; j++)
			beta43[i][j] = beta[i][j];

	multMM(BS, beta43, stress);
	
	transposeM(BST, BS);
	
	multMM(BBS, beta43, BST);

	NodeForce[0] = p0 * BBS[0][0] + p1 * BBS[1][0] + p2 * BBS[2][0] + p3 * BBS[3][0];
	NodeForce[1] = p0 * BBS[0][1] + p1 * BBS[1][1] + p2 * BBS[2][1] + p3 * BBS[3][1];
	NodeForce[2] = p0 * BBS[0][2] + p1 * BBS[1][2] + p2 * BBS[2][2] + p3 * BBS[3][2];
	NodeForce[3] = p0 * BBS[0][3] + p1 * BBS[1][3] + p2 * BBS[2][3] + p3 * BBS[3][3];

	Real	v2 = -volume * 0.5;

	NodeForce[0] *= v2;
	NodeForce[1] *= v2;
	NodeForce[2] *= v2;
	NodeForce[3] *= v2;

}
