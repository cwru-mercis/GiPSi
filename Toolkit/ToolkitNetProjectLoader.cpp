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
#ifdef GIPSINET

#include "ToolkitNetProjectLoader.h"

#include "collision.h"
#include "GiPSiException.h"
#include "logger.h"
#include "ToolkitCollisionDARLoader.h"
#include "ToolkitConnectorLoader.h"
#include "ToolkitObjectLoader.h"
#include "ToolkitShaderParamLoader.h"
#include "XMLDocumentBuilder.h"
#include "XMLDocument.h"
#include "XMLNode.h"
#include "OpenHapticsManager.h"
#include "probe.h"
#include "catheter.h"

using namespace GiPSiXMLWrapper;

/**
 * Load project Server.
 * 
 * @param argc Size of argument list.
 * @param argv Contains argument list.
 * @param nameService namse service ip and port number
 * @param sim Is set to loaded simulation kernel. 
 */
void ToolkitNetProjectLoader::LoadProjectServer(char* filename, SimulationKernel ** sim)
{
	try
	{
		XMLDocumentBuilder builder;
		XMLDocument * projectDoc = builder.Build(filename);
		XMLNode * rootNode = projectDoc->GetRootNode();

		// Create simulation kernel and load project file
		*sim = new SimulationKernel(rootNode, new ToolkitObjectLoader(new ToolkitShaderParamLoader()), new ToolkitConnectorLoader(), new ToolkitCollisionDARLoader());
		
		delete rootNode;
		delete projectDoc;
	}
	catch (...)
	{
		logger->Message("GiPSiNet Server Project Loader", "Aborting project initialization.", 0);
		return;
	}
	printf("loaded project file\n");
}

/**
 * Load project Haptic Manager Proxy Server.
 * 
 * @param argc Size of argument list.
 * @param argv Contains argument list.
 * @param nameService namse service ip and port number
 * @param sim Is set to loaded simulation kernel. 
 * @param hapticsManProxy Is set to instantiated haptics manager proxy.
 */
void ToolkitNetProjectLoader::LoadProjectHMPServer(int argc, char** argv, char* nameService, SimulationKernel ** sim, HapticsManagerProxy ** hapticsManProxy)
{
#ifdef GIPSI_HAPTICS_ENABLED
	try 
	{
		// First initialize the ORB, that will remove some arguments...
		argc = 3;  		
		//argv[0] = "Application";
		argv[1] = "-ORBInitRef"; 
		argv[2] = nameService;//"NameService=corbaloc:iiop:129.22.151.97:1234567/NameService";

		// Create Haptic manager proxy
		StartGiPSiNetHaptics(argc, argv, false);				
		(*hapticsManProxy) = OpenHapticsManagerProxy::GetOpenHapticsManagerProxy("OpenHapticsManager");

		/********************************************/
		/*   Setup and Start Haptics Manager Proxy	*/
		/********************************************/
		//Register All HIO to HapticsManagerProxy
		
		for(int j=0;j<(*sim)->num_object;j++)
		{				
			// attach the phantom haptic interface
			if (strcmp((*sim)->object[j]->GetType(), "PHIO") == 0)
			{
				RigidProbeHIO *rp = (RigidProbeHIO*)((*sim)->object[j]);
				PhantomHapticInterfaceProxy *phip = new PhantomHapticInterfaceProxy("Phantom");	
				rp->Attach((PhantomHapticInterface*)phip);
				printf("Register PHIP to PHIO %s\n",(*sim)->object[j]->GetName());
				(*hapticsManProxy)->RegisterHIO(rp);	// no need to use for gipsinet, this function just increas the number of HI in HM
			}
			// attach the mouse haptic interface
			if (strcmp((*sim)->object[j]->GetType(), "MHIO") == 0)
			{
				CatheterHIO *cat = (CatheterHIO*)((*sim)->object[j]);				
				MouseHapticInterfaceProxy *mhip = new MouseHapticInterfaceProxy("Mouse");	
				cat->Attach((MouseHapticInterface*)mhip);
				printf("Register MouseIP to MouseO %s\n",(*sim)->object[j]->GetName());
				(*hapticsManProxy)->RegisterHIO(cat);	// no need to use for gipsinet, this function just increas the number of HI in HM
			}
		}			
	}
	catch (CORBA::Exception &e) 
	{
		cerr << "CORBA exception raised!" << endl;
		cerr << e._name() << endl;
		logger->Message("GiPSiNet Server Project Loader", "Aborting Haptic manager proxy initialization.", 0);
		return;
	}
#endif
}

