/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSiNet Simulation Kernel Implementation (GiPSiSimKernel.cpp).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	GIPSINETSIMKERNEL.CPP v0.0
////
////	GiPSiNet Simulation Kernel
////
////////////////////////////////////////////////////////////////

#ifdef GIPSINET

#include <ace/streams.h>
#include <orbsvcs/CosNamingC.h>

#include "timing.h"
#include "logger.h"
#include "GiPSiNetSK_i.h"
#include "GlutVisualizationEngine.h"
#include "GiPSiNetVisualization.h"
#include "ToolkitNetProjectLoader.h"

CORBA::ORB_var GiPSiNetSK_orb;
PortableServer::POA_var GiPSiNetSK_poa;
CORBAGIPSINETSK_var GiPSiNetSK_var;
CORBAGIPSINETSK_i GiPSiNetSK_servant;
CosNaming::NamingContext_var GiPSiNetSK_Naming_context;
SimulationKernelProxy * simProxy = NULL;

/**
 * Begin simulation client using the input objects.
 * 
 * @param simProxy SimulationKernelProxy to run. 
 * @param visEngine VisualizationEngine to run.
 * @param hapticsMan HapticsManager to run.
 */
void GiPSiNetStartClient(SimulationKernelProxy * simProxy, VisualizationEngine * visEngine, HapticsManager * hapticsMan)
{	
	int status;

	if (!simProxy)
	{
		logger->Error("GiPSiStartClient", "Recieved empty SimulationKernel proxy pointer.");
		exit(0);
	}
	if (!visEngine)
	{
		logger->Error("GiPSiStartClient", "Recieved empty VisualizationEngine pointer.");
		exit(0);
	}

	/*
	status = pthread_create(simProxy->GetSimThread(), NULL, StartSimulationThread, (void*)SimProxy);
	if (status)
	{
		logger->Error("GiPSiStartClient", "Could not launch simulation thread.");
		perror("pthread_create");
	    exit(0);
	}	
	*/

	init_timers();
	
#ifdef GIPSI_HAPTICS_ENABLED
	// Start the GiPSiNet Haptics Manager Servant Thread	
	// This will call infinite loop of RunGiPSiNetHaptics()
	pthread_t GiPSiNetHapticsThread;
	status = pthread_create(&GiPSiNetHapticsThread, NULL, GiPSiNetHaptics_Start, NULL);
	if (status)
	{
		logger->Error("GiPSiStartClient", "Could not launch haptics manager thread.");
		perror("pthread_create");
		exit(0);
	}		
#endif

	// Start the GiPSiNet SKP-SEP Servant Thread	
	// This will call infinite loop of RunGiPSiNetVisualization()
	pthread_t GiPSiNetVEThread;
	status = pthread_create(&GiPSiNetVEThread, NULL, GiPSiNetVisualization_Start, NULL);
	if (status)
	{
		logger->Error("GiPSiStartClient", "Could not launch vitualization engine thread.");
		perror("pthread_create");
	    exit(0);
	}
		
	/****************************************/
	/*   Setup and Start GUI				*/
	/****************************************/
	visEngine->Start(simProxy);

	// Destroy project logger
	if (logger)
	{
		delete logger;
		logger = NULL;
	}
	if (simProxy)
	{
		delete simProxy;
		simProxy = NULL;
	}
	if (visEngine)
	{
		delete visEngine;
		visEngine = NULL;
	}
	return;
}

/**
 * Begin haptic manager client using the input objects.
 * 
 * @param hapticsMan HapticsManager to run.
 */
void GiPSiNetStartHapticClient(HapticsManager * hapticsMan)
{		
	RunGiPSiNetHaptics();
	if (hapticsMan)
	{		
		delete hapticsMan;
		hapticsMan = NULL;
	}
	return;
}

/**
 * Begin visualization engine client using the input objects.
 * 
 * @param simProxy SimulationKernelProxy to run. 
 * @param visEngine VisualizationEngine to run. 
 */
