/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit user interface Implementation (ToolkitUserInterface.cpp).

The Initial Developer of the Original Code is Suriya Natsupakpong.
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	ToolkitUserInterface.cpp v0.0
////
////	Toolkit user interface
////
////
////////////////////////////////////////////////////////////////

#include "ToolkitUserInterface.h"

EndoSimUserInterface::EndoSimUserInterface()
{
	mapping = NULL;
	size = 0;
}

EndoSimUserInterface::~EndoSimUserInterface()
{
	if (mapping!=NULL)
		delete mapping;	
}

/**
 * Initialize the endo user interface.
 * 
 */
void EndoSimUserInterface::Initialize(void)
{
	size = 8;
	mapping = new UIMapping[8];
	mapping[0].key = '_';	mapping[0].uiCommand = "exit";
	mapping[1].key = 'q';	mapping[1].uiCommand = "exit";
	mapping[2].key = 'r';	mapping[2].uiCommand = "simulation";
	mapping[3].key = 't';	mapping[3].uiCommand = "computationalHook";
	mapping[4].key = '+';	mapping[4].uiCommand = "timestepUp";
	mapping[5].key = '-';	mapping[5].uiCommand = "timestepDown";
	mapping[6].key = '[';	mapping[6].uiCommand = "inflateBalloon";
	mapping[7].key = ']';	mapping[7].uiCommand = "deflateBalloon";
}

/**
 * On key press of endo user interface.
 * 
 * @param key unsigned char of user key press.
 */
void EndoSimUserInterface::OnKeypress(unsigned char key)
{
	for(unsigned int i=0; i<size; i++) 
	{
		if(key==mapping[i].key) 
		{
			sim->setUICommand(mapping[i].uiCommand);
			return;
		}
	}
}