/**
 * Load project Visualization Engine Proxy Server.
 * 
 * @param argc Size of argument list.
 * @param argv Contains argument list.
 * @param nameService namse service ip and port number
 * @param sim Is set to loaded simulation kernel.
 * @param visEngineProxy Is set to loaded visualization engine proxy. 
 */
void ToolkitNetProjectLoader::LoadProjectVEPServer(int argc, char** argv, char* nameService, SimulationKernel ** sim, VisualizationEngineProxy ** visEngineProxy)
{
	try 
	{
		// First initialize the ORB, that will remove some arguments...
		argc = 3;    
		//argv[0] = "Application";
		argv[1] = "-ORBInitRef"; 		
		argv[2] = nameService;//"NameService=corbaloc:iiop:129.22.151.97:1234567/NameService";

		// Create visualization engine proxy
		StartGiPSiNetVisualization(argc, argv, false);
		*visEngineProxy = new VisualizationEngineProxy((*sim)->GetDisplayBufferHead(), "GiPSiNetVisualization");				
	}	
	catch (CORBA::Exception &e) 
	{
		cerr << "CORBA exception raised! in VE" << endl;
		cerr << e._name() << endl;
		logger->Message("GiPSiNet Server Project Loader", "Aborting VE proxy initialization.", 0);
		return;
	}
}

/**
 * Load project Client.
 * 
 * @param argc Size of argument list.
 * @param argv Contains argument list.
 * @param nameService namse service ip and port number
 * @param simProxy Is set to loaded simulation kernel proxy.
 * @param visEngine Is set to loaded visualization engine.
 * @param hapticsMan Is set to instantiated haptics manager.
 */
void ToolkitNetProjectLoader::LoadProjectClient(int argc, char** argv, char* nameService, SimulationKernelProxy ** simProxy, VisualizationEngine ** visEngine, HapticsManager ** hapticsMan)
{
	try
	{
		XMLDocumentBuilder builder;
		XMLDocument * projectDoc = builder.Build(argv[1]);
		XMLNode * rootNode = projectDoc->GetRootNode();
		// Create simulation kernel proxy		
		*simProxy = new SimulationKernelProxy(rootNode, new ToolkitObjectLoader(new ToolkitShaderParamLoader()), new ToolkitConnectorLoader(), new ToolkitCollisionDARLoader(), new Collision());		
			
		XMLNodeList * rootChildren = rootNode->GetChildren();
		XMLNode * visualizationNode = rootChildren->GetNode("visualization");
		// Create visualizaiton engine
		*visEngine = new GlutVisualizationEngine(argc, argv, (*simProxy)->GetDisplayBufferHead(), visualizationNode, 200, 200, 800, 600);			

		delete visualizationNode;
		delete rootChildren;
		delete rootNode;
		delete projectDoc;
	}
	catch (...)
	{
		logger->Message("GiPSiNet Client Project Loader", "Aborting project initialization.", 0);
		return;
	}

#ifdef GIPSI_HAPTICS_ENABLED
	try {			
		// First initialize the ORB, that will remove some arguments...
		argc = 3;    
		//argv[0] = "Application";
		argv[1] = "-ORBInitRef"; 
		argv[2] = nameService;//"NameService=corbaloc:iiop:129.22.151.97:1234567/NameService";
		
		// Create Haptic manager service
		StartGiPSiNetHaptics(argc, argv, true);
		
		// Setup and Start Haptics Manager					
		*hapticsMan = (HapticsManager*) OpenHapticsManager::GetHapticsManager();

		/****************************************/
		/*   Setup and Start Haptics Manager	*/
		/****************************************/		
		int status = (*hapticsMan)->StartHapticsThread();		                                                               
		if (status != 0) {			
			printf("ERROR: Could not launch haptics manager thread!\n");
			perror("pthread_create");
			exit(0);
		}		
	
		CreatePhantomHapticInterfaceServant((PhantomHapticInterface*)(*hapticsMan)->GetHapticInterface(1), "Phantom");				
		CreateMouseHapticInterfaceServant((MouseHapticInterface*)(*hapticsMan)->GetHapticInterface(101), "Mouse");				
		CreateOpenHapticsManagerServant("OpenHapticsManager");		
	}
	catch (CORBA::Exception &e) 
	{
		cerr << "CORBA exception raised!" << endl;
		cerr << e._name() << endl;
		logger->Message("GiPSiNet Client Project Loader - CORBA", "Aborting Haptic manager initialization.", 0);
		return;
	}
#endif

	try {		
		// First initialize the ORB, that will remove some arguments...
		argc = 3;  		
		//argv[0] = "Application";
		argv[1] = "-ORBInitRef"; 
		argv[2] = nameService;//"NameService=corbaloc:iiop:129.22.151.97:1234567/NameService";
		
		// Create visualization engine service
		StartGiPSiNetVisualization(argc, argv, true);
		CreateSKPServant((*simProxy), "GiPSiNetVisualization");			
	}
	catch (CORBA::Exception &e) 
	{
		cerr << "CORBA exception raised!" << endl;
		cerr << e._name() << endl;
		logger->Message("GiPSiNet Client Project Loader - CORBA", "Aborting visualization engine initialization.", 0);
		return;
	}
}

