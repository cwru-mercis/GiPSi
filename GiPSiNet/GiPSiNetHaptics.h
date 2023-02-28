/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSiNet Haptics Definition (GiPSiNetHaptics.h).

The Initial Developers of the Original Code are Qingbo Cai and Suriya Natsupakpong.  
Portions created by Qingbo Cai and Suriya Natsupakpong are Copyright (C) 2007-2008.
All Rights Reserved.

Contributor(s): Qingbo Cai, Suriya Natsupakpong.
*/

////	GIPSINETHAPTICS.h v0.0
////
////	GiPSiNet Haptics
////
////////////////////////////////////////////////////////////////

#ifndef _GIPSINETHAPTICS_H_
#define _GIPSINETHAPTICS_H_
#ifdef GIPSINET
#ifdef GIPSI_HAPTICS_ENABLED

#include "OpenHapticsManager.h"
#include "PhantomHapticInterface.h"
#include "MouseHapticInterface.h"

void StartGiPSiNetHaptics(int argc, char **argv, bool IsServer);
void RunGiPSiNetHaptics();
void CreatePhantomHapticInterfaceServant(PhantomHapticInterface *phantom, const char *pname);
void CreateMouseHapticInterfaceServant(MouseHapticInterface *mouse, const char *mname);
void CreateOpenHapticsManagerServant(const char *oname);
void *GiPSiNetHaptics_Start(void *arg); 

#endif
#endif
#endif /* _GIPSINETHAPTICS_H_ */