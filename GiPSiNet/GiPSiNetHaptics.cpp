/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSiNet Haptics Implementation (GiPSiNetHaptics.cpp).

The Initial Developers of the Original Code are Qingbo Cai and Suriya Natsupakpong.  
Portions created by Qingbo Cai and Suriya Natsupakpong are Copyright (C) 2007-2008.
All Rights Reserved.

Contributor(s): Qingbo Cai, Suriya Natsupakpong.
*/

////	GIPSINETHAPTICS.CPP v0.0
////
////	GiPSiNet Haptics
////
////////////////////////////////////////////////////////////////

#ifdef GIPSINET
#ifdef GIPSI_HAPTICS_ENABLED

#include <ace/streams.h>
#include <orbsvcs/CosNamingC.h>
#include "HapticsI.h"
#include "GiPSiNetHaptics.h"

CORBA::ORB_var GiPSiNetHaptics_Orb;
PortableServer::POA_var poa;
CORBAPhantomHapticInterface_var phi;
CORBAMouseHapticInterface_var mhi;
CosNaming::NamingContext_var naming_context;
CORBAPhantomHapticInterface_i phantom_i(NULL);
CORBAMouseHapticInterface_i mouse_i(NULL);
CORBAOpenHapticsManager_i openHapticsManager_i;
CORBAOpenHapticsManager_var ohm;

void StartGiPSiNetHaptics(int argc, char **argv, bool IsServer)
{
	GiPSiNetHaptics_Orb = CORBA::ORB_init (argc, argv);

	if (IsServer)
	{
		CORBA::Object_var poa_object = GiPSiNetHaptics_Orb->resolve_initial_references ("RootPOA");
		poa = PortableServer::POA::_narrow (poa_object.in ());
		PortableServer::POAManager_var poa_manager = poa->the_POAManager ();
		poa_manager->activate ();
	}
}

void RunGiPSiNetHaptics()
{
	GiPSiNetHaptics_Orb->run ();
	
	// Destroy the POA, waiting until the destruction terminates
    poa->destroy (1, 1);
    GiPSiNetHaptics_Orb->destroy ();
}


void CreatePhantomHapticInterfaceServant(PhantomHapticInterface *phantom, const char *pname)
{
	phantom_i.setPhantom(phantom);

	// Activate it to obtain the object reference
    phi = phantom_i._this ();

	// Name service
	CORBA::Object_var naming_context_object = GiPSiNetHaptics_Orb->resolve_initial_references ("NameService");
    naming_context = CosNaming::NamingContext::_narrow (naming_context_object.in ());
	
    // Create and initialize the name.
    CosNaming::Name name (1);
    name.length (1);
    name[0].id = CORBA::string_dup (pname);

    // Bind the object
    naming_context->rebind (name, phi.in ());
}

void CreateMouseHapticInterfaceServant(MouseHapticInterface *mouse, const char *mname)
{
	mouse_i.setMouse(mouse);

	// Activate it to obtain the object reference
    mhi = mouse_i._this ();

	// Name service
	CORBA::Object_var naming_context_object = GiPSiNetHaptics_Orb->resolve_initial_references ("NameService");
    naming_context = CosNaming::NamingContext::_narrow (naming_context_object.in ());
	
    // Create and initialize the name.
    CosNaming::Name name (1);
    name.length (1);
    name[0].id = CORBA::string_dup (mname);

    // Bind the object
    naming_context->rebind (name, mhi.in ());
}

void CreateOpenHapticsManagerServant(const char *oname)
{
	// Activate it to obtain the object reference
	CORBAOpenHapticsManager_var ohm = openHapticsManager_i._this ();

	// Name service
	CORBA::Object_var naming_context_object = GiPSiNetHaptics_Orb->resolve_initial_references ("NameService");
    CosNaming::NamingContext_var naming_context = CosNaming::NamingContext::_narrow (naming_context_object.in ());
	
    // Create and initialize the name.
    CosNaming::Name HM_name (1);
    HM_name.length (1);
    HM_name[0].id = CORBA::string_dup (oname);

    // Bind the object
	naming_context->rebind (HM_name, ohm.in ());
}

void *GiPSiNetHaptics_Start(void *arg)
{
	RunGiPSiNetHaptics();
	return NULL;
}

/**********************************************/
PhantomHapticInterfaceProxy :: PhantomHapticInterfaceProxy	(const char * phiName)
		:	HapticInterfaceProxy(), PhantomHapticInterface(RIGHT_PHANTOM, 0, 1000.0)
	{	//hi = phi;
		//Name service
		CORBA::Object_var naming_context_object =
			GiPSiNetHaptics_Orb->resolve_initial_references ("NameService");
		CosNaming::NamingContext_var naming_context =
			CosNaming::NamingContext::_narrow (naming_context_object.in ());

		CosNaming::Name name (1);
		name.length (1);
		name[0].id = CORBA::string_dup (phiName);

		CORBA::Object_var phantomHI_object = naming_context->resolve (name);

		// Now downcast the object reference to the appropriate type
		phi = CORBAPhantomHapticInterface::_narrow (phantomHI_object.in ());
		hi = phi;
	}

MouseHapticInterfaceProxy :: MouseHapticInterfaceProxy	(const char * mhiName)
		:	HapticInterfaceProxy(), MouseHapticInterface(BASE_MOUSE_HI + 1)
	{	
		//Name service
		CORBA::Object_var naming_context_object =
			GiPSiNetHaptics_Orb->resolve_initial_references ("NameService");
		CosNaming::NamingContext_var naming_context =
			CosNaming::NamingContext::_narrow (naming_context_object.in ());

		CosNaming::Name name (1);
		name.length (1);
		name[0].id = CORBA::string_dup (mhiName);

		CORBA::Object_var mouseHI_object = naming_context->resolve (name);

		// Now downcast the object reference to the appropriate type
		mhi = CORBAMouseHapticInterface::_narrow (mouseHI_object.in ());
		hi = mhi;
	}

OpenHapticsManagerProxy * OpenHapticsManagerProxy :: GetOpenHapticsManagerProxy (const char * name)
{	//Name service
	CORBA::Object_var naming_context_object =
      GiPSiNetHaptics_Orb->resolve_initial_references ("NameService");
    CosNaming::NamingContext_var naming_context =
      CosNaming::NamingContext::_narrow (naming_context_object.in ());

    CosNaming::Name HM_name (1);
	HM_name.length (1);
    HM_name[0].id = CORBA::string_dup (name);

	CORBA::Object_var openHM_object = naming_context->resolve (HM_name);

    // Now downcast the object reference to the appropriate type
	ohm = CORBAOpenHapticsManager::_narrow (openHM_object.in ());
	OpenHapticsManagerProxy *ohmp = new OpenHapticsManagerProxy (ohm);
	return ohmp;
}

#endif // GIPSI_HAPTICS_ENABLED
#endif // GIPSINET