/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Project Loader Definition (ProjectLoader.h).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	PROJECTLOADER.H v0.0
////
////	Project Loader
////
////////////////////////////////////////////////////////////////

#ifndef _PROJECT_LOADER_H_
#define _PROJECT_LOADER_H_

#include "simulator.h"
#include "GiPSiVisualizationEngine.h"
#include "GiPSiHapticsManager.h"

class ProjectLoader
{
public:
	/**
	 * Load project.
	 * 
	 * @param argc Size of argument list.
	 * @param argv Contains argument list.
	 * @param Sim Is set to loaded simulation kernel.
	 * @param visEngine Is set to loaded visualization engine.
	 * @param hapticsMan Is set to instantiated haptics manager.
	 */
	virtual void LoadProject(int argc, char* argv[], SimulationKernel ** Sim, VisualizationEngine ** visEngine, HapticsManager ** hapticsMan) = 0;
};

#endif
