/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Start Definition (GiPSiStart.h).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	GIPSISTART.H v0.0
////
////	GiPSi Start
////
////////////////////////////////////////////////////////////////

#ifndef _GIPSI_START_H_
#define _GIPSI_START_H_

#ifdef GIPSINET
#include "HapticsI.h"
#include "VisualizationI.h"
#include "GiPSiNetHaptics.h"
#include "GiPSiNetVisualization.h"
#endif

#include "GiPSiVisualizationEngine.h"
#include "GiPSiHapticsManager.h"
#include "logger.h"
#include "simulator.h"
#include "timing.h"

#include "GlutVisualizationEngine.h"
#include "collision.h"
#include "ToolkitCollisionDARLoader.h"
#include "ToolkitConnectorLoader.h"
#include "ToolkitObjectLoader.h"
#include "ToolkitProjectLoader.h"
#include "ToolkitShaderParamLoader.h"

#include "XMLDocumentBuilder.h"
#include "XMLDocument.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

/**
 * Begin simulation using the input objects.
 * 
 * @param Sim SimulationKernel to run.
 * @param visEngine VisualizationEngine to run.
 * @param hapticsMan HapticsManager to run.
 */
void GiPSiStart(SimulationKernel * Sim, VisualizationEngine * visEngine, HapticsManager * hapticsMan)
{
	if (!Sim)
	{
		logger->Error("GiPSiStart", "Recieved empty SimulationKernel pointer.");
		exit(0);
	}
	if (!visEngine)
	{
		logger->Error("GiPSiStart", "Recieved empty VisualizationEngine pointer.");
		exit(0);
	}

	int status = pthread_create(Sim->GetSimThread(), NULL, StartSimulationThread, (void*)Sim);
	if (status)
	{
		logger->Error("GiPSiStart", "Could not launch simulation thread.");
		perror("pthread_create");
	    exit(0);
	}
	
	init_timers();

#ifdef GIPSI_HAPTICS_ENABLED
	/****************************************/
	/*   Setup and Start Haptics Manager	*/
	/****************************************/
	status = hapticsMan->StartHapticsThread();
	if (status != 0) {
	    printf("ERROR: Could not launch haptics manager thread!\n");
	    exit(0);
	}
#endif


	/****************************************/
	/*   Setup and Start GUI				*/
	/****************************************/
	visEngine->Start(Sim);

	// Destroy project logger
	if (logger)
	{
		delete logger;
		logger = NULL;
	}
	if (Sim)
	{
		delete Sim;
		Sim = NULL;
	}
	if (visEngine)
	{
		delete visEngine;
		visEngine = NULL;
	}
#ifdef GIPSI_HAPTICS_ENABLED
	if (hapticsMan)
	{
		hapticsMan->EndHapticsThread();
		delete hapticsMan;
		hapticsMan = NULL;
	}
#endif

	return;
}

#endif