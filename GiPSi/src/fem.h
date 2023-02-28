/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for 3D Linear Material FEM Module (fem.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	FEM.H v0.0
////
////	Header for 3D Linear Material FEM Module
////
////////////////////////////////////////////////////////////////

#ifndef _FEM_H
#define _FEM_H

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"

// Base class Element class
template <unsigned int NODE, unsigned int DIM>
class FEMElement : public Element{
public:
	unsigned int			node_id[NODE];		// Reference id 
	Real					volume;				// Volume of the element
	Matrix<Real>			strain;				// Strain tensor
	Matrix<Real>			stress;				// Stress tensor
	Matrix<Real>			strain_velocity;	// Strain velocity tensor
	Matrix<Real>			beta;				// Linear basis function matrix
	Vector<Real>			NodeForce[NODE];	// Nodal forces based on this element
	Real					density;			// Density of the element
	Real					Mu;					// Mu for the element
	Real					Lambda;				// Lambda for the element
	Real					Nu;					// Nu for the element
	Real					Phi;				// Phi for the element

	FEMElement():		strain(DIM, DIM, 0.0), stress(DIM, DIM, 0.0), strain_velocity(DIM, DIM, 0.0), 
						beta(DIM+1, NODE, 0.0) {}
	~FEMElement() {};

	void	init(unsigned int	*nodes) {
		for(unsigned int i=0; i<NODE; i++)
			node_id[i] = nodes[i];
	}

	Real				computeVolume() { return 0;};
	void				computeBeta();

protected:


};


class Triangle2DFEMElement : public FEMElement<3, 2> {
public:
	Triangle2DFEMElement();
	~Triangle2DFEMElement();
};


class Triangle3DFEMElement : public FEMElement<3, 3> {
public:
	Triangle3DFEMElement();
	~Triangle3DFEMElement();
};


class Tetrahedra3DFEMElement : public FEMElement<4, 3> {
public:
	Tetrahedra3DFEMElement() {
		// Initialize node force vectors
		// NOTE: This can be done at the base class
		for(int i=0; i<4; i++) {
			NodeForce[i] = zero_vector3;
		}
	};

	~Tetrahedra3DFEMElement() {};

	Real	computeVolume	(	Vector<Real> &p0, Vector<Real> &p1, 
								Vector<Real> &p2, Vector<Real> &p3);
	void	computeStrain	(	Vector<Real> &p0, Vector<Real> &p1, 
								Vector<Real> &p2, Vector<Real> &p3);
	void	computeStress	();
	void	computeStrainVelocity(	Vector<Real> &p0, Vector<Real> &p1, 
									Vector<Real> &p2, Vector<Real> &p3,
									Vector<Real> &v0, Vector<Real> &v1, 
									Vector<Real> &v2, Vector<Real> &v3);
	void	computeBeta		(	Vector<Real> &p0, Vector<Real> &p1, 
								Vector<Real> &p2, Vector<Real> &p3);
	void	computeForces	(	Vector<Real> &p0, Vector<Real> &p1, 
								Vector<Real> &p2, Vector<Real> &p3);
};



// Base class for 3D Linear Material FEM Object
class FEM_3LMObject: public SIMObject {
public:
	// State info for the FEM_3LM
	typedef struct {
		Vector<Real>	*pos;		// position of nodes		[m]
		Vector<Real>	*vel;		// velocity of nodes		[m/s]
		unsigned int	size;
	} State;

	FEM_3LMObject(	char* name, 
					Real *RGBAcolor,
					Real time		= 0.0, 
					Real timestep	= 0.01,
					Real g			= 100.0, 
					Real mass		= 1.0): 
						SIMObject(name, time, timestep), 
						g(g), defaultMass(mass) { 
		for (int i=0; i<4; i++) initialcolor[i]=RGBAcolor[i];
	}

	// Loaders
	void			Load(char *filename);

	// Force calculators
	void			UpdateForces(State &state);	
	void			ExternalForce(void);

	// Integrator interface
	void			AccumState(State &new_state, const State &state, const State &deriv, const Real &h);
	void			DerivState(State &deriv, State &state);
	void			AllocState(State &s);
	void			Simulate(void);

	// Get and Set interfaces for the Boundary and the Domain
	Vector<Real>	GetNodePosition(unsigned int index);
	Vector<Real>	GetNodeVelocity(unsigned int index);
	Vector<Real>	GetNodeForce(unsigned int index);
	void			SetMaterial(unsigned int element_index, Real Rho, Real Mu, Real Lambda, Real Nu, Real Phi);
	void			SetMaterial(Real *Rho, Real *Mu, Real *Lambda, Real *Nu, Real *Phi);
	void			SetMaterial(Real Rho, Real Mu, Real Lambda, Real Nu, Real Phi);
	void			updateMass(void);

	// Display functions
	void			Display(void);
	void			SetupDisplay(void);

	void			Geom2State(void);
	void			State2Geom(void);
	void			State2Bound(void);

	State&			GetState(void)	{	return	state; }

protected:
	State						state;
	Vector<Real>				*rcpos;			// Reference config positions
	Vector<Real>				*force;			// Force vector		[g m/s2]
	Vector<Real>				mass;			// Mass vector		[g]
	unsigned int				num_node;
    unsigned int				num_element;
    Tetrahedra3DFEMElement		*elements;

	Integrator<FEM_3LMObject>	*integrator;

	const Real					g;				// Local copy of the gravity			[m/s2]
	Real						defaultMass;	// The default mass value of the mesh	[g]

	Real						initialcolor[4];// Initial color specified in constructor
												//   It is used in the init() function  

	// Initialization and allocation of internal variables
	//   invoked from Load() method
	void			init(unsigned int num_node, unsigned int num_element); 
    
};



// Base FEM boundary class
class FEMBoundary : public Boundary {
public:
	unsigned int		*global_id;
	unsigned int		*facetovertexids;
	unsigned int		*boundary_type;
	Vector<Real>		*boundary_value;
	Real				*boundary_value2_scalar;
	Vector<Real>		*boundary_value2_vector;

	// Pointer to the parent object
	FEM_3LMObject		*Object;

	// Initializer
	void				init(FEM_3LMObject *ParentObj, unsigned int num_vertex, unsigned int num_face);

	// Get and Set interfaces
	Vector<Real>		GetPosition(unsigned int index);
	void				GetPosition(Vector<Real> *Bpos);	
	Vector<Real>		GetVelocity(unsigned int index);
	void				GetVelocity(Vector<Real> *Bvel);	
	Vector<Real>		GetReactionForce(unsigned int index);
	void				GetReactionForce(Vector<Real> *Bforce);	

	void				Set(unsigned int index, unsigned int boundary_type, 
							Vector<Real> boundary_value, 
							Real boundary_value2_scalar, Vector<Real> boundary_value2_vector);
	void				Set(unsigned int *boundary_type, 
							Vector<Real> *boundary_value, 
							Real *boundary_value2_scalar, Vector<Real> *boundary_value2_vector);
protected:
 
};


// Base FEM domain class
class FEMDomain : public Domain {
public:
	Matrix<Real>		*DomStress;

	// Pointer to the parent object
	FEM_3LMObject		*Object; 

	// Initializer
	void				init(FEM_3LMObject *ParentObj);

	// Get and Set interfaces
	Matrix<Real>		GetStress(unsigned int element_index);
	void				GetStress(Matrix<Real> *StressArray);
	Matrix<Real>		GetStrain(unsigned int element_index);
	void				GetStrain(Matrix<Real> *StrainArray);

	void				SetDomainStress(unsigned int element_index, Matrix<Real> Stress_Tensor);
	void				SetDomainStress(Matrix<Real> *Stress_Tensor_Array);
	void				SetMaterial(unsigned int element_index, Real Rho, Real Mu, Real Lambda, Real Nu, Real Phi);
	void				SetMaterial(Real *Rho, Real *Mu, Real *Lambda, Real *Nu, Real *Phi);
	void				SetForce(unsigned int node_index, Vector<Real> Force_Vector);
	void				SetForce(Vector<Real> *Force_Vector);
};



#endif