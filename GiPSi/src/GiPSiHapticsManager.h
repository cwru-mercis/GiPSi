/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for GiPSi Haptics Manager (GiPSiHapticsManager.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	GiPSiHapticsManager.h v0.0
////
////	Header for Haptics Manager
////
////////////////////////////////////////////////////////////////

#ifndef _GIPSIHAPTICSMANAGER_H
#define _GIPSIHAPTICSMANAGER_H

#include "GiPSiHaptics.h"
#include <pthread.h>
#include "matrix.h"

// ****************************************************************
// *			HAPTICS Manager									  *  
// ****************************************************************
// Haptics Manager class.  This is a singleton object; 
// use the factory function HapticsManager::GetHapticsManager()
// to instantiate the haptics manager, or if it already exists, get a
// pointer to it.
//
// Instantiating the Haptics Manager is not thread safe; it should be done
// in single-threaded mode, before the additional threads are started.

class HapticsManager
{
public:
	virtual int					StartHapticsThread()=0;
	virtual void				EndHapticsThread()=0;
	virtual void				RegisterHIO(HapticInterfaceObject* HIO)=0;
	virtual bool				isHapticsThreadRunning()=0;

	virtual HapticInterface*	GetHapticInterface(unsigned int haptic_interface_identifier)=0;
	virtual unsigned int		GetNumHapticInterfaces()=0;
};


#endif