void GiPSiNetStartVEClient(int argc, char** argv, SimulationKernelProxy * simProxy, VisualizationEngine * visEngine)
{		
	int status;
	if (!simProxy)
	{
		logger->Error("GiPSiStartVEClient", "Recieved empty SimulationKernel proxy pointer.");
		exit(0);
	}
	if (!visEngine)
	{
		logger->Error("GiPSiStartVEClient", "Recieved empty VisualizationEngine pointer.");
		exit(0);
	}

	init_timers();
	
	// Start the GiPSiNet SKP-SEP Servant Thread	
	// This will call infinite loop of RunGiPSiNetVisualization()
	pthread_t GiPSiNetVEThread;
	status = pthread_create(&GiPSiNetVEThread, NULL, GiPSiNetVisualization_Start, NULL);
	if (status)
	{
		logger->Error("GiPSiStartVEClient", "Could not launch vitualization engine thread.");
		perror("pthread_create");
	    exit(0);
	}

	/****************************************/
	/*   Setup and Start GUI				*/
	/****************************************/
	visEngine->Start(simProxy);

	// Destroy project logger
	if (logger)
	{
		delete logger;
		logger = NULL;
	}
	if (simProxy)
	{
		delete simProxy;
		simProxy = NULL;
	}
	if (visEngine)
	{
		delete visEngine;
		visEngine = NULL;
	}

	return;
}

