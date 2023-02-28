/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for Phantom Haptic Interface (PhantomHapticInterface.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	PhantomHapticInterface.H v0.0
////
////	Header for Phantom Haptic Interface
////    Implementation is hardware specific, and assumes 
////          Phantom open haptics toolkit
////
////////////////////////////////////////////////////////////////

#ifndef _PHANTOMHAPTICINTERFACE_H
#define _PHANTOMHAPTICINTERFACE_H

#ifdef _USE_PHANTOM
// Phantom Open Haptics Toolkit Specific Incudes
#include <HD/hd.h>

#include "GiPSiHaptics.h"
#include <pthread.h>
#include "matrix.h"

#define RIGHT_PHANTOM  0x0001

//  Phantom Haptic Interface Class definition
class	PhantomHapticInterface: public HapticInterface {
public:
	PhantomHapticInterface			(unsigned int ident, unsigned int handle,  Real	RequestedUpdateRate = 1000.0 );
	int			Enable				(void);
	int			Disable				(void);
	int			Terminate			(void);

	void		SetHandle(unsigned int handle) { myhandle = handle;};

	void		ReadConfiguration	(Vector<Real> &Position, Matrix<Real> &Orientation, unsigned int &ButtonState);
	void		UseHapticModel		(GiPSiLowOrderLinearHapticModel &NewModel);
	void		switchModel			(void) { activeModel=1-activeModel; }

	unsigned int	ReturnHandle	(void) { return myhandle; }

	~PhantomHapticInterface();

	friend  HDCallbackCode HDCALLBACK HapticLoopCallback(void *data);

protected:
	unsigned int						myhandle;
	GiPSiLowOrderLinearHapticModel		Models[2];
	int									activeModel;
	Vector<Real>*						ModelState[2];
};


//
// THE FOLLOWING ARE THE TYPE DEFINITIONS USED BY THE OPEN HAPTICS TOOLKIT CALLBACK FUNCTIONS.
//

// Type definition to be used in passing data from HapticLoopCallback to main haptic server thread
typedef struct {
	HHD		handle;
    double	position[3];
    double	transform[16];
} HapticInterfaceState;

// Type definition to be used in passing data from main haptic server thread to the HapticLoopCallback
typedef struct {
	PhantomHapticInterface			*Interface;
} HapticParameters;

#endif  //_USE_PHANTOM

#endif