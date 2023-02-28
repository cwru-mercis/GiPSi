/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Lumped Fluid Implementation (lumpedfluid.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	lumpedfluid.CPP v0.0
////
////	Lumped Fluid Model
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
#include "lumpedfluid.h"

// NOTE: Might want to find a better place for this
static	const Vector<Real>	zero_Vector(3, 0.0);
static	const Matrix<Real>	zero_Matrix(3, 3, 0.0);

////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::init()
//
//		Initialize the LumpedFluidObject
//
void LumpedFluidObject::init(unsigned int num_node)
{
	unsigned int i;

	this->num_node		= num_node;

	// Allocate and init state
	state.size	= num_node;
	if((state.pos = new Vector<Real>[state.size]) == NULL) {
		error_exit(-1, "Cannot allocate memory for positions!\n");
	}
	for(i=0; i<state.size; i++) state.pos[i] = zero_vector3;
	
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::Load()
//
//		Reads in .fem file
//
void LumpedFluidObject::Load(char *basename)   
{
	unsigned int	i;

	/************************************************************/
	/*	Load the geometric model for the heart muscle			*/
	/************************************************************/
	ChamberGeometry.Load(basename);
	
	/************************************************************/
	/*	Initialize the LumpedFluidObject						*/
	/************************************************************/
	init(ChamberGeometry.num_vertex);

	/****************************************/
	/*	Set geometry        				*/
	/****************************************/
	geometry=&ChamberGeometry;

	/****************************************/
	/*	Setting up of the boundary			*/
	/****************************************/
	// Allocate boundary
	if((boundary = new LumpedFluidBoundary()) == NULL) {
		printf("ERROR: Could not allocate boundary!\n");
		exit(0);
	}
	LumpedFluidBoundary*	bound = (LumpedFluidBoundary *) boundary;
	
	// For performance we are using a reference to vertex and face lists, not a separate variable
	// NOTE:
	//   This makes the vertex list and face list of the geometry accessible
	//   through the LumpedFluidBoundary, while it was protected in SimObject
	bound->init(this,ChamberGeometry.num_vertex,ChamberGeometry.num_face,
					ChamberGeometry.vertex,ChamberGeometry.face);

	//  Fill in the rest of the boundary
	//
	for (i=0; i<bound->num_vertex; i++)
	{
		bound->global_id[i]=i;
		bound->Set(i,0,zero_vector3);
	}
	for (i=0; i<bound->num_face; i++)
	{
		*(bound->facetovertexids+3*i+0)=bound->face[i].vertex[0]->refid;
		*(bound->facetovertexids+3*i+1)=bound->face[i].vertex[1]->refid;
		*(bound->facetovertexids+3*i+2)=bound->face[i].vertex[2]->refid;
	}


	/****************************************/
	/*	Setting up of the domain			*/
	/****************************************/
	// No need for a domain


	/****************************************/
	/*	Initialize the states				*/
	/****************************************/
  
	// first find the center of the chamber
	Vector<Real>	tempv(3,0.0);
	for (i=0; i<ChamberGeometry.num_vertex; i++)
		state.pos[i]=ChamberGeometry.vertex[i].pos;
	for (i=0; i<state.size; i++)
		tempv+=state.pos[i];
	tempv=tempv*(1.0/state.size);
	state.Vf=0;
	for (i=0;i<ChamberGeometry.num_face;i++) 
		state.Vf-=TetrahedraVolume(ChamberGeometry.face[i].vertex[0]->pos,
								ChamberGeometry.face[i].vertex[1]->pos,
								ChamberGeometry.face[i].vertex[2]->pos,
								tempv);
	Vm=state.Vf;
	VfminusVm_last=0.0;
	Pf=Pfo;

	// MENTAL NOTE: move this to somewhere logical asap!
	// NOTE: The method is harcoded for now.
	integrator = new ERKHeun3<LumpedFluidObject>(*this);
	//integrator = new Euler<LumpedFluidObject>(*this);
}



////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::AccumState()
//
//		Computes x(t+h) = x(t) + h * f(..)
//
//		where	x(t+h)	= new_state
//				x(t)	= state
//				f(..)	= deriv
//
inline void LumpedFluidObject::AccumState(State &new_state, const State &state, const State &deriv, const Real &h)
{
	new_state.Vf = state.Vf + deriv.Vf * h;
	for(unsigned int i = 0; i <state.size; i++) {
		new_state.pos[i] = state.pos[i] + deriv.pos[i] * h;
	}
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::DerivState()
//
//		Computes the derivatives of state variables. This is simply 
//		the f() evaluation where f() = dx/dt.
//
//
inline void LumpedFluidObject::DerivState(State &deriv, State &state)
{
	unsigned int			i;
	LumpedFluidBoundary		*bound=(LumpedFluidBoundary *) this->boundary;
	Vector<Real>			tempv(3,0.0);
	Real					Vm,qi,qo,Pf;

	// first find the center of the chamber
	for (i=0; i<state.size; i++)
		tempv+=state.pos[i];
	tempv=tempv*(1.0/state.size);
	// now calculate chamber volume Vm, 
	Vm=0;
	for (i=0;i<bound->num_face;i++) 
		Vm-=TetrahedraVolume(state.pos[bound->face[i].vertex[0]->refid],
								state.pos[bound->face[i].vertex[1]->refid],
								state.pos[bound->face[i].vertex[2]->refid],
								tempv);
	
	for (i=0; i<state.size; i++) 
	{
		if (bound->boundary_type[i]==0) 
			deriv.pos[i]=bound->boundary_value[i];
		else 
			error_exit(-1,"Functionality not implemented or unknown boundary type...");
	}

	Pf=K*(state.Vf-Vm)+Pfo;
	qi=(Pi-Pf)/Ri;  qi=(qi>0)?qi:0;
	qo=(Pf-Po)/Ro;  qo=(qo>0)?qo:0;
	deriv.Vf=qi-qo;

	//printf("Pf=%f Vf=%f Vm=%f qi=%f qo=%f\n",Pf,state.Vf,Vm,qi,qo);

}



////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::AllocState()
//
//		Allocates the memory for the integrator's local state 
//		members
//
inline void LumpedFluidObject::AllocState(State &s)
{
	s.pos = new Vector<Real>[state.size];
	if(s.pos == NULL) {
		error_exit(-1, "Cannot allocate memory for positions!\n");
	}
	for(unsigned int i = 0; i<state.size; i++)	s.pos[i] = zero_vector3;

	s.size = state.size;

	// no need to do anything for Vf since it doesn't require any memory allocation
}



////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::Simulate()
//
//		Integrates the FEM_3LM object by a single timestep
//
//
void LumpedFluidObject::Simulate(void)
{
	integrator->Integrate(*this, timestep);
	time+=timestep;

	//We alse need to update the Boundary from states 
	//   so that we will have the correct Boundary definition for boundary calculations
	State2Bound();

	// now calculate chamber volume Vm, 
	unsigned int i;
	LumpedFluidBoundary		*bound=(LumpedFluidBoundary *) this->boundary;
	// first find the center of the chamber
	Vector<Real>	tempv(3,0.0);
	for (i=0; i<state.size; i++)
		tempv+=state.pos[i];
	tempv=tempv*(1.0/state.size);
	// then calculate volume
	Vm=0;
	for (i=0;i<bound->num_face;i++) 
		Vm-=TetrahedraVolume(state.pos[bound->face[i].vertex[0]->refid],
								state.pos[bound->face[i].vertex[1]->refid],
								state.pos[bound->face[i].vertex[2]->refid],
								tempv);
	// and chamber pressure
	Pf=K*(state.Vf-Vm)+Pfo+B*(state.Vf-Vm-VfminusVm_last)/timestep;
	VfminusVm_last=state.Vf-Vm;

	//printf("Pf=%f Pfo=%f Vf=%f Vm=%f \n",Pf,Pfo,state.Vf,Vm);
}




////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::Display()
//
//		Displays the mesh
//
//
void LumpedFluidObject::Display(void)
{
	// Copy the state into the geometry
	State2Geom();

	displayMngr->Display();

}



////////////////////////////////////////////////////////////////
//
//	FEM_3LMObject::Geom2State()
//
//		Copies geometry to state
//
//
void LumpedFluidObject::Geom2State(void)
{
	static TriSurface *geom = (TriSurface *) geometry;

	for(unsigned int i=0; i < geom->num_vertex; i++) {
		state.pos[i] = geom->vertex[i].pos;
	}
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::State2Geom()
//
//		Copies state to geometry
//
//
inline void LumpedFluidObject::State2Geom(void)
{
	unsigned int		i;
	// Define an intermediate variable so that we can access the geometry without explicit type casting every time
	static TriSurface *geom = (TriSurface *) geometry;  

	// We don't need to do anything to vertex poitions with current implementation 
	//    as geometry is shared with boundary, which is updated at every time step
	// Let's just change color with Pressure

	Real	colormult=(Pf-Pi*.99)/(1.01*Po-Pi*.99);
	for (i=0; i< geom->num_vertex; i++) 
		geom->vertex[i].color[0]=geom->vertex[i].color[2]=colormult;
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::State2Bound()
//
//		Copies state to boundary
//
//
inline void LumpedFluidObject::State2Bound(void)
{
	static LumpedFluidBoundary		*bound = (LumpedFluidBoundary *) boundary;

	for(unsigned int i=0; i<bound->num_vertex; i++) {
		bound->vertex[i].pos = state.pos[bound->global_id[i]];
	}
	bound->calcNormals();
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidObject::GetPressure()
//
//		Returns pressure level for a node and the whole boundary
//
//
Real	LumpedFluidObject::GetPressure(unsigned int index)
{
	return Pf;
}

void	LumpedFluidObject::GetPressure(Real *BPres)
{
	unsigned int i;
	for (i=0; i < boundary->num_vertex; i++)
	{
		*(BPres+i)= Pf;
	}
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidBoundary::init
//
//		LumpedFluidBoundary initializer
//
void	LumpedFluidBoundary::init(LumpedFluidObject *ParentObj, unsigned int num_vertex, unsigned int num_face)
{
	Object				=	ParentObj;
	this->num_vertex	=	num_vertex;
	this->num_face		=	num_face;

	vertex = new Vertex[num_vertex];
	if(vertex == NULL) {
		error_exit(-1, "Cannot allocate memory for vertices!\n");
	}
	face = new Triangle[num_face];
	if(face == NULL) {
		error_exit(-1, "Cannot allocate memory for faces!\n");
	}
	global_id = new unsigned int[num_vertex];
	if(global_id == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	facetovertexids = (unsigned int *) malloc(3*num_face*sizeof(unsigned int));
	if(facetovertexids == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	boundary_type = new unsigned int[num_vertex];
	if(boundary_type == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	boundary_value = new Vector<Real>[num_vertex];
	if(boundary_value == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
}

////////////////////////////////////////////////////////////////
//
//	LumpedFluidBoundary::init
//
//		LumpedFluidBoundary initializer
//			this version uses already existing vertex, face, and glocal idlist rather than allocating new
//
void	LumpedFluidBoundary::init(LumpedFluidObject *ParentObj, unsigned int num_vertex, unsigned int num_face,
						 Vertex *vertexlist, Triangle *facelist)
{
	Object				=	ParentObj;
	this->num_vertex	=	num_vertex;
	this->num_face		=	num_face;

	vertex				=	vertexlist;
	face				=	facelist;

	global_id = new unsigned int[num_vertex];
	if(global_id == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	facetovertexids = (unsigned int *) malloc(3*num_face*sizeof(unsigned int));
	if(facetovertexids == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	boundary_type = new unsigned int[num_vertex];
	if(boundary_type == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
	boundary_value = new Vector<Real>[num_vertex];
	if(boundary_value == NULL) {
		error_exit(-1, "Cannot allocate memory for boundary!\n");
	}
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidBoundary::GetPressure()
//
//		Returns pressure for a node and the whole boundary
//
//
Real	LumpedFluidBoundary::GetPressure(unsigned int index)
{
	return Object->GetPressure(index);
}

void	LumpedFluidBoundary::GetPressure(Real *BPres)
{
	Object->GetPressure(BPres);
}


////////////////////////////////////////////////////////////////
//
//	LumpedFluidBoundary::Set
//
//		Sets boundary conditions for one node and whole boundary
//
void	LumpedFluidBoundary::Set(int index, unsigned int boundary_type, Vector<Real> boundary_value)
{
	this->boundary_type[index]			=boundary_type;
	this->boundary_value[index]			=boundary_value;
}

void	LumpedFluidBoundary::Set(unsigned int *boundary_type, Vector<Real> *boundary_value)
{
	for (unsigned int index=0; index < this->num_vertex; index++){
		this->boundary_type[index]			=boundary_type[index];
		this->boundary_value[index]			=boundary_value[index];
	}
}

