/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for GiPSi GUI Manager (guiman.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	GUIMAN.H v0.0
////
////	Header for GUIMan
////
////////////////////////////////////////////////////////////////


#ifndef _GUIMAN_H
#define _GUIMAN_H

#include "simulator.h"


//  Public Functions exported by GUIMan
void	GUIMan_Init(SimulationKernel *sim, int width, int height, int xpos, int ypos);
int		GUIMan_Start(void);


#endif