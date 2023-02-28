/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Simple Cardiac Bioelectricity Model Implementation (bioe.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	bioe.CPP v0.0
////
////	Cardiac Bioelectricity Model
////
////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "errors.h"
#include "timing.h"
#include "bioe.h"

// NOTE: Might want to find a better place for this
static	const Vector<Real>	zero_Vector(3, 0.0);
static	const Matrix<Real>	zero_Matrix(3, 3, 0.0);



////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::Load()
//
//		Reads in .fem file
//
void CardiacBioEObject::Load(char *basename)   
{
	// Load the geometric model for the heart muscle
	HeartMuscleGeometry.Load(basename);
	geometry=&HeartMuscleGeometry;

	/****************************************/
	/*	Setting up of the boundary			*/
	/****************************************/
	// No need for a boundary


	/****************************************/
	/*	Setting up of the domain			*/
	/****************************************/

	//first allocate the domain
	if((domain = new CardiacBioEDomain()) == NULL) {
		printf("ERROR: Could not allocate boundary!\n");
		exit(0);
	}
	CardiacBioEDomain *		dom = (CardiacBioEDomain *) domain;

	//and then load the domain
	dom->Load(basename);
	dom->init(this);

	/****************************************/
	/*	Initialize the states				*/
	/****************************************/
  
	state.x[0]=1.0;
	state.x[1]=0.0;
	
	// NOTE: The method is harcoded for now.
	integrator = new ERKHeun3<CardiacBioEObject>(*this);
}



////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::AccumState()
//
//		Computes x(t+h) = x(t) + h * f(..)
//
//		where	x(t+h)	= new_state
//				x(t)	= state
//				f(..)	= deriv
//
inline void CardiacBioEObject::AccumState(State &new_state, const State &state, const State &deriv, const Real &h)
{
	for(int i = 0; i <2; i++) {
		new_state.x[i] = state.x[i] + deriv.x[i] * h;
	}
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::DerivState()
//
//		Computes the derivatives of state variables. This is simply 
//		the f() evaluation where f() = dx/dt.
//
//
inline void CardiacBioEObject::DerivState(State &deriv, State &state)
{
	deriv.x[0]=state.x[1];
	deriv.x[1]=-(2*M_PI*TemporalFreq)*(2*M_PI*TemporalFreq)*state.x[0];
}



////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::AllocState()
//
//		Allocates the memory for the integrator's local state 
//		members
//
inline void CardiacBioEObject::AllocState(State &s)
{
}



////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::Simulate()
//
//		Integrates the FEM_3LM object by a single timestep
//
//
void CardiacBioEObject::Simulate(void)
{
	integrator->Integrate(*this, timestep);
	time+=timestep;
}




////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::Display()
//
//		Displays the FEM mesh
//
//
void CardiacBioEObject::Display(void)
{
	// Copy the state into the geometry
	State2Geom();

	displayMngr->Display();

}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::State2Geom()
//
//		Copies state to geometry
//
//
inline void CardiacBioEObject::State2Geom(void)
{
	static TetVolume *geom = (TetVolume *) geometry;

	for(unsigned int i=0; i<geom->num_vertex; i++) {
		geom->vertex[i].color[1] = CalcExcitation(geom->vertex[i].pos[0]);
	}
}

////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::GetExcitation()
//
//		Returns excitation level for the element
//
//
Real	CardiacBioEObject::GetExcitation(unsigned int index)
{
	Real xloc=0.0;

	for (int i=0; i<4; i++) 
		xloc += 0.25*HeartMuscleGeometry.tet[index].vertex[i]->pos[0];

	return CalcExcitation(xloc);
}

////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::CalcExcitation()
//
//		Returns excitation level for a location
//
//

Real	CardiacBioEObject::CalcExcitation(Real xloc)
{
	Real refcoord= 0.0;

	// The following is a simple finite state machine bioelectric model
	//   used in the two chamber heart simulation
	// Timing of the switching is coupled to the second order oscillator
	//   from the state of the model
	if (state.x[0]>0.1) {
		return (xloc>2.5)?1:0;
	}
	else if (state.x[0]<-0.1) {
		return (xloc<-2.5)?1:0;
	}
	else {
		return 0.0;
	}
	// The following is a simple wave propagation bioelectric model
	//   used in the single chamber heart model
	//Real temp=cos(2*M_PI*TemporalFreq*(time-(xloc-refcoord)/SpatialVel)+M_PI)-cos(M_PI*DutyCycle);
	//return (xloc>0)? ((temp>0)?1:0):(0.0) ;
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::SetMuscleStrain
//
//		Sets Muscle strain to use in excitation contraction coupling
//
void	CardiacBioEObject::SetMuscleStrain(unsigned int index, Matrix<Real> Strain)	// we will need this for the real thing
{
	error_exit(-1,"Functionality not implemented yet!\n");
}

////////////////////////////////////////////////////////////////
//
//	CardiacBioEObject::SetMuscleGeometry
//
//		Updates the muscle geometry
//
void	CardiacBioEObject::SetHeartMuscleGeometry(TetVolume *newgeom)				// will we need this ???
{
	error_exit(-1,"Functionality not implemented yet!\n");
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEDomain::init
//
//		CardiacBioEObject Domain initializer
//
void			CardiacBioEDomain::init(CardiacBioEObject *ParentObj)
{
	unsigned int i;

	Object=ParentObj;

	DomStrain = new Matrix<Real>[num_tet];
	
	if(DomStrain == NULL) {
		error_exit(-1, "Cannot allocate memory for domain stresses!\n");
	}	
	
	for(i=0; i<num_tet; i++)	DomStrain[i] = zero_Matrix;

}

////////////////////////////////////////////////////////////////
//
//	CardiacBioEDomain::SetDomStrain
//
//		Sets Domain Strain
//
void	CardiacBioEDomain::SetDomainStrain(unsigned int element_index, Matrix<Real> Strain_Tensor)
{
	DomStrain[element_index]=Strain_Tensor;
}

////////////////////////////////////////////////////////////////
//
//	CardiacBioEDomain::SetDomStrain
//
//		Sets Domain Strain
//

void		CardiacBioEDomain::SetDomainStrain(Matrix<Real> *Strain_Tensor_Array)
{
	for (unsigned int index=0; index < this->num_tet; index++){
		DomStrain[index]=Strain_Tensor_Array[index];
	}
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEDomain::SetGeometry
//
//		Sets Domain Geometry
//

void		CardiacBioEDomain::SetGeometry()
{
	error_exit(-1,"Functionality not implemented yet!\n");
}


////////////////////////////////////////////////////////////////
//
//	CardiacBioEDomain::Get
//
//		Sets External Excitation
//
Real		CardiacBioEDomain::GetExcitation(unsigned int element_index)
{
	
	return  Object->GetExcitation(element_index);
}


void		CardiacBioEDomain::GetExcitation(Real *Excitation_Array)
{
	for (unsigned int index=0; index < this->num_tet; index++){
		Excitation_Array[index]=Object->GetExcitation(index);
	}
}

