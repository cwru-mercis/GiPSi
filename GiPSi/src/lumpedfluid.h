/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for Lumped Fluid Model (lumpedfluid.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	lumpedfluid.h v0.0
////
////	Lumped Fluid Model
////
////////////////////////////////////////////////////////////////

#ifndef _LUMPEDFLUID_H
#define _LUMPEDFLUID_H

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"

class LumpedFluidObject: public SIMObject {
public:
	typedef struct {
		Vector<Real>	*pos;		// position of nodes		[m]
		unsigned int	size;
		Real			Vf;
	} State;
	
	// Constructor
	LumpedFluidObject(	char* name,
						Real *RGBAcolor, 
						Real time			=0.0,
						Real timestep		=0.01,
						Real _Pi			=0.8,
						Real _Po			=1.4,
						Real _Pfo			=0.8,
						Real _K				=100.0,
						Real _B				=0.0,
						Real _Ri			=1.0,
						Real _Ro			=1.0			): 
							SIMObject(name, time, timestep),
							Pi(_Pi),Po(_Po),Pfo(_Pfo),K(_K),B(_B),Ri(_Ri),Ro(_Ro),
							ChamberGeometry(RGBAcolor[0],RGBAcolor[1],RGBAcolor[2],RGBAcolor[3])
	{ }

	void				init(unsigned int num_node);   // Will be called by loader

	// Loaders
	void				Load(char *filename);

	// Integrator interface
	void				AccumState(State &new_state, const State &state, const State &deriv, const Real &h);
	void				DerivState(State &deriv, State &state);
	void				AllocState(State &s);
	void				Simulate(void);

	// Get and Set interfaces for the Boundary and the Domain
	Real				GetPressure(unsigned int index);
	void				GetPressure(Real *BPres);	

	// Display functions
	void				Display(void);
	void				SetupDisplay(void) {
		DisplayHeader	header;

		header.objType				= GIPSI_DRAW_TRIANGLE;
		header.polyMode				= GIPSI_POLYGON_FILL;
		header.shadeMode			= GIPSI_SHADE_FLAT;
		header.dataType				= 0x06;

		displayMngr = new TriSurfaceDisplayManager((TriSurface *) geometry, header);
	}
	void				Geom2State(void);
	void				State2Geom(void);
	void				State2Bound(void);

	State&				GetState(void)	{	return	state; }	// We need this to be able to access state in 
																//	integrator as state is protected

protected:
	State			state;
	unsigned int	num_node;
	Real			Pi;
	Real			Po;
	Real			Pfo;
	Real			K;
	Real			B;
	Real			Ri;
	Real			Ro;
	Real			Pf,Vm;
	Real			VfminusVm_last;
	TriSurface		ChamberGeometry;

	Integrator<LumpedFluidObject>		*integrator;

};


// FluidBoundary boundary class
class LumpedFluidBoundary : public Boundary {
public:
	unsigned int				*global_id;
	unsigned int				*facetovertexids;
	// Assume that only velocity boundary conditions (boundary_type=0) or free surface (boundary_type=1)
	// If velocity BC, (b_t==0) boundary_value is the nodal velocity
	// If free surface (b_t==1) ignore boundary_value
	unsigned int				*boundary_type;
	Vector<Real>				*boundary_value;

	// Pointer to the parent object
	LumpedFluidObject			*Object;

	// Initializer
	void		init(LumpedFluidObject *ParentObj, unsigned int num_vertex, unsigned int num_face);
	void		init(LumpedFluidObject *ParentObj, unsigned int num_vertex, unsigned int num_face,
							Vertex *vertexlist, Triangle *facelist);

	// Get and Set interfaces
	Real		GetPressure(unsigned int index);
	void		GetPressure(Real *BPres);	

	void		Set(int index, unsigned int boundary_type, Vector<Real> boundary_value);
	void		Set(unsigned int *boundary_type, Vector<Real> *boundary_value);

protected:
 
};


#endif