/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Phantom Haptic Interface Implementation (PhantomHapticInterface.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	PhantomHapticInterface.CPP v0.0
////
////	Haptics Manager for GiPSi
////    Implementation is hardware specific, and assumes 
////          Phantom open haptics toolkit
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
#include "PhantomHapticInterface.h"


//#define HAPTICS_TEST

using namespace std;
#include <iostream>

#ifdef _USE_PHANTOM
// *******************************************************************************
//  Constructor for PhantomHapticInterface
// *******************************************************************************
PhantomHapticInterface::PhantomHapticInterface	(unsigned int ident, HHD handle, Real RequestedUpdateRate)
				:HapticInterface()
{
	// In this set-up, the update rate is set by the haptics server which calls the scheduler
	UpdateRate	 = RequestedUpdateRate;
	SamplingTime = 1.0/UpdateRate;

	for (int i=0; i<2; i++) {		
		Models[i]._n=0;								// number of states of the low order linear model
		Models[i]._m=6;								// number of inputs	: Dimension of the position measurement
		Models[i]._k=3;								// number of outputs: DImension of force ouput
		Models[i].A11=new Matrix<Real>(0,0,0.0);	// _n x _n matrix
		Models[i].A12=new Matrix<Real>(0,0,0.0);
		Models[i].B1=new Matrix<Real>(0,6,0.0);		// _n x _m matrix
		Models[i].C11=new Matrix<Real>(3,0,0.0);	// _k x _n matrix
		Models[i].C12=new Matrix<Real>(3,0,0.0);
		Models[i].D=new Matrix<Real>(3,6,0.0);		// _k x _m matrix
		Models[i].f_0=new Vector<Real>(3,0.0);		// _k x  1 vector
		Models[i].zdot_0=new Vector<Real>(0,0.0);	// _n x  1 vector
		Models[i].normal=new Vector<Real>(3,0.0);	// _k x  1 vector
		
		ModelState[i] = new Vector<Real>(0,0.0);		
	}
	activeModel = 0;

	myhandle = handle;
	init_flag = true;
	enable_flag = false;
	Identifier = ident;
}

// *******************************************************************************
//  Enable the haptic interface
// *******************************************************************************
int		PhantomHapticInterface::Enable (void)
{
	if (!init_flag) {
		error_display(1,"Attempting to enable haptic interface before it is initialized");
		return 0;
	}
	cout << "PhantomHapticInterface enabling..." << endl;
	hdEnable(HD_FORCE_OUTPUT);
	enable_flag = true;
	return 1;

}

// *******************************************************************************
//  Disable the haptic interface
// *******************************************************************************
int		PhantomHapticInterface::Disable	(void)
{
	hdDisable(HD_FORCE_OUTPUT);
	enable_flag = false;
	return 1;
}

// *******************************************************************************
//  Completely terminate the haptic interface
// *******************************************************************************
int		PhantomHapticInterface::Terminate (void)
{
	Disable();
	init_flag = false;
    hdDisableDevice(myhandle);
	return 1;
}

// *******************************************************************************
//  Use this function to read the haptic interface
// *******************************************************************************
void	PhantomHapticInterface::ReadConfiguration	(Vector<Real> &Position, Matrix<Real> &Orientation, unsigned int &ButtonState)
{
    int i,j;
	HapticInterfaceState his;

	his.handle = myhandle;
    // Obtain a thread-safe copy of the current haptic display state. 
    hdScheduleSynchronous(copyHapticDisplayStateCallback, &his, HD_DEFAULT_SCHEDULER_PRIORITY);

	for (i=0; i<3; i++)	{
		Position[i] = his.position[i];
		for (j=0; j<3; j++) 
			Orientation[i][j] = his.transform[j*4+i];  // note that phantom reports data in transposed form
	}
	// ButtonState is not currently used
	ButtonState = 0;
}


// *******************************************************************************
//  Use this function to send data to the haptic interface
// *******************************************************************************
void	PhantomHapticInterface::UseHapticModel	(GiPSiLowOrderLinearHapticModel &NewModel)
{
	HapticParameters		param;

	param.Interface		= this;
	
	GiPSiLowOrderLinearHapticModel *source	=	&NewModel;
	GiPSiLowOrderLinearHapticModel *dest	=	&(Models[1-activeModel]);

	Vector<Real>				*zerostate	=   new Vector<Real>(source->_n, 0.0);
	Vector<Real>				**deststate	=	&(ModelState[1-activeModel]);

	if ((dest->_n != source->_n) || (dest->_m != source->_m) || (dest->_k != source->_k)) {
		// Which means that local model dimensions have changed
		delete dest->A11;		dest->A11	= new Matrix<Real>(source->_n/2, source->_n/2, 0.0);		
		delete dest->A12;		dest->A12	= new Matrix<Real>(source->_n/2, source->_n/2, 0.0);
		delete dest->B1;		dest->B1	= new Matrix<Real>(source->_n/2, source->_m, 0.0);
		delete dest->C11;		dest->C11	= new Matrix<Real>(source->_k, source->_n/2, 0.0);		
		delete dest->C12;		dest->C12	= new Matrix<Real>(source->_k, source->_n/2, 0.0);
		delete dest->D;			dest->D		= new Matrix<Real>(source->_k, source->_m, 0.0);
		delete dest->f_0;		dest->f_0	= new Vector<Real>(source->_k, 0.0);
		delete dest->zdot_0;	dest->zdot_0= new Vector<Real>(source->_n, 0.0);		
		delete dest->normal;	dest->normal= new Vector<Real>(source->_k, 0.0);

		delete (*deststate);	*deststate	= new Vector<Real>(source->_n, 0.0); 

		dest->_n = source->_n;
		dest->_m = source->_m;
		dest->_k = source->_k;		
	}
	
	// Now copy the data	
	*(dest->A11)	=	*(source->A11);
	*(dest->A12)	=	*(source->A12);
	*(dest->B1)		=	*(source->B1);
	*(dest->C11) 	=	*(source->C11);
	*(dest->C12) 	=	*(source->C12);
	*(dest->D)		=	*(source->D);
	*(dest->f_0)	=	*(source->f_0);
	*(dest->zdot_0)	=	*(source->zdot_0);
	*(dest->normal)	=	*(source->normal);		
	
	*(*deststate)	=   *zerostate;
	
	delete source->A11;
	delete source->A12;
	delete source->B1;
	delete source->C11;
	delete source->C12;
	delete source->D;
	delete source->f_0;
	delete source->zdot_0;
	delete source->normal;		
	
	delete zerostate;
	
	// Pass a thread-safe copy of the parameters	
	hdScheduleSynchronous(setHapticDisplayForceCallback, &param, HD_DEFAULT_SCHEDULER_PRIORITY);
	
}

// *******************************************************************************
//  Destructor
// *******************************************************************************
PhantomHapticInterface::~PhantomHapticInterface() 
{
	//Delete the allocated memory
	for (int i=0; i<2; i++) {
		delete Models[i].A11;
		delete Models[i].A12;
		delete Models[i].B1;	
		delete Models[i].C11;
		delete Models[i].C12;
		delete Models[i].D;	
		delete Models[i].f_0;
		delete Models[i].zdot_0;
		delete Models[i].normal;
		Models[i]._n=0;
		Models[i]._m=0;
		Models[i]._k=0;		
		delete ModelState[i];
	}

	init_flag=false;
}


#endif