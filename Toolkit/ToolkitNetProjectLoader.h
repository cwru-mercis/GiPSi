/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit Project Loader Definition (ToolkitProjectLoader.h).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	TOOLKITNETPROJECTLOADER.H v0.0
////
////	Toolkit Net Project Loader
////
////////////////////////////////////////////////////////////////

#ifndef _TOOLKIT_NET_PROJECT_LOADER_H_
#define _TOOLKIT_NET_PROJECT_LOADER_H_

#ifdef GIPSINET

#include "HapticsI.h"
#include "VisualizationI.h"
#include "GiPSiNetHaptics.h"
#include "GiPSiNetVisualization.h"

#include "GlutVisualizationEngine.h"
#include "simulator.h"

class ToolkitNetProjectLoader
{
public:
	ToolkitNetProjectLoader() {}

	void LoadProjectServer(char* filename, SimulationKernel ** Sim);	
	void LoadProjectHMPServer(int argc, char** argv, char* nameService, SimulationKernel ** Sim, HapticsManagerProxy ** hapticsManProxy);	
	void LoadProjectVEPServer(int argc, char** argv, char* nameService, SimulationKernel ** Sim, VisualizationEngineProxy ** visEngineProxy);	
	void LoadProjectClient(int argc, char** argv, char* nameService, SimulationKernelProxy ** simProxy, VisualizationEngine ** visEngine, HapticsManager ** hapticsMan);	
	void LoadProjectHMClient(int argc, char** argv, char* nameService, HapticsManager ** hapticsMan);	
	void LoadProjectVEClient(int argc, char** argv, char* nameService, SimulationKernelProxy ** simProxy, VisualizationEngine ** visEngine);	

};

#endif
#endif
