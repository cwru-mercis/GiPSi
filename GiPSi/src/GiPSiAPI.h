/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Core GiPSi API Definition (GiPSiAPI.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	GiPSiAPI.h v0.1.0
////
////    Definitions for the Core GiPSi API
////	1)	Defines the base class for the simulation objects
////    2)	Defines the base class for connectors
////    3)	Defines the base class for integrators
////	4)	Defines the base class for haptic interfaces
////	5)  Defines the base class for haptic interface objects
////	6)  Defines the gemetry primitives
////	7)	Defines the display primitives
////
////////////////////////////////////////////////////////////////


#ifndef _GiPSiAPI_H
#define _GiPSiAPI_H

#include "GiPSiGeometry.h"
#include "GiPSiDisplay.h"
#include "GiPSiHaptics.h"
#include "GiPSiSimObject.h"

#define	MAX_SIMOBJ		64
#define	MAX_CONNECTOR	32

#endif