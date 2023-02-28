/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for Open Haptics Manager (OpenHapticsManager.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	OpenHapticsManager.H v0.0
////
////	Header for Haptics Manager
////    Implementation is hardware specific, and assumes 
////          Phantom open haptics toolkit
////
////////////////////////////////////////////////////////////////

#ifndef _OPENHAPTICSMANAGER_H
#define _OPENHAPTICSMANAGER_H

#ifdef _USE_PHANTOM
// Open Haptics Toolkit Specific Includes
#include <HD/hd.h>

#include "GiPSiHaptics.h"
#include "GiPSiHapticsManager.h"
#include <pthread.h>
#include "matrix.h"
#include "MouseHapticInterface.h"

#define MAX_HIOS 10
#define MAX_MHI 2	// Maximum of Mouse haptic interface


// ****************************************************************
// *			OpenHapticsManager								  *  
// ****************************************************************
// OpenHapticsManager class.  This is a singleton object; 
// use the factory function OpenHapticsManager::GetHapticsManager()
// to instantiate the haptics manager, or if it already exists, get a
// pointer to it.
//
// Instantiating the Haptics Manager is not thread safe; it should be done
// in single-threaded mode, before the additional threads are started.

class OpenHapticsManager: public HapticsManager
{
public:
   int StartHapticsThread();   // replaces HapticsManager_Start
   void EndHapticsThread();
   void RegisterHIO(HapticInterfaceObject* HIO);
   bool isHapticsThreadRunning();

   HapticInterface* GetHapticInterface(unsigned int haptic_interface_identifier);
   unsigned int GetNumHapticInterfaces(){ return num_hios; };

   static OpenHapticsManager* GetHapticsManager();

protected:
   OpenHapticsManager(); // the haptics manager may not be instantiated directly
   ~OpenHapticsManager(){}; // no-one else can delete it, either

   static OpenHapticsManager* hmInstance;
   pthread_t hapticsth;
   bool thread_running;

   ///HapticInterfaceObject* HIOArray[MAX_HIOS];
   unsigned int num_hios;

   // CPN mouse
   unsigned int num_mhi;
   MouseHapticInterface *mhi[MAX_MHI];   
};

//
// THE FOLLOWING ARE THE CALLBACK FUNCTIONS FOR THE OPEN HAPTICS TOOLKIT.
//   THEY WILL BE CALLED BY THE HAPTIC INTERFACE DEVICES (such as, PhantomHapticInterface)
//

HDCallbackCode HDCALLBACK HapticLoopCallback(void *data);
HDCallbackCode HDCALLBACK copyHapticDisplayStateCallback(void *pUserData);
HDCallbackCode HDCALLBACK setHapticDisplayForceCallback(void *pUserData);


#endif	//_USE_PHANTOM

#endif