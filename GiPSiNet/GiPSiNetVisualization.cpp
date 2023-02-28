/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSiNet Visualization Implementation (GiPSiNetVisualization.cpp).

The Initial Developers of the Original Code are Qingbo Cai and Suriya Natsupakpong.  
Portions created by Qingbo Cai and Suriya Natsupakpong are Copyright (C) 2007-2008.
All Rights Reserved.

Contributor(s): Qingbo Cai, Suriya Natsupakpong.
*/

////	GIPSINETVISUALIZATION.CPP v0.0
////
////	GiPSiNet Visualization
////
////////////////////////////////////////////////////////////////

#ifdef GIPSINET

#include <ace/streams.h>
#include <orbsvcs/CosNamingC.h>

#include "VisualizationI.h"
#include "simulator.h"
#include "GiPSiVisualizationEngine.h"

CORBA::ORB_var GiPSiNetVisualization_Orb;
PortableServer::POA_var GiPSiNetVisualizationPoa;
CORBASKP_var skp_var;
CORBASKP_i skp_i(NULL);
CosNaming::NamingContext_var GiPSiNetVisualizationNaming_context;

void StartGiPSiNetVisualization(int argc, char ** argv, bool IsServer)
{
	GiPSiNetVisualization_Orb = CORBA::ORB_init (argc, argv);

	if (IsServer)
	{
		CORBA::Object_var poa_object = GiPSiNetVisualization_Orb->resolve_initial_references ("RootPOA");
		GiPSiNetVisualizationPoa = PortableServer::POA::_narrow (poa_object.in ());
		PortableServer::POAManager_var poa_manager = GiPSiNetVisualizationPoa->the_POAManager ();
		poa_manager->activate ();
	}
}

void RunGiPSiNetVisualization()
{
	GiPSiNetVisualization_Orb->run ();	

	// Destroy the POA, waiting until the destruction terminates
    GiPSiNetVisualizationPoa->destroy (1, 1);
    GiPSiNetVisualization_Orb->destroy ();
}

void CreateSKPServant(SimulationKernelProxy *skp, const char * pname)
{
	skp_i.setSKP(skp);

	// Activate it to obtain the object reference
    skp_var = skp_i._this ();

	// Name service
	CORBA::Object_var naming_context_object = GiPSiNetVisualization_Orb->resolve_initial_references ("NameService");
    GiPSiNetVisualizationNaming_context = CosNaming::NamingContext::_narrow (naming_context_object.in ());
	
    // Create and initialize the name.
    CosNaming::Name name (1);
    name.length (1);
    name[0].id = CORBA::string_dup (pname);

    // Bind the object
    GiPSiNetVisualizationNaming_context->rebind (name, skp_var.in ());
	cout <<  "SKPServant Created!" << endl;
}

void *GiPSiNetVisualization_Start(void *arg)
{
	RunGiPSiNetVisualization();
	return NULL;
}

/**********************************************/
VisualizationEngineProxy :: VisualizationEngineProxy (DisplayBuffer * dbHead, const char * name)
								: VisualizationEngine (dbHead, NULL)
{	
	//Name service
	CORBA::Object_var naming_context_object =
		GiPSiNetVisualization_Orb->resolve_initial_references ("NameService");
	CosNaming::NamingContext_var naming_context =
		CosNaming::NamingContext::_narrow (naming_context_object.in ());

	CosNaming::Name VEP_name (1);
	VEP_name.length (1);
	VEP_name[0].id = CORBA::string_dup (name);

	CORBA::Object_var skp_object = naming_context->resolve (VEP_name);

	// Now downcast the object reference to the appropriate type
	skp_var = CORBASKP::_narrow (skp_object.in ());
	skptr = skp_var;
	STOP = false;

	return;
}		
#endif