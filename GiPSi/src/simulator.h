/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Main Simulation Kernel Definition (simulator.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	SIMULATOR.H v0.1.0
////
////	Defines the simulation kernel
////
////////////////////////////////////////////////////////////////


#ifndef _SIMULATOR_H
#define _SIMULATOR_H

#include "GiPSiAPI.h"
#include <pthread.h>

#define MAX_SIMOBJ		64
#define	MAX_CONNECTOR	32


/****************************************************************
 *							SIMULATOR							*  
 ****************************************************************/

// The main Simulation Kernel
class SimulationKernel {
public:
	Real			g;					// Gravity (= 1.0 g*cm/s2)	
    SIMObject*		object[MAX_SIMOBJ];	// Object array
	int				num_object;			// Number of objects
	
	// Constructors
	SimulationKernel(	Real time = 0.0, 
						Real timestep = 0.01, 
						Real g = 1000.0) 
					:	time(time), timestep(timestep), 
						g(g), num_object(0), num_connector(0), RUN(false) {}

    int				LoadProject(char* filename);
	void			Display(void);
	void			Simulate(void);

	pthread_t*		GetSimThread(void)	{	return &simThread; }
	bool			IsRunning(void)		{	return RUN; }
	void			Run(void)			{	RUN = true; }
	void			Stop(void)			{	RUN = false; }

protected:
	char			*name;				// Name of the project
	Real			time;				// Global time
	Real			timestep;			// Global time step (= 0.01 s) 
	Connector*		connector[MAX_CONNECTOR];
	int				num_connector;		// Number of connectors
    DisplayArray	display;			// Global display
    pthread_t		simThread;			// The main simulation thread
	bool			RUN;

};

// Simulation thread
void *Simulate(void *arg);

#endif