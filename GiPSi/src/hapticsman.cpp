/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Haptics Manager Implementation (hapticsman.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	HAPTICSMAN.CPP v0.0
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
#include "hapticsman.h"
#include "timing.h"

#define HAPTICS_TEST

#ifdef _USE_PHANTOM
// Phantom Open Haptics Toolkit Specific Incudes
//#include <HD/hd.h>
#include <HDU/hduError.h>

HDCallbackCode HDCALLBACK HapticLoopCallback(void *data);

volatile static bool HAPTICS_THREAD_END_FLAG = false;
volatile static bool HAPTICS_THREAD_END_COMPLETED_FLAG = false;

// global variables will be accessed by the HapticLoopCallback function
//   Note: It will be nicer if we find how to send data to the HapticLoopCallback function
//   other than using global variables
static unsigned int		NumPhantomPtrs=0;
PhantomHapticInterface	*PhantomPtr[2];


// *******************************************************************************
//  Constructor for PhantomHapticInterface
// *******************************************************************************
PhantomHapticInterface::PhantomHapticInterface	(HHD handle, Real	RequestedUpdateRate)
				:HapticInterface()
{
	// In this set-up, the update rate is set by the haptics server which calls the scheduler
	UpdateRate	 = RequestedUpdateRate;
	SamplingTime = 1.0/UpdateRate;

	for (int i=0; i<2; i++) {
		Models[i]._n=0;								// number of states of the low order linear model
		Models[i]._m=3;								// number of inputs	: Dimension of the position measurement
		Models[i]._k=3;								// number of outputs: DImension of force ouput
		Models[i].A=new Matrix<Real>(0,0,0.0);		// _n x _n matrix
		Models[i].B=new Matrix<Real>(0,0,0.0);		// _n x _m matrix
		Models[i].C=new Matrix<Real>(0,0,0.0);		// _k x _n matrix
		Models[i].D=new Matrix<Real>(3,3,0.0);		// _k x _m matrix
		Models[i].f_0=new Vector<Real>(3,0.0);		// _k x  1 vector
		Models[i].zdot_0=new Vector<Real>(0,0.0);	// _n x  1 vector
		Models[i].normal=new Vector<Real>(3,0.0);	// _k x  1 vector
	}
	activeModel=0;
	myhandle=handle;
	init_flag=true;
}

// *******************************************************************************
//  Enable the haptic interface
// *******************************************************************************
int		PhantomHapticInterface::Enable	(void)
{
	if (!init_flag) {
		error_display(1,"Attempting to enable haptic interface before it is initialized");
		return 0;
	}
	hdEnable(HD_FORCE_OUTPUT);
	enable_flag= true;
	return 1;

}

// *******************************************************************************
//  Disable the haptic interface
// *******************************************************************************
int		PhantomHapticInterface::Disable	(void)
{
	hdDisable(HD_FORCE_OUTPUT);
	enable_flag= false;
	return 1;
}

// *******************************************************************************
//  Completely terminate the haptic interface
// *******************************************************************************
int		PhantomHapticInterface::Terminate	(void)
{
	Disable();
	init_flag=false;
    hdDisableDevice(myhandle);
	return 1;
}

// Type definition to be used in passing data from HapticLoopCallback to main haptic server thread
typedef struct {
	HHD		handle;
    double	position[3];
    double	transform[16];
} HapticInterfaceState;

// *******************************************************************************
//  Use this scheduler callback to obtain a thread-safe snapshot of the haptic
//  device state that we intend on displaying 
//  This function is hardware dependent - use the ReadHapticInterface function
// *******************************************************************************
HDCallbackCode HDCALLBACK copyHapticDisplayStateCallback(void *pUserData) {

    HapticInterfaceState *pState = (HapticInterfaceState *) pUserData;

	hdMakeCurrentDevice(pState->handle);

    hdGetDoublev(HD_CURRENT_POSITION, pState->position);
    hdGetDoublev(HD_CURRENT_TRANSFORM, pState->transform);

    return HD_CALLBACK_DONE;
}

// *******************************************************************************
//  Use this function to read the haptic interface
// *******************************************************************************
void	PhantomHapticInterface::ReadConfiguration	(Vector<Real> &Position, Matrix<Real> &Orientation, unsigned int state)
{
    int i,j;
	HapticInterfaceState his;

	his.handle=myhandle;
    // Obtain a thread-safe copy of the current haptic display state. 
    hdScheduleSynchronous(copyHapticDisplayStateCallback, &his,
                          HD_DEFAULT_SCHEDULER_PRIORITY);
	for (i=0; i<3; i++)	{
		Position[i]=his.position[i];
		for (j=0; j<3; j++) Orientation[i][j]=his.transform[j*4+i];  // note that phantom reports data in transposed form
	}
	// state is not currently used
	state=0;
}


// Type definition to be used in passing data from main haptic server thread to the HapticLoopCallback
typedef struct {
	PhantomHapticInterface			*Interface;
} HapticParameters;

//******************************************************************************
// A scheduler callback for synchronously changing the Local model used by the 
//  Phantom schedular callback
// This function is hardware dependent - use the SendtoHapticInterface function
//******************************************************************************/
HDCallbackCode HDCALLBACK setHapticDisplayForceCallback(void *pUserData)
{
    HapticParameters				*param		= (HapticParameters *) pUserData;

	param->Interface->switchModel();

    return HD_CALLBACK_DONE;    
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

	if ((dest->_n != source->_n) || (dest->_m != source->_m) || (dest->_k != source->_k)) {
		// Which means that local model dimensions have changed
		delete dest->A;			dest->A		= new Matrix<Real>(source->_n,source->_n,0.0);
		delete dest->B;			dest->B		= new Matrix<Real>(source->_n,source->_m,0.0);
		delete dest->C;			dest->C		= new Matrix<Real>(source->_k,source->_n,0.0);
		delete dest->D;			dest->D		= new Matrix<Real>(source->_k,source->_m,0.0);
		delete dest->f_0;		dest->f_0	= new Vector<Real>(source->_k,0.0);
		delete dest->zdot_0;	dest->zdot_0= new Vector<Real>(source->_n,0.0);
		delete dest->normal;	dest->normal= new Vector<Real>(source->_k,0.0);
		dest->_n=source->_n;
		dest->_m=source->_m;
		dest->_k=source->_k;
	}
	// Now copy the data
	*(dest->A)		=	*(source->A);
	*(dest->B)		=	*(source->B);
	*(dest->C)		=	*(source->C);
	*(dest->D)		=	*(source->D);
	*(dest->f_0)	=	*(source->f_0);
	*(dest->zdot_0)	=	*(source->zdot_0);
	*(dest->normal)	=	*(source->normal);

	// Pass a thread-safe copy of the parameters
    hdScheduleSynchronous(setHapticDisplayForceCallback, &param,
                          HD_DEFAULT_SCHEDULER_PRIORITY);

}

// *******************************************************************************
//  Destructor
// *******************************************************************************
PhantomHapticInterface::~PhantomHapticInterface() 
{
	//Delete the allocated memory
	for (int i=0; i<2; i++) {
		delete Models[i].A;
		delete Models[i].B;	
		delete Models[i].C;	
		delete Models[i].D;	
		delete Models[i].f_0;
		delete Models[i].zdot_0;
		delete Models[i].normal;
		Models[i]._n=0;
		Models[i]._m=0;
		Models[i]._k=0;
	}
	init_flag=false;
}


////////////////////////////////////////////////////////////////
//
//	HapticsManager_Initialize
//
//		Initializes the haptics manager
//
void	HapticsManager_Initialize(void) 
{
}

////////////////////////////////////////////////////////////////
//
//	HapticsManager_Initialize
//
//		Initializes the haptic interface
//
void *HapticsManager_Start(void *arg) 
{	
    HDErrorInfo error;
    HDSchedulerHandle HapticLoopHandle;

	Vector<Real>	p(3);
	Matrix<Real>    R(3,3);
	unsigned int	s=0;

    // Initialize the device before attempting to call any hd functions.
    HHD hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError())) {
		hduPrintError(stderr, &error, "Failed to initialize Open Haptics Toolkit haptic device.");
        error_exit(1,"Failed to initialize Open Haptics Toolkit haptic device.\n");
		return NULL;
    }

	PhantomHapticInterface Phantom1(hHD, 1000.0);
	PhantomPtr[1]=&Phantom1;
	NumPhantomPtrs=1;

    printf("Phantom Haptic Device Initialized...\n");
    printf("Found device model: %s.\n\n", hdGetString(HD_DEVICE_MODEL_TYPE));

    // Schedule the main callback that will render forces to the device.
    HapticLoopHandle = hdScheduleAsynchronous(
		HapticLoopCallback, 0, 
        HD_MAX_SCHEDULER_PRIORITY);

	Phantom1.Enable();

	hdStartScheduler();

    // Check for errors and abort if so.
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to start scheduler");
		error_exit(1,"Failed to start scheduler.\n");
		return NULL;
    }

    // Wait until the user presses a key. Meanwhile, the scheduler is
    //   running and applying forces to the device.
    printf("Haptic Loop started...\n");

