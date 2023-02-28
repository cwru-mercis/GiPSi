/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Mouse Haptic Interface Implementation (MouseHapticInterface.cpp).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	MouseHapticInterface.CPP v0.0
////
////    Implementation for Mouse Haptic Interface.
////	using CPN mouse driver.
////
////////////////////////////////////////////////////////////////

// Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <conio.h>

// GiPSi includes
#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "GiPSiHaptics.h"
#include "GiPSiHapticsManager.h"
#include "timing.h"
#include "OpenHapticsManager.h"
#include "MouseHapticInterface.h"

int	MouseHapticInterface::Enable(void) 
{ 
#ifdef _USE_CPNMOUSE
	hSuspendMouse(Identifier-BASE_MOUSE_HI); 
#endif
	return 0; 
}
int	MouseHapticInterface::Disable(void) 
{ 
#ifdef _USE_CPNMOUSE
	hUnSuspendMouse(Identifier-BASE_MOUSE_HI); 
#endif
	return 0; 
}
int	MouseHapticInterface::Terminate(void) 
{ 
#ifdef _USE_CPNMOUSE
	hCleanup(); 
#endif
	return 0; 
}

MouseHapticInterface::MouseHapticInterface(unsigned int identifier) 
{	

	Identifier = identifier;
	init_flag = false; 
	enable_flag = false; 	
#ifdef _USE_CPNMOUSE
	hPOINT pMouse;
	// set the mouse to the middle of working area
	pMouse.x = MAX_X/2; pMouse.y = MAX_Y/2;
	hSetCursor(Identifier-BASE_MOUSE_HI, pMouse, NULL, 0, 0, 0);
#endif
}

void MouseHapticInterface::ReadConfiguration(Vector<Real> &Position, 
 						Matrix<Real> &Orientation, unsigned int &ButtonState) 
{
	Vector<Real>	p(3,0.0);
	Matrix<Real>	R(3,3,0.0);	
	R[0][0] = R[1][1] = R[2][2] = 1.0;
	// get absolute position from CPN mouse
	// use y component for move in/out and x component for rotation around axis
	//hGetRelativePosition(Identifier-BASE_MOUSE_HI, &pMouse);	
#ifdef _USE_CPNMOUSE
	hPOINT pMouse;		
	hGetAbsolutePosition(Identifier-BASE_MOUSE_HI, &pMouse);
	//printf("cpn mouse: %f %f\n", pMouse.x/(float)MAX_X, pMouse.y/(float)MAX_Y);
	//lastX = currentX;
	//lastY = currentY;
	//currentX += pMouse.x;
	//currentY += pMouse.y;
	p[0] = 0.0;					 	// no movement in x direction
	p[1] = -pMouse.y/(float)MAX_Y;	// move in/out in y direction with y position
	p[2] = 0.0;						// no movement in z direction			
	ToRotationMatrix(R, 3.14 * pMouse.x/(float)MAX_X, 0.0, 1.0, 0.0);	// rotate in y axis with x position
#endif
	Position = p;
	Orientation = R;
	ButtonState = 0;
}
