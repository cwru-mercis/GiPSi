/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for GiPSi Haptics Manager (hapticsman.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	HAPTICSMAN.H v0.0
////
////	Header for Haptics Manager
////    Implementation is hardware specific, and assumes 
////          Phantom open haptics toolkit
////
////////////////////////////////////////////////////////////////

#ifndef _HAPTICSMAN_H
#define _HAPTICSMAN_H

#ifdef _USE_PHANTOM
// Phantom Open Haptics Toolkit Specific Incudes
#include <HD/hd.h>
#endif

//  Public Functions exported by Haptics Manager
void	HapticsManager_Initialize(void);
void	*HapticsManager_Start(void *);
void	HapticsManager_End(void);

#ifdef _USE_PHANTOM
//  Phantom Haptic Interface Class definition
class	PhantomHapticInterface: public HapticInterface {
public:
	PhantomHapticInterface			(unsigned int handle,  Real	RequestedUpdateRate = 1000.0 );
	int			Enable				(void);
	int			Disable				(void);
	int			Terminate			(void);
    
	void		ReadConfiguration	(Vector<Real> &Position, Matrix<Real> &Orientation, unsigned int state);
	void		UseHapticModel		(GiPSiLowOrderLinearHapticModel &NewModel);
	void		switchModel			(void)	{	activeModel=1-activeModel;	}

	unsigned int	ReturnHandle	(void) { return myhandle; }

	~PhantomHapticInterface();

	friend  HDCallbackCode HDCALLBACK HapticLoopCallback(void *data);

protected:
	unsigned int						myhandle;
	GiPSiLowOrderLinearHapticModel		Models[2];
	int									activeModel;
};
#endif  //_USE_PHANTOM

#endif