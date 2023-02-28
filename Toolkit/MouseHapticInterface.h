/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Mouse Haptic Interface Definition (MouseHapticInterface.h).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	MouseHapticInterface.H v0.0
////
////	Mouse Haptic Interface Definition
////	using CPN mouse driver.
////
////////////////////////////////////////////////////////////////

#ifndef _MOUSEHAPTICINTERFACE_H
#define _MOUSEHAPTICINTERFACE_H

#ifdef _USE_CPNMOUSE
#pragma comment(lib,"cpnmouse.lib"); 
#pragma comment(lib,"setupapi.lib"); 
#pragma comment(lib,"msimg32.lib");

#include <cpnmouse\hapi.h>
#endif //_USE_CPNMOUSE

#include "GiPSiHaptics.h"
#include "matrix.h"

#define MAX_X			1920
#define MAX_Y			1200
#define BASE_MOUSE_HI	100

//  Mouse Haptic Interface Class definition
class	MouseHapticInterface: public HapticInterface {
public:	
	MouseHapticInterface			(unsigned int identifier);
	~MouseHapticInterface			() {}
	int			Enable				(void);
	int			Disable				(void);
	int			Terminate			(void);

	void		ReadConfiguration	(Vector<Real> &Position, Matrix<Real> &Orientation, unsigned int &ButtonState);
	void		UseHapticModel		(GiPSiLowOrderLinearHapticModel &Model) {}	

protected:
	//int	lastX, lastY;
	//int	currentX, currentY;
};

#endif