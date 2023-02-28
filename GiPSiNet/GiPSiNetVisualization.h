/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSiNet Visualization Definition (GiPSiNetVisualization.h).

The Initial Developers of the Original Code are Qingbo Cai and Suriya Natsupakpong.  
Portions created by Qingbo Cai and Suriya Natsupakpong are Copyright (C) 2007-2008.
All Rights Reserved.

Contributor(s): Qingbo Cai, Suriya Natsupakpong.
*/

////	GIPSINETVISUALIZATION.H v0.0
////
////	GiPSiNet Visualization
////
////////////////////////////////////////////////////////////////

#ifndef _GIPSINETVISUALIZATION_H_
#define _GIPSINETVISUALIZATION_H_
#ifdef GIPSINET

#include "VisualizationI.h"

void StartGiPSiNetVisualization(int argc, char ** argv, bool IsServer);
void RunGiPSiNetVisualization();
void CreateSKPServant(SimulationKernelProxy *skp, const char * pname);
void *GiPSiNetVisualization_Start(void *arg); 

#endif

#endif /* _GIPSINETVISUALIZATION_H_ */