#ifdef HAPTICS_TEST
	GiPSiLowOrderLinearHapticModel 	model;
	model._n	=0;							// number of states of the low order linear model
	model._m	=3;							// number of inputs	: Dimension of the position measurement
	model._k	=3;							// number of outputs: DImension of force ouput
	model.A=new Matrix<Real>(0,0,0.0);		// _n x _n matrix
	model.B=new Matrix<Real>(0,0,0.0);		// _n x _m matrix
	model.C=new Matrix<Real>(0,0,0.0);		// _k x _n matrix
	model.D=new Matrix<Real>(3,3,0.0);		// _k x _m matrix
	model.f_0=new Vector<Real>(3,0.0);		// _k x  1 vector
	model.zdot_0=new Vector<Real>(0,0.0);	// _n x  1 vector
	model.normal=new Vector<Real>(3,0.0);	// _k x  1 vector

	Vector<Real>	f(3,0.0);
	Vector<Real>	xi(3,0.0),yi(3,0.0),zi(3,0.0);
	xi[0]=0.1; yi[1]=0.1; zi[2]=0.1;
	char xc;
#endif HAPTICS_TEST


	while (!HAPTICS_THREAD_END_FLAG) {

#ifdef HAPTICS_TEST

		printf("Press any key to print position , q to quit.\n");
		xc=getch();
		Phantom1.ReadConfiguration(p, R, s);
		printf("%f %f %f... \n %f %f %f \n %f %f %f \n %f %f %f \n",
			p[0],p[1],p[2],	R[0][0],R[0][1],R[0][2],
							R[1][0],R[1][1],R[1][2],
							R[2][0],R[2][1],R[2][2]);
		if (xc=='q') break;
		switch (xc) {
			case 'o':
				f=f+xi;
				printf("xi+\n");
				break;
			case 'p':
				f=f-xi;
				printf("xi-\n");
				break;
			case 'k':
				f=f+yi;
				printf("yi+\n");
				break;
			case 'l':
				f=f-yi;
				printf("yi-\n");
				break;
			case 'n':
				f=f+zi;
				printf("zi+\n");
				break;
			case 'm':
				f=f-zi;
				printf("zi-\n");
				break;
		}
		*(model.f_0) = f;
		Phantom1.UseHapticModel(model);

#endif // HAPTICS_TEST		

        // Periodically check if the callback has exited
        if (!hdWaitForCompletion(HapticLoopHandle, HD_WAIT_CHECK_STATUS))  {
            fprintf(stderr, "Press any key to quit.\n");     
            getch();
            break;
        }
    }
	
	// Disable device
	Phantom1.Disable();
	printf("Phantom Haptic Device Disabled...\n");

    // For cleanup, unschedule our callback and stop the scheduler. 
    hdStopScheduler();
    hdUnschedule(HapticLoopHandle);
	printf("Scheduler Stopped...\n");

    // Terminate the haptic interface, since we are done using it. 
	Phantom1.Terminate();
	printf("Phantom Haptic Device Terminated...\n");

	HAPTICS_THREAD_END_COMPLETED_FLAG=true;

    return NULL;
}

