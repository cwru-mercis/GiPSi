/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Main Simulation Kernel Implementation (simulator.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	Simulator.CPP v0.0.9
////
////	Main Simulation Kernel module 
////
////////////////////////////////////////////////////////////////

// Standard includes
#include "algebra.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <crtdbg.h>

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "GiPSiHaptics.h"
#include "GlutVisualizationEngine.h"
#include "simulator.h"
#include "hapticsman.h"
#include "msd.h"
#include "fem.h"
#include "bioe.h"
#include "timing.h"



////////////////////////////////////////////////////////////////
//
//	Simulate
//
//		Main simulation thread
//
void *Simulate(void *arg)
{
	int					tc = 0;
	int					status;
	SimulationKernel	*sim;
	double				time;

	sim = (SimulationKernel *) arg;

    // Detach itself
    status = pthread_detach(pthread_self());
	
	while(1) {
		if(sim->IsRunning()) {
			start_timer(1);

			sim->Simulate();

			time = get_timer(1);

			printf("%lf ms\n", time);
		}
		sim->Display();
	}
    
	return NULL;
}


////////////////////////////////////////////////////////////////
//
//	SimulationKernel::Display
//
//		Displays each object
//
void SimulationKernel::Display(void)
{
	for(int i=0; i<num_object; i++) {
		object[i]->Display();
	}
}


////////////////////////////////////////////////////////////////
//
//	SimulationKernel::Simulate
//
//		Simulates the system
//
inline void SimulationKernel::Simulate(void)
{
	int		i;

	// NOTE: Right now all objects integrate themselves
	for(i=0; i<num_object; i++) {
		object[i]->Simulate();
	}
	for(i=0; i<num_connector; i++) {
		connector[i]->process();
	}

	time += timestep;
}



////////////////////////////////////////////////////////////////
//
//	Main
//
//		Simply the main of GiPSi Simulator
//
int main(int argc, char* argv[])
{
	SimulationKernel	Sim;
	int					status;

#ifdef _DETECT_MEMORY_LEAKS
#ifdef _WIN32
	int tmp=_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmp=tmp|_CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag(tmp);
#endif
#endif

	/****************************************/
	/*   Setup Simulation					*/
	/****************************************/
	Sim.LoadProject(argv[1]);
	
	status = pthread_create(Sim.GetSimThread(), NULL, Simulate, &Sim);
	if (status != 0) {
	    printf("ERROR: Could not launch simulation thread!\n");
	    exit(0);
	}
	
	init_timers();

#ifdef GIPSI_HAPTICS_ENABLED
	/****************************************/
	/*   Setup and Start Haptics Manager	*/
	/****************************************/
	HapticsManager_Initialize();
	pthread_t hapticsth;
	status = pthread_create(&hapticsth, NULL, HapticsManager_Start, NULL);
	if (status != 0) {
	    printf("ERROR: Could not launch haptics manager thread!\n");
	    exit(0);
	}
#endif


	/****************************************/
	/*   Setup and Start GUI				*/
	/****************************************/
	GlutVisualizationEngine *visEngine =
		new GlutVisualizationEngine(argc, argv, &Sim, 200, 200, 800, 600);
	visEngine->Start();

	return 1;
}
