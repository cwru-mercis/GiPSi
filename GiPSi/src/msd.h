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

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	MSD.H v0.1.0
////
////	Defines:
////		Spring		-	Spring class
////		MSDObject	-	Mass-Spring-Damper (MSD) class.
////
////////////////////////////////////////////////////////////////

#ifndef _MSD_H
#define _MSD_H

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"

// The base Spring Class
class Spring : public Element {
public:
	unsigned int				node[2];
	Vector<Real>				dir;		// Direction of the spring
	Real						L0;			// Rest length
	Real						k;			// Stiffness of the string
	Real						d;			// Internal damping of the spring

	// Constructors
	Spring() : dir(3) { }

	Real			length() { return dir.length(); }
};


// Base class for Mass Spring Damper Object
class MSDObject: public SIMObject {
public:
	// State info for the MSD
	typedef struct {
		Vector<Real>			*POS;		// Global position vector
		Vector<Real>			*pos;		// position of nodes		[cm]
		Vector<Real>			*VEL;		// Global velocity vector
		Vector<Real>			*vel;		// velocity of nodes		[cm/s]
		unsigned int			size;
	} State;


	// Constructors
	MSDObject(	char* name,						
				Real *RGBAcolor,
				Real time		= 0.0, 
				Real timestep	= 0.01,
				Real K			= 10.0, 
				Real D			= 0.0, 
				Real mass		= 1.0,
				Real g			= 10.0): 
				SIMObject(name, time, timestep), K(K), D(D), defaultMass(mass), g(g) { 
		for (int i=0; i<4; i++) initialcolor[i]=RGBAcolor[i];
	}
    
	// Loaders
	void				Load(char *filename);

	// Force calculators
	void				UpdateForces(State	&state);	
	void				GravityForce(void);
	void				SpringForce(void);
	void				DampingForce(void);
	void				ExternalForce(void);

	// Integrator interface
	void				AccumState(State &new_state, const State &state, const State &deriv, const Real &h);
	void				DerivState(State &deriv, State &state);
	void				AllocState(State &s);
	void				Simulate(void);

	// Display functions
	void				Display(void);
    void				SetupDisplay(void) {
							DisplayHeader	header;

							header.objType				= GIPSI_DRAW_TRIANGLE;
							header.polyMode				= GIPSI_POLYGON_OUTLINE;
							header.shadeMode			= GIPSI_SHADE_SMOOTH;
							header.dataType				= 0x06;

							displayMngr = (TriSurfaceDisplayManager *) new TriSurfaceDisplayManager((TriSurface *) geometry, header);
						}

	void				Geom2State(void);
	void				State2Geom(void);
	void				State2Bound(void);

	State&				GetState(void)	{	return	state; }

protected:
	const					Real K;			// Global stiffness of the mesh			[g/s2]
	const					Real D;			// Global damping of the mesh			[g/s]
	const					Real g;			// Local copy of the gravity			[cm/s2]
	Real					defaultMass;	// The default mass value of the nodes	[g]

	State						state;
	Vector<Real>				*force;		// Force vector		[g cm/s2 = 10 N]
	Real						*mass;		// Mass vector		[g]
	Spring						*spring;
    unsigned int				num_spring;

	Integrator<MSDObject>		*integrator;

	Real						initialcolor[4];// Initial color specified in constructor
												//   It is used in the Load() function  

};



#endif
