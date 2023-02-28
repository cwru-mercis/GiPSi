/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi user interface Definitions (GiPSiUserInterface.h).

The Initial Developer of the Original Code is Suriya Natsupakpong.
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	GiPSiUserInterface.h v0.0
////
////	GiPSi user interface
////
////////////////////////////////////////////////////////////////

#ifndef _GiPSiUSERINTERFACE_H
#define _GiPSiUSERINTERFACE_H

#include "simulator.h"

class UserInterface {
public:
	UserInterface():sim(NULL) {};
	~UserInterface() {};
	void setSimKernel(SimulationKernel *Sim) { sim = Sim; };
	virtual void OnKeypress(unsigned char key) {};
protected:
	SimulationKernel *sim;
};

#endif