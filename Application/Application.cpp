/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Application Implementation (Application.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao and Suriya Natsupakpong are Copyright (C) 2006-2008.
All Rights Reserved.

Contributor(s): John Pao, Suriya Natsupakpong.
*/

////	APPLICATION.CPP v0.0
////
////	Application
////
////////////////////////////////////////////////////////////////

#ifdef GIPSINET
#include "ToolkitNetProjectLoader.h"
#include "GiPSiNetSimKernel.h"
#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib,"ACEd.lib");
#pragma comment(lib,"TAOd.lib");
#pragma comment(lib,"TAO_AnyTypeCoded.lib");
#pragma comment(lib,"TAO_PortableServerd.lib");
#pragma comment(lib,"TAO_CosNamingd.lib");
#pragma comment(lib,"GiPSiNet_HM_Libd.lib");
#pragma comment(lib,"GiPSiNet_SK_Libd.lib");
#pragma comment(lib,"GiPSiNet_VE_Libd.lib");
#else
#pragma comment(lib,"ACE.lib");
#pragma comment(lib,"TAO.lib");
#pragma comment(lib,"TAO_AnyTypeCode.lib");
#pragma comment(lib,"TAO_PortableServer.lib");
#pragma comment(lib,"TAO_CosNaming.lib");
#pragma comment(lib,"GiPSiNet_HM_Lib.lib");
#pragma comment(lib,"GiPSiNet_SK_Lib.lib");
#pragma comment(lib,"GiPSiNet_VE_Lib.lib");
#endif //_DEBUG
#endif //_WIN32
#endif //_GIPSINET

#include "simulator.h"
#include "GiPSiStart.h"
#include "GlutVisualizationEngine.h"
#include "ToolkitProjectLoader.h"

/**
 * Application code.
 * Option:	no option									// start GiPSi application
 *			-net startServer nameServiceIP:Port			// start Server of GiPSiNet application 
 *			-net startClient nameServiceIP:Port			// start Client of GiPSiNet application which start HapticManager and Visualization Engine
 *			-net startHMClient nameServiceIP:Port		// start Haptic Client of GiPSiNet application
 *			-net startVEClient nameServiceIP:Port		// start Visualization Client of GiPSiNet application
 */
int main(int argc, char* argv[])
{

#ifdef _DETECT_MEMORY_LEAKS
#ifdef _WIN32
	int tmp=_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmp=tmp|_CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag(tmp);
#endif
#endif

	bool GiPSiNet = false;

#ifdef GIPSINET	
	bool Server = false;
	bool Client = false;
	bool HMClient = false;
	bool VEClient = false;
	char * nameService;

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-net") == 0)	// process optional arguments
		{
			GiPSiNet = true;
			if (i + 2 <= argc - 1)			// There are enough arguments in argv
			{
				if (strcmp(argv[i+1], "startServer") == 0)
					Server = true;
				if (strcmp(argv[i+1], "startClient") == 0)
					Client = true;				
				if (strcmp(argv[i+1], "startHMClient") == 0)
					HMClient = true;
				if (strcmp(argv[i+1], "startVEClient") == 0)
					VEClient = true;
				//nameService = new char[strlen(argv[i+2])];
				//strcpy(nameService, argv[i+2]);
				//nameService = argv[i+2];

				char message1[] = "NameService=corbaloc:";
				char message2[] = "/NameService";
				int messageLen = strlen(message1) + strlen(argv[i+2]) + strlen(message2) + 1;
				nameService = new char[messageLen];
				sprintf_s(nameService, messageLen, "%s%s%s", message1, argv[i+2], message2);

				i+=2;
			}
			else
			{
				printf("%s with [-net] option need one more argument [startServer, startClient, startHapticClient, startVEClient] [nameServiceIp:Port].\n",argv[0]);
				return 0;
			}
		}
	}
#endif	

	/****************************************/
	/*   Setup Simulation					*/
	/****************************************/

	try
	{
		// Initialize project logger
		if (!logger)
			logger = new Logger();

		if (!GiPSiNet)		// Start GiPSi application
		{
			SimulationKernel * sim = NULL;
			VisualizationEngine * visEngine = NULL;
			HapticsManager * hapticsMan = NULL;
			ToolkitProjectLoader loader;
			loader.LoadProject(argc, argv, &sim, &visEngine, &hapticsMan);
			GiPSiStart(sim, visEngine, hapticsMan);
		}
#ifdef GIPSINET	
		else				// Start GiPSiNet application
		{
			if (Server)		// Start GiPSiNet Server
			{	
				argv[1] = nameService;
				StartGiPSiNetServer(argc, argv);			
			}
			
			if (Client)		// Start GiPSiNet Client
			{
				argv[2] = nameService;
				StartGiPSiNetClient(argc, argv);					
			}

			if (HMClient)	// Start GiPSiNet Haptics Client
			{
				HapticsManager * hapticsMan = NULL;
				argv[2] = nameService;
				StartGiPSiNetHMClient(argc, argv, &hapticsMan);														
			}
			
			if (VEClient)	// Start GiPSiNet Visualization Engine Client
			{
				argv[2] = nameService;
				StartGiPSiNetVEClient(argc, argv);				
			}	
		}	
#endif	
		return 1;
	}
	catch (...)
	{
		return 0;
	}
}
