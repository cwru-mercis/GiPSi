/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSiNet Simulation Kernel Definition (GiPSiSimKernel.h).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	GIPSINETSIMKERNEL.h v0.0
////
////	GiPSiNet Simulation Kernel
////
////////////////////////////////////////////////////////////////

#ifndef _GIPSINETSIMKERNEL_H_
#define _GIPSINETSIMKERNEL_H_
#ifdef GIPSINET
#include "GiPSiNetSK_i.h"

void StartGiPSiNetSK(int argc, char ** argv, bool IsServer);

void RunGiPSiNetSK();
void CreateGiPSiNetSKServant(const char * pname);

void ConnectGiPSiNetSKService(const char * pname);
void saveXMLProject(const char * filename, const char * xmldata);
void testConnection();

int StartGiPSiNetServer(int argc, char** argv);
int StartGiPSiNetClient(int argc, char** argv);
int StartGiPSiNetHMClient(int argc, char** argv, HapticsManager ** hapticsMan);
int StartGiPSiNetVEClient(int argc, char** argv);
#endif
#endif /* _GIPSINETSIMKERNEL_H_ */

