/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Glut Visualization Engine implementation
(GlutVisualizationEngine.h).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen.
*/

#ifndef GLUTVISUALIZATIONENGINE__H
#define GLUTVISUALIZATIONENGINE__H

/*
===============================================================================
	Headers
===============================================================================
*/

#include "GiPSiVisualizationEngine.h"

/*
===============================================================================
	Glut interface
===============================================================================
*/

class GlutVisualizationEngine : virtual public VisualizationEngine
{
	friend void GlutMainWindow_Display		(void);
	friend void GlutMainWindow_Keyboard		(unsigned char key, int x, int y);
	friend void GlutMainWindow_KeyboardSpec	(int key, int x, int y);
	friend void GlutMainWindow_MouseClick	(int button, int state, int x, int y);
	friend void GlutMainWindow_MouseMotion	(int x, int y);
	friend void GlutMainWindow_Reshape		(int width, int height);

public:
	GlutVisualizationEngine(int argc, char **argv, SimulationKernel* simKernel,
							int x, int y, int width, int height);
	virtual void Start(void);

protected:
	virtual void OnDisplay		(void);
	virtual void OnKeyboard		(unsigned char key, int x, int y);
	virtual void OnKeyboardSpec	(int key, int x, int y);
	virtual void OnMouseClick	(int button, int state, int x, int y);
	virtual void OnMouseMotion	(int x, int y);
	virtual void OnReshape		(int width, int height);

private:
	int mouseX;
	int mouseY;

};

#endif // #ifndef GLUTVISUALIZATIONENGINE__H