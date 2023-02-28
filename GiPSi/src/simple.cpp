/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Simple Test Object Model Implementation (simple.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	SIMPLE.cpp	v0.0
////
////	Simple test object class.
////
////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "simple.h"
#include "errors.h"

#define		DELIM 10

////////////////////////////////////////////////////////////////
//
//	SimpleTestObject::Load()
//
//		Reads in .obj file
//
void SimpleTestObject::Load(char *filename)   
{
	TriSurface	*geom;
  
	/****************************************/
	/*	Set up geometry						*/
	/****************************************/
	if((geometry = new TriSurface(initialcolor[0],initialcolor[1],initialcolor[2],initialcolor[3])) == NULL) {
		printf("ERROR: Could not allocate geometry!\n");
		exit(0);
	}
	geom = (TriSurface *) geometry;

	geom->Load(filename);
	// should we replace this with the following?
	// LoadData *data;
	// data=LoadObj(filename);
	// geom->Load(data);

	geom->calcNormals();

	/****************************************/
	/*	Set up states						*/
	/****************************************/
	state.t=0.0;

	/****************************************/
	/*	Set up boundary						*/
	/****************************************/
	//   SimpleTestObject Model does not have a Boundary Implemented
	
	/****************************************/
	/*	Set up integrator					*/
	/****************************************/
	// MENTAL NOTE: move this to somewhere logical asap!
	integrator = new ERKHeun3<SimpleTestObject>(*this);

}




////////////////////////////////////////////////////////////////
//
//	SimpleTestObject::AccumState()
//
//		Computes y(t+h) = y(t) + h * f(..)
//
//		where	y(t+h)	= new_state
//				y(t)	= state
//				f(..)	= deriv
//
inline void SimpleTestObject::AccumState(State &new_state, const State &state, const State &deriv, const Real &h)
{
	new_state.t=deriv.t*h+state.t;
}




////////////////////////////////////////////////////////////////
//
//	SimpleTestObject::DerivState()
//
//		Computes the derivatives of state variables. This is simply 
//		the f() evaluation where f() = dy/dt.
//
//
inline void SimpleTestObject::DerivState(State &deriv, State &state)
{
	deriv.t	=	1.0;
}



////////////////////////////////////////////////////////////////
//
//	SimpleTestObject::AllocState()
//
//		Allocates the memory for the integrator's local state 
//		members
//
inline void SimpleTestObject::AllocState(State &s)
{
}



////////////////////////////////////////////////////////////////
//
//	SimpleTestObject::Simulate()
//
//		Simulates the SimpleTestObject object by a single timestep
//
//
void SimpleTestObject::Simulate(void)
{
	integrator->Integrate(*this, timestep);
	time+=timestep;

	//We alse need to update the Boundary from states 
	//   so that we will have the correct Boundary definition for boundary calculations
	State2Bound();

}



////////////////////////////////////////////////////////////////
//
//	SimpleTestObject::Geom2State()
//
//		Copies geometry to state
//
//
void SimpleTestObject::Geom2State(void)
{
}



////////////////////////////////////////////////////////////////
//
//	SimpleTestObject::State2Geom()
//
//		Copies state to geometry
//
//
inline void SimpleTestObject::State2Geom(void)
{
}

////////////////////////////////////////////////////////////////
//
//	SimpleTestObject::State2Geom()
//
//		Copies state to boundary
//
//
inline void SimpleTestObject::State2Bound(void)
{
	printf("No SimpleTestObject Boundary implemented...\n");
}


////////////////////////////////////////////////////////////////
//
//	SimpleTestObject::Display()
//
//		Displays the SimpleTestObject mesh
//
//
void SimpleTestObject::Display(void)
{
	TriSurfaceDisplayManager	*tmp = (TriSurfaceDisplayManager	*) displayMngr;

	// Copy the state into the geometry
	State2Geom();

	displayMngr->Display();
}

////////////////////////////////////////////////////////////////
//
//	SimpleTestObject::SetupDisplay()
//
//		Sets up the display of the the SimpleTestObject
//
//
void SimpleTestObject::SetupDisplay(void) 
{
	DisplayHeader	header;

	header.objType				= GIPSI_DRAW_TRIANGLE;
	header.polyMode				= GIPSI_POLYGON_FILL;
	header.shadeMode			= GIPSI_SHADE_SMOOTH;
	header.dataType				= 0x0E;					// TEXTURE ON, NORMAL ON, RGBA

	displayMngr = (TriSurfaceDisplayManager *) new TriSurfaceDisplayManager((TriSurface *) geometry, header);
}