////////////////////////////////////////////////////////////////
//
//	HapticsManager_End
//
//		Terminates the haptics manager
//
void	HapticsManager_End(void) {
	HAPTICS_THREAD_END_FLAG=true;
	while (!HAPTICS_THREAD_END_COMPLETED_FLAG) {}
	return;
}


// *******************************************************************************
// This is called every servo loop tick.  
// *******************************************************************************
HDCallbackCode HDCALLBACK HapticLoopCallback(void *data) {

    HDErrorInfo error;
    HDdouble position[3];
    HDdouble force[3];
 
    HHD hHD = PhantomPtr[1]->myhandle;

    // Begin haptics frame.  In general, all state-related haptics calls
    //   should be made within a frame.
	hdBeginFrame(hHD);

    // Get the current position of the device.
    hdGetDoublev(HD_CURRENT_POSITION, position);
    
	// Force to be sent to the haptic interface is calculated
    memset(force, 0, 3*sizeof(HDdouble));

	// We are not using the full low order linear model at the moment
	for(int i=0;i<3;i++) force[i]=(*(PhantomPtr[1]->Models[PhantomPtr[1]->activeModel].f_0))[i];
    
    // Send the force to the device.
    hdSetDoublev(HD_CURRENT_FORCE, force);
    
    // End haptics frame. 
    hdEndFrame(hHD);

    // Check for errors and abort the callback if a scheduler error
    //   is detected 
    if (HD_DEVICE_ERROR(error = hdGetError()))  {
        hduPrintError(stderr, &error, "Error detected inside Haptic Loop Callback Function\n");
        if (hduIsSchedulerError(&error))  {
            return HD_CALLBACK_DONE;
        }
    }

    // Signify that the callback should continue running, i.e. that
    //   it will be called again the next scheduler tick. 
    return HD_CALLBACK_CONTINUE;
}


#else
#endif