/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit user interface Definitions (ToolkitUserInterface.h).

The Initial Developer of the Original Code is Suriya Natsupakpong.
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	ToolkitUserInterface.h v0.0
////
////	Defines toolkit user interface
////
////////////////////////////////////////////////////////////////

#ifndef _TOOLKIT_USER_INTERFACE_H
#define _TOOLKIT_USER_INTERFACE_H

#include "GiPSiUserInterface.h"

class EndoSimUserInterface : public UserInterface {
public:
	typedef struct {
		unsigned char key;
		char * uiCommand;
	} UIMapping;
	EndoSimUserInterface();
	~EndoSimUserInterface();
	void Initialize(void);
	void OnKeypress(unsigned char key);
private:
	UIMapping *mapping;
	unsigned int size;
};

#endif