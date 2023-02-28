/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Core GiPSi API Definition (GiPSiAPI.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	GiPSiAPI.h v0.1.0
////
////    Definitions for the Core GiPSi API
////	1)	Defines the base class for the simulation objects
////    2)	Defines the base class for connectors
////    3)	Defines the base class for integrators
////
////////////////////////////////////////////////////////////////


#ifndef _GiPSiAPI_H
#define _GiPSiAPI_H

#include "GiPSiGeometry.h"
#include "GiPSiDisplay.h"
#include "GiPSiHaptics.h"

///****************************************************************
// *						SIMULATION OBJECT					  *  
// ****************************************************************

// Base class for a simulatable object
class SIMObject {
public:
	DisplayManager	*displayMngr;

	// Constructors
	SIMObject(	char* name, 
				Real time		= 0.0, 
				Real timestep	= 0.01): 
				time(time), timestep(timestep), 
				geometry(NULL), boundary(NULL), domain(NULL) { 

		this->name = new char[strlen(name)+1];

		sprintf(this->name,"%s",name); 
	}

	// Access to private data
	char*			GetName(void)		const { return name; }
	void			SetName(char *newname)	  { delete[] name;
												name = new char[strlen(newname)+1];
												sprintf(this->name,"%s",newname); }
	Real			GetTimestep(void)	const { return timestep; }
	void			SetTimestep(Real dt)	{ timestep = dt; }
	Real			GetTime(void)		const { return time; }
	void			SetTime(Real t)			{ time = t; }
	Geometry*		GetGeometryPtr(void)	const { return geometry; }
	Boundary*		GetBoundaryPtr(void)	const { return boundary; }
	Domain*			GetDomainPtr(void)		const { return domain; }

	// Loader
	virtual void	Load(char* filename) {}

	// Display functions
    virtual void	Display(void) {}
    virtual void	SetupDisplay(void) {}

	// Haptics API
	virtual int		ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model) {return 0; }

	// Simulation API
	virtual void	Simulate(void) {}

protected:
	char			*name;				// Name
	Geometry		*geometry;			// Display Geometry
	Boundary		*boundary;			// Boundary Geometry
	Domain			*domain;			// Domain Geometry
	Real			time;				// Local time of the object
	Real			timestep;			// The local timestep
};



// ****************************************************************
// *						BASE CONNECTOR						  *  
// ****************************************************************
class Connector {
public:

	virtual void	process(void) {}

protected:
	Boundary	* boundaries;
	Domain		* domains;

};


// ****************************************************************
// *						BASE INTEGRATOR CLASS				  *  
// ****************************************************************

// The base Integrator Class
template <class S>
class Integrator {
public:
  typedef typename				S::State State;

  virtual void				Integrate(S &system, Real h) {}
};



#endif