/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit Project Loader Implementation (ToolkitProjectLoader.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao, Suriya Natsupakpong.
*/

////	TOOLKITPROJECTLOADER.CPP v0.0
////
////	Toolkit Project Loader
////
////////////////////////////////////////////////////////////////

#include "collision.h"
#include "GiPSiException.h"
#include "logger.h"
#include "ToolkitCollisionDARLoader.h"
#include "ToolkitConnectorLoader.h"
#include "ToolkitObjectLoader.h"
#include "ToolkitProjectLoader.h"
#include "ToolkitShaderParamLoader.h"
#include "XMLDocumentBuilder.h"
#include "XMLDocument.h"
#include "XMLNode.h"
#include "OpenHapticsManager.h"
#include "probe.h"
#include "catheter.h"


using namespace GiPSiXMLWrapper;

/**
 * Load project.
 * 
 * @param argc Size of argument list.
 * @param argv Contains argument list.
 * @param Sim Is set to loaded simulation kernel.
 * @param visEngine Is set to loaded visualization engine.
 * @param hapticsMan Is set to instantiated haptics manager.
 */
void ToolkitProjectLoader::LoadProject(int argc, char* argv[], SimulationKernel ** sim, VisualizationEngine ** visEngine, HapticsManager ** hapticsMan)
{
	try
	{
		XMLDocumentBuilder builder;
		XMLDocument * projectDoc = builder.Build(argv[1]);
		XMLNode * rootNode = projectDoc->GetRootNode();

		*sim = new SimulationKernel(rootNode, new ToolkitObjectLoader(new ToolkitShaderParamLoader()), new ToolkitConnectorLoader(), new ToolkitCollisionDARLoader());
		XMLNodeList * rootChildren = rootNode->GetChildren();
		XMLNode * visualizationNode = rootChildren->GetNode("visualization");
		*visEngine = new GlutVisualizationEngine(argc, argv, (*sim)->GetDisplayBufferHead(), visualizationNode, 200, 200, 800, 600);
		delete visualizationNode;
		delete rootChildren;
		delete rootNode;
		delete projectDoc;

#ifdef GIPSI_HAPTICS_ENABLED
		*hapticsMan = (HapticsManager*) OpenHapticsManager::GetHapticsManager();
		//Register All HIO to HapticsManager
		//Attach haptics interface to haptics interface object by get the haptic interface pointer by id
		for(int j=0;j<(*sim)->num_object;j++)
		{
			// attach the phantom haptic interface
			if (strcmp((*sim)->object[j]->GetType(), "PHIO") == 0)
			{
				RigidProbeHIO *rp = (RigidProbeHIO*)((*sim)->object[j]);				
				rp->Attach((*hapticsMan)->GetHapticInterface(rp->GetHIIdentifier()));				
				//(*hapticsMan)->RegisterHIO(rp);				
				
			}
			// attach the mouse haptic interface
			if (strcmp((*sim)->object[j]->GetType(), "MHIO") == 0)
			{
				CatheterHIO *cat = (CatheterHIO*)((*sim)->object[j]);				
				cat->Attach((*hapticsMan)->GetHapticInterface(cat->GetHIIdentifier()));
				//(*hapticsMan)->RegisterHIO(rp);				
			}
		}				
#endif

	}
	catch (...)
	{
		logger->Message("Project Loader", "Aborting project initialization.", 0);
		return;
	}
}