void StartGiPSiNetSK(int argc, char ** argv, bool IsServer)
{
	GiPSiNetSK_orb = CORBA::ORB_init (argc, argv);

	if (IsServer)
	{
		CORBA::Object_var poa_object = GiPSiNetSK_orb->resolve_initial_references ("RootPOA");
		GiPSiNetSK_poa = PortableServer::POA::_narrow (poa_object.in ());
		PortableServer::POAManager_var poa_manager = GiPSiNetSK_poa->the_POAManager ();
		poa_manager->activate ();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Server used

void RunGiPSiNetSK()
{
	GiPSiNetSK_orb->run ();
	
	// Destroy the POA, waiting until the destruction terminates
    GiPSiNetSK_poa->destroy (1, 1);
    GiPSiNetSK_orb->destroy ();
}

void CreateGiPSiNetSKServant(const char * pname)
{
	// Activate it to obtain the object reference
	GiPSiNetSK_var = GiPSiNetSK_servant._this ();

	// Name service
	CORBA::Object_var naming_context_object = GiPSiNetSK_orb->resolve_initial_references ("NameService");
    GiPSiNetSK_Naming_context = CosNaming::NamingContext::_narrow (naming_context_object.in ());
	
    // Create and initialize the name.
    CosNaming::Name name (1);
    name.length (1);
    name[0].id = CORBA::string_dup (pname);

    // Bind the object
    GiPSiNetSK_Naming_context->rebind (name, GiPSiNetSK_var.in ());
	cout <<  "GiPSiNet Simulation Kernel Servant Created!" << endl;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Client used

void ConnectGiPSiNetSKService(const char * pname)
{	
	//Name service
	CORBA::Object_var naming_context_object = GiPSiNetSK_orb->resolve_initial_references ("NameService");
	CosNaming::NamingContext_var naming_context = CosNaming::NamingContext::_narrow (naming_context_object.in ());

	CosNaming::Name VEP_name (1);
	VEP_name.length (1);
	VEP_name[0].id = CORBA::string_dup (pname);

	CORBA::Object_var GiPSiNetSK_object = naming_context->resolve (VEP_name);

	// Now downcast the object reference to the appropriate type
	GiPSiNetSK_var = CORBAGIPSINETSK::_narrow (GiPSiNetSK_object.in ());

	cout <<  "GiPSiNet Simulation Kernel Service Connected!" << endl;
}		

void saveXMLProject(const char * filename, const char * xmldata)
{
	ofstream outdata;		// outdata is like cin
	outdata.open(filename); // opens the file
	if( !outdata ) {		// file couldn't be opened
		cerr << "Error: file could not be opened" << endl;
		exit(1);
	}	
    outdata << xmldata;
	outdata.close();
}

void testConnection()
{
	char * datain = "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";			
	CORBA::String_var data = CORBA::string_dup(datain);
	start_timer(3);
	GiPSiNetSK_var->testConnection(data);
	Real time3 = get_timer(3);
	printf("time use %f\n",time3);
}

// Start GiPSiNet Server in main application
// nameService is in argv[1]
int StartGiPSiNetServer(int argc, char** argv)
{
	try {	
		if (!logger)
			logger = new Logger();		
		
		char * nameService = argv[1];

		// First initialize the ORB, that will remove some arguments...
		argc = 3;    		
		argv[1] = "-ORBInitRef"; 		
		argv[2] = nameService;//"NameService=corbaloc:iiop:129.22.151.97:1234567/NameService";

		StartGiPSiNetSK(argc, argv, true);

		CreateGiPSiNetSKServant("GiPSiNetSK");
		
		printf("Simulation Kernel Startup... (CORBA Servant) \n");

		RunGiPSiNetSK();	
		return 1;
	}
	catch (CORBA::Exception &e) {
		cerr << "CORBA exception raised in GiPSiNet Server!" << e._name() << endl;
		return 0;
	}	
	catch (...) {
		logger->Message("GiPSiNet Simulation Kernel", "", 0);
		return 0;
	}
}

// Start GiPSiNet Client in main application
// nameService is in argv[2]
// filename is in argv[1]
int StartGiPSiNetClient(int argc, char** argv)
{
	try {
		if (!logger)
			logger = new Logger();

		char * nameService = argv[2];
		char * filename = argv[1];

		// First initialize the ORB, that will remove some arguments...
		argc = 3;    		
		argv[1] = "-ORBInitRef"; 		
		argv[2] = nameService;//"NameService=corbaloc:iiop:129.22.151.97:1234567/NameService";

		StartGiPSiNetSK(argc, argv, false);	

		ConnectGiPSiNetSKService("GiPSiNetSK");
		
		// request XML project from server and send data to client and save to file
		printf("request XML project from server\n");
		
		char *xmldata;		
		GiPSiNetSK_var->getProjectXML(filename, xmldata);
		//printf("%s\n",xmldata);
		saveXMLProject(filename, xmldata);

		printf("load XML project at client\n");
		argc = 2;		
		argv[1] = filename;		
		
		VisualizationEngine * visEngine = NULL;				
		HapticsManager * hapticsMan = NULL;		
		ToolkitNetProjectLoader netloader;
		netloader.LoadProjectClient(argc, argv, nameService, &simProxy, &visEngine, &hapticsMan);

		// Send acknowleagement to server to load project "filename"
		GiPSiNetSK_var->loadedProject(filename);

		//GiPSiNetStartClient(simProxy, visEngine, hapticsMan);	
		int status;
		if (!simProxy)
		{
			logger->Error("GiPSiStartClient", "Recieved empty SimulationKernel proxy pointer.");
			exit(0);
		}
		if (!visEngine)
		{
			logger->Error("GiPSiStartClient", "Recieved empty VisualizationEngine pointer.");
			exit(0);
		}

		init_timers();

#ifdef GIPSI_HAPTICS_ENABLED
		// Start the GiPSiNet Haptics Manager Servant Thread	
		// This will call infinite loop of RunGiPSiNetHaptics()
		pthread_t GiPSiNetHapticsThread;
		status = pthread_create(&GiPSiNetHapticsThread, NULL, GiPSiNetHaptics_Start, NULL);
		if (status)
		{
			logger->Error("GiPSiStartClient", "Could not launch haptics manager thread.");
			perror("pthread_create");
			exit(0);
		}		
#endif	
		printf("Sending signal to server that haptics manager started\n");
		// Send acknowleagement to server that HM is started
		GiPSiNetSK_var->startedService("HM", nameService);

		// Start the GiPSiNet SKP-SEP Servant Thread	
		// This will call infinite loop of RunGiPSiNetVisualization()
		pthread_t GiPSiNetVEThread;
		status = pthread_create(&GiPSiNetVEThread, NULL, GiPSiNetVisualization_Start, NULL);
		if (status)
		{
			logger->Error("GiPSiStartClient", "Could not launch vitualization engine thread.");
			perror("pthread_create");
			exit(0);
		}	

		printf("Sending signal to server that visualization engine started\n");
		
		// Send acknowleagement to server that VE is started
		GiPSiNetSK_var->startedService("VE", nameService);		

		simProxy->setCORBASK(GiPSiNetSK_var);
		
		/****************************************/
		/*   Setup and Start GUI				*/
		/****************************************/
		
		visEngine->Start(simProxy);		

		return 1;
	}
	catch (CORBA::Exception &e) {
		cerr << "CORBA exception raised in GiPSiNet Client!" << e._name() << endl;
		return 0;
	}	
}

// Start GiPSiNet Haptic Client in main application
// nameService is in argv[2]
// filename is in argv[1]
int StartGiPSiNetHMClient(int argc, char** argv, HapticsManager ** hapticsMan)
{
	try {
		if (!logger)
			logger = new Logger();

		char * nameService = argv[2];
		char * filename = argv[1];
		
		// First initialize the ORB, that will remove some arguments...
		argc = 3;    		
		argv[1] = "-ORBInitRef"; 		
		argv[2] = nameService;//"NameService=corbaloc:iiop:129.22.151.97:1234567/NameService";

		//StartGiPSiNetSK(argc, argv, false);	

		//ConnectGiPSiNetSKService("GiPSiNetSK");
		
		// request XML project from server and send data to client and save to file
		//printf("request XML project from server\n");
		//char *xmldata;		
		//GiPSiNetSK_var->loadProject(filename, xmldata);
		//printf("%s\n",xmldata);
		//saveXMLProject(filename, xmldata);

		//printf("load XML project at client\n");
		//argc = 2;		
		//argv[1] = filename;		
		
		//HapticsManager * hapticsMan = NULL;		
		ToolkitNetProjectLoader netloader;
		netloader.LoadProjectHMClient(argc, argv, nameService, hapticsMan);
		
		//GiPSiNetStartHapticClient(simProxy, visEngine, hapticsMan);	
#ifdef GIPSI_HAPTICS_ENABLED
		RunGiPSiNetHaptics();
		// Start the GiPSiNet Haptics Manager Servant Thread	
		// This will call infinite loop of RunGiPSiNetHaptics()
		//pthread_t GiPSiNetHapticsThread;
		//int status = pthread_create(&GiPSiNetHapticsThread, NULL, GiPSiNetHaptics_Start, NULL);
		//if (status)
		//{
		//	logger->Error("GiPSiStartClient", "Could not launch haptics manager thread.");
		//	perror("pthread_create");
		//	exit(0);
		//}		
#endif
		//printf("Sending signal to server that haptics manager started\n");
		//GiPSiNetSK_var->startedService("HM", nameService);

		return 1;
	}
	catch (CORBA::Exception &e) {
		cerr << "CORBA exception raised in GiPSiNet Haptic Client!" << e._name() << endl;
		return 0;
	}		
}

// Start GiPSiNet Visualization Engine Client in main application
// nameService is in argv[2]
// filename is in argv[1]
int StartGiPSiNetVEClient(int argc, char** argv)
{
	try {
		if (!logger)
			logger = new Logger();

		char * nameService = argv[2];
		char * filename = argv[1];

		// First initialize the ORB, that will remove some arguments...
		argc = 3;    		
		argv[1] = "-ORBInitRef"; 		
		argv[2] = nameService;//"NameService=corbaloc:iiop:129.22.151.97:1234567/NameService";

		StartGiPSiNetSK(argc, argv, false);	

		ConnectGiPSiNetSKService("GiPSiNetSK");
		
		// request XML project from server and send data to client and save to file
		printf("request XML project from server\n");
		char *xmldata;
		//CORBA::String_var xmldata;
		GiPSiNetSK_var->getProjectXML(filename, xmldata);
		//printf("%s\n",xmldata);
		saveXMLProject(filename, xmldata);

		printf("load XML project at client\n");
		argc = 2;		
		argv[1] = filename;		
		//SimulationKernelProxy * simProxy = NULL;
		VisualizationEngine * visEngine = NULL;						
		ToolkitNetProjectLoader netloader;
		netloader.LoadProjectVEClient(argc, argv, nameService, &simProxy, &visEngine);

		// Send acknowleagement to server to load project "filename"
		GiPSiNetSK_var->loadedProject(filename);
		
		//GiPSiNetStartVEClient(simProxy, visEngine);			
		if (!simProxy)
		{
			logger->Error("GiPSiStartClient", "Recieved empty SimulationKernel proxy pointer.");
			exit(0);
		}
		if (!visEngine)
		{
			logger->Error("GiPSiStartClient", "Recieved empty VisualizationEngine pointer.");
			exit(0);
		}

		init_timers();

		// Start the GiPSiNet SKP-SEP Servant Thread	
		// This will call infinite loop of RunGiPSiNetVisualization()
		pthread_t GiPSiNetVEThread;
		int status = pthread_create(&GiPSiNetVEThread, NULL, GiPSiNetVisualization_Start, NULL);
		if (status)
		{
			logger->Error("GiPSiStartClient", "Could not launch vitualization engine thread.");
			perror("pthread_create");
			exit(0);
		}	

		GiPSiNetSK_var->startedService("HM", nameService);
		printf("Sending signal to server that visualization engine started\n");
		GiPSiNetSK_var->startedService("VE", nameService);

		simProxy->setCORBASK(GiPSiNetSK_var);

		/****************************************/
		/*   Setup and Start GUI				*/
		/****************************************/
		
		visEngine->Start(simProxy);

		return 1;		
	}
	catch (CORBA::Exception &e) {
		cerr << "CORBA exception raised in GiPSiNet VE Client!" << e._name() << endl;
		return 0;
	}	
}

#endif