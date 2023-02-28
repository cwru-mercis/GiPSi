/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSiNetSKI Implementation (GiPSiNetSK_i.cpp).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Auriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	GIPSINETSK_I.CPP v0.0
////
////	GiPSiNetSK_i Implementation
////
////////////////////////////////////////////////////////////////

#ifdef GIPSINET

#include "GiPSiNetSK_i.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

#include "timing.h"
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

#include "GiPSiNetVisualization.h"
#include "GiPSiNetHaptics.h"
#include "GlutVisualizationEngine.h"
#include "ToolkitProjectLoader.h"
#include "ToolkitNetProjectLoader.h"

using namespace GiPSiXMLWrapper;

/**
 * Start Visualization Engine Proxy Loop Thread.
 **/
void *StartVEPLoopThread(void *arg)
{
	VisualizationEngineProxy	*vep;
	vep = (VisualizationEngineProxy *) arg;  
	vep->Start();
	return NULL;
}

/**
 * Begin simulation server using the input objects.
 * 
 * @param Sim SimulationKernel to run. 
 * @param visEngineProxy VisualizationEngineProxy to run.
 * @param hapticsManProxy HapticsManagerProxy to run.
 */
void GiPSiNetStartServer(SimulationKernel * Sim, VisualizationEngineProxy * visEngineProxy, HapticsManagerProxy * hapticsManProxy)
{	
	int status;
	if (!Sim)
	{
		logger->Error("GiPSiStartServer", "Recieved empty SimulationKernel pointer.");
		exit(0);
	}
	if (!visEngineProxy)
	{
		logger->Error("GiPSiStartServer", "Recieved empty VisualizationEngine Proxy pointer.");
		exit(0);
	}

	printf("Starting simulation kernel thread...\n");
	status = pthread_create(Sim->GetSimThread(), NULL, StartSimulationThread, (void*)Sim);
	if (status)
	{
		logger->Error("GiPSiStartServer", "Could not launch simulation thread.");
		perror("pthread_create");
	    exit(0);
	}	

	init_timers();

	/****************************************/
	/*   Start Visualization Proxy Loop		*/
	/****************************************/	
	// start VEPLoop thread instead of using visEngineProxy->Start();
	printf("Starting visualization engine proxy thread...\n");
	pthread_t GiPSiNetVEPLoopThread;
	status = pthread_create(&GiPSiNetVEPLoopThread, NULL, StartVEPLoopThread, (void*)visEngineProxy);
	if (status)
	{
		logger->Error("GiPSiStartServer", "Could not launch VEP Loop thread.");
		perror("pthread_create");
	    exit(0);
	}		
}

// Implementation skeleton constructor
CORBAGIPSINETSK_i::CORBAGIPSINETSK_i (void)
{	
	sim = NULL;
	visEngineProxy = NULL;
	hapticsManProxy = NULL;
	//XMLProject = NULL;
}

// Implementation skeleton destructor
CORBAGIPSINETSK_i::~CORBAGIPSINETSK_i (void)
{	
}

void CORBAGIPSINETSK_i :: setSK (SimulationKernel * ptr)
{	
	sim = ptr;
}