/**
 * Load project Haptic Manager Client.
 * 
 * @param argc Size of argument list.
 * @param argv Contains argument list.
 * @param nameService namse service ip and port number
 * @param hapticsMan Is set to instantiated haptics manager.
 */
void ToolkitNetProjectLoader::LoadProjectHMClient(int argc, char** argv, char* nameService, HapticsManager ** hapticsMan)
{
#ifdef GIPSI_HAPTICS_ENABLED
	try {					
		// First initialize the ORB, that will remove some arguments...
		argc = 3;    
		//argv[0] = "Application";
		argv[1] = "-ORBInitRef"; 
		argv[2] = nameService;//"NameService=corbaloc:iiop:129.22.151.97:1234567/NameService";
		
		// Create Haptic manager service
		StartGiPSiNetHaptics(argc, argv, true);
		// Setup and Start Haptics Manager					
		*hapticsMan = (HapticsManager*) OpenHapticsManager::GetHapticsManager();

		/****************************************/
		/*   Setup and Start Haptics Manager	*/
		/****************************************/		
		int status = (*hapticsMan)->StartHapticsThread();		                                                               
		if (status != 0) {			
			printf("ERROR: Could not launch haptics manager thread!\n");
			perror("pthread_create");
			exit(0);
		}		
	
		CreatePhantomHapticInterfaceServant((PhantomHapticInterface*)(*hapticsMan)->GetHapticInterface(1), "Phantom");		
		CreateMouseHapticInterfaceServant((MouseHapticInterface*)(*hapticsMan)->GetHapticInterface(101), "Mouse");				
		CreateOpenHapticsManagerServant("OpenHapticsManager");
	}
	catch (CORBA::Exception &e) 
	{
		cerr << "CORBA exception raised!" << endl;
		cerr << e._name() << endl;
		logger->Message("GiPSiNet Client Project Loader - CORBA", "Aborting Haptic manager initialization.", 0);
		return;
	}
#endif
}

/**
 * Load project VE Client.
 * 
 * @param argc Size of argument list.
 * @param argv Contains argument list.
 * @param nameService namse service ip and port number
 * @param simProxy Is set to loaded simulation kernel proxy.
 * @param visEngine Is set to loaded visualization engine. 
 */
void ToolkitNetProjectLoader::LoadProjectVEClient(int argc, char** argv, char* nameService, SimulationKernelProxy ** simProxy, VisualizationEngine ** visEngine)
{
	try
	{
		XMLDocumentBuilder builder;
		XMLDocument * projectDoc = builder.Build(argv[1]);
		XMLNode * rootNode = projectDoc->GetRootNode();
		// Create simulation kernel proxy		
		*simProxy = new SimulationKernelProxy(rootNode, new ToolkitObjectLoader(new ToolkitShaderParamLoader()), new ToolkitConnectorLoader(), new ToolkitCollisionDARLoader(), new Collision());		
			
		XMLNodeList * rootChildren = rootNode->GetChildren();
		XMLNode * visualizationNode = rootChildren->GetNode("visualization");
		// Create visualizaiton engine
		*visEngine = new GlutVisualizationEngine(argc, argv, (*simProxy)->GetDisplayBufferHead(), visualizationNode, 200, 200, 800, 600);			

		delete visualizationNode;
		delete rootChildren;
		delete rootNode;
		delete projectDoc;
	}
	catch (...)
	{
		logger->Message("GiPSiNet Client Project Loader", "Aborting project initialization.", 0);
		return;
	}

	try {
		// First initialize the ORB, that will remove some arguments...
		argc = 3;    
		//argv[0] = "Application";
		argv[1] = "-ORBInitRef"; 
		argv[2] = nameService;//"NameService=corbaloc:iiop:129.22.151.97:1234567/NameService";
		
		// Create visualization engine service
		StartGiPSiNetVisualization(argc, argv, true);
		CreateSKPServant((*simProxy), "GiPSiNetVisualization");			
	}
	catch (CORBA::Exception &e) 
	{
		cerr << "CORBA exception raised!" << endl;
		cerr << e._name() << endl;
		logger->Message("GiPSiNet Client Project Loader - CORBA", "Aborting visualization engine initialization.", 0);
		return;
	}
}
#endif