void CORBAGIPSINETSK_i::getProjectXML (
    const char * filename,
    ::CORBA::String_out xmldata
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{  
	// Add your implementation here
	printf("CORBAGIPSINETSK_i::getProjectXML()...\n");
	// open file load file and send file back to client
	struct stat file_stat;
	int    status, readSize;
	char *data;
	
	int files = open(filename, O_RDONLY);	// open file
	if (files < 0)
	{
	   fprintf(stderr, "Could not open file: '%s' ", filename);
	   exit (-1);
	}

	// get file status for details see http://www.opengroup.org/onlinepubs/007908799/xsh/fstat.html
	status = fstat(files, &file_stat);     // get its status
	 
	// print file size informations for details see http://www.opengroup.org/onlinepubs/007908799/xsh/sysstat.h.html
	printf("file %s size %d\n", filename, file_stat.st_size);	// print status
	 
	// create storage for string and read it in
	data = new char[file_stat.st_size+1]; 
	readSize = read(files, data, file_stat.st_size);
	 
	// terminate the string at number of characters read and print it
	data[readSize] = '\0';
	//printf("%s\nsize=%d chars\n", data, readSize);
	close(files);
	
	// set XMLProject filename for use when loading file
	//XMLProject = new char[sizeof(filename)+1];
	//strcpy(XMLProject, filename);

	// send XMLdata back to client
	xmldata = CORBA::string_dup(data);		
}

void CORBAGIPSINETSK_i::loadedProject (
    const char * filename
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
	// Add your implementation here
	printf("CORBAGIPSINETSK_i::loadedProject(): %s\n", filename);
	
	ToolkitNetProjectLoader netloader;		
	netloader.LoadProjectServer(const_cast< char* >(filename), &sim);
	
	if (!sim)
	{
		logger->Error("GiPSiStartServer", "Recieved empty SimulationKernel pointer.");
		exit(0);
	}
	
	int status = pthread_create(sim->GetSimThread(), NULL, StartSimulationThread, (void*)sim);
	if (status)
	{
		logger->Error("GiPSiStartServer", "Could not launch simulation thread.");
		perror("pthread_create");
	    exit(0);
	}
	printf("Simulation kernel thread started...\n");

	init_timers();

	return;
}

void CORBAGIPSINETSK_i::startedService (
    const char * service,
    const char * nameservice
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
	// Add your implementation here
	printf("CORBAGIPSINETSK_i::startedService(): %s at name service: %s\n", service, nameservice);
	
	if (strcmp(service, "HM") == 0) 
	{
		ToolkitNetProjectLoader netloader;	
		int argc;    
		char * argv[3];
		argv[0] = "";
		argv[1] = "";
		argv[2] = "";
		argc = 2;	

		/****************************************/
		/*   Start Haptic Manager Proxy 		*/
		/****************************************/
		netloader.LoadProjectHMPServer(argc, argv, const_cast< char* >(nameservice), &sim, &hapticsManProxy);		
		printf("Haptic manager proxy thread started..\n");
	}
	else if (strcmp(service, "VE") == 0) 
	{
		ToolkitNetProjectLoader netloader;	
		int argc;    
		char * argv[3];
		argv[0] = "";
		argv[1] = "";
		argv[2] = "";
		argc = 2;	

		/****************************************/
		/*   Start Visualization Engin Proxy	*/
		/****************************************/
		netloader.LoadProjectVEPServer(argc, argv, const_cast< char* >(nameservice), &sim, &visEngineProxy);

		visEngineProxy->setSimKernel(sim);

		pthread_t GiPSiNetVEPLoopThread;
		int status = pthread_create(&GiPSiNetVEPLoopThread, NULL, StartVEPLoopThread, (void*)visEngineProxy);
		if (status)
		{
			logger->Error("GiPSiStartServer", "Could not launch VEP Loop thread.");
			perror("pthread_create");
			exit(0);
		}
		printf("Visualization engine proxy thread started..\n");
	}

	return;
}

void CORBAGIPSINETSK_i::setUICommand (
    const char * command
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
	// Add your implementation here
	//printf("CORBAGIPSINETSK_i::setUICommand()...\n");
	sim->setUICommand(command);
	if (strcmp(command, "exit") == 0)
	{		
		closeConnection("HM");
		closeConnection("VE");			
	}
}

void CORBAGIPSINETSK_i::closeConnection (
    const char * service
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
	// Add your implementation here
	printf("CORBAGIPSINETSK_i::closeConnection(%s) ...\n", service);
	// free Visualization engine proxy and Haptic manager proxy
		
	if (strcmp(service, "HM") == 0)
	{
		if (hapticsManProxy){
			//hapticsManProxy->EndHapticsThread();								
		}
	}
	else if (strcmp(service, "VE") == 0)
	{
		if (visEngineProxy){
			visEngineProxy->Stop();		

			//need to wait until all data send through network
#ifdef WIN32
			Sleep(1000);
#else
			usleep(1000);
#endif

		}
	}	
}

void CORBAGIPSINETSK_i::testConnection (
    char *& data
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
	// Add your implementation here
	//printf("CORBAGIPSINETSK_i::testConnection()...\n");	
	// just receive and send it back
	data = CORBA::string_dup(data);
}

#endif