/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is OpenHaptics Manager Implementation (OpenHapticsManager.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu, Suriya Natsupakpong.
*/

////	OpenHapticsManager.CPP v0.0
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

static unsigned int	NumMousePtrs=0;   
void	CPNMouse_Initialize(void);

#ifdef _USE_PHANTOM 

void	HapticsManager_Initialize(void);
void	*HapticsManager_Start(void *);
void	HapticsManager_End(void);

// utility function: multiplication of partial matrix-vector pairs
template<class T>
void multMVp(Vector<T>& result, Matrix<T>& M, Vector<T>& V, int m0, int n0, int r0, int l0, int l);

#include <HDU/hduError.h>

HDCallbackCode HDCALLBACK HapticLoopCallback(void *data);

volatile static bool HAPTICS_THREAD_END_FLAG = false;
volatile static bool HAPTICS_THREAD_END_COMPLETED_FLAG = false;

// global variables will be accessed by the HapticLoopCallback function
//   Note: It will be nicer if we find how to send data to the HapticLoopCallback function
//   other than using global variables
static unsigned int		NumPhantomPtrs=0;
PhantomHapticInterface	*PhantomPtr[2];
// Internal data necessary necessary to track change in position and velocity
// within the HapticLoopCallback function.  This is a very
// groaty way of doing this, as above.
static Vector<Real> hlc_initial_pos(3,"0.0 0.0 0.0");    // u
static Vector<Real> hlc_initial_vel(3,"0.0 0.0 0.0");    // v
static bool hlc_model_initialize_flag = false;


// *******************************************************************************
//  Accessor function for HapticsManager
// *******************************************************************************
OpenHapticsManager* OpenHapticsManager::hmInstance = NULL;

OpenHapticsManager* OpenHapticsManager::GetHapticsManager()
{
	if(hmInstance == NULL)
		hmInstance = new OpenHapticsManager();
	return hmInstance;
}

OpenHapticsManager::OpenHapticsManager()
{
	printf("Initializing Haptics Manager...\n");
	HapticsManager_Initialize();
	
	thread_running = false;
	num_hios = 0;

	// Initialize the cpn mouse
	CPNMouse_Initialize(); 
	num_mhi = NumMousePtrs;
	for(int i=0; i<num_mhi; i++)
		mhi[i] = new MouseHapticInterface(101+i);
}

bool OpenHapticsManager::isHapticsThreadRunning()
{
	return thread_running;
}

int OpenHapticsManager::StartHapticsThread()
{
	int status = pthread_create(&hapticsth, NULL, HapticsManager_Start, NULL);
	if (status == 0)
		thread_running = true;
	return status;
}

void OpenHapticsManager::EndHapticsThread()
{
	HapticsManager_End();	
}

void OpenHapticsManager::RegisterHIO(HapticInterfaceObject* HIO)
{
	///HIOArray[num_hios] = HIO;
	num_hios++;
}

HapticInterface* OpenHapticsManager::GetHapticInterface(unsigned int haptic_interface_identifier)
{
	unsigned int id=haptic_interface_identifier;
	HapticInterface *HI = NULL;

	if (haptic_interface_identifier < 100)
	{		
		// Phantom Haptic Interface
		for(int i=0; i<2; i++) {
			if( PhantomPtr[i] != NULL ) {
				if( PhantomPtr[i]->ReturnHapticInterfaceIdentifier() == id )	
					HI = (HapticInterface*) PhantomPtr[i];
			}
		}
	}
	else
	{
		// Mouse Haptic Interface
		for(int i=0; i<NumMousePtrs; i++) {
			if( mhi[i]->ReturnHapticInterfaceIdentifier() == id )
				HI = (HapticInterface*) mhi[i];
		}
	}

	return HI;
}


////////////////////////////////////////////////////////////////
//
//	HapticsManager_Initialize
//
//		Initializes the haptics manager
//
void	HapticsManager_Initialize(void) 
{
	PhantomPtr[0] = NULL;
	PhantomPtr[1] = new PhantomHapticInterface(RIGHT_PHANTOM, 0, 1000.0);
	NumPhantomPtrs=1;	
}


////////////////////////////////////////////////////////////////
//
//	HapticsManager_Start
//
//		Starts the haptic interface
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

//	PhantomHapticInterface Phantom1(RIGHT_PHANTOM, hHD, 1000.0);
//	PhantomPtr[1]=&Phantom1;
//	NumPhantomPtrs=1;
	PhantomHapticInterface& Phantom1 = *PhantomPtr[1];
	Phantom1.SetHandle(hHD);

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
	model._m	=6;							// number of inputs	: Dimension of the position measurement
	model._k	=3;							// number of outputs: DImension of force ouput
	model.A11 = new Matrix<Real>(0,0,0.0);	// _n/2 x _n/2 matrix
	model.A12 = new Matrix<Real>(0,0,0.0);  // _n/2 x _n/2 matrix
	model.B1  = new Matrix<Real>(0,0,0.0);	// _n/2 x _m matrix
	model.C11 = new Matrix<Real>(0,0,0.0);	// _k x _n/2 matrix
	model.C12 = new Matrix<Real>(0,0,0.0);  // k x n/2
	model.D   = new Matrix<Real>(3,3,0.0);	// _k x _m matrix
	model.f_0 = new Vector<Real>(3,0.0);	// _k x  1 vector
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

	HAPTICS_THREAD_END_COMPLETED_FLAG = true;

    return NULL;
}

////////////////////////////////////////////////////////////////
//
//	HapticsManager_End
//
//		Terminates the haptics manager
//
void	HapticsManager_End(void) {
	HAPTICS_THREAD_END_FLAG = true;
	while (!HAPTICS_THREAD_END_COMPLETED_FLAG) {}
	return;
}


// *******************************************************************************
// *
// *
// * CALLBACK FUNCTIONS USED BY THE OPEN HAPTICS TOOLKIT
// *
// *
// *******************************************************************************

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
	
	// Convert unit from mm to m
	for(int i=0;i<3;i++)
		pState->position[i] *= 1E-3; 
	
    return HD_CALLBACK_DONE;
}


//******************************************************************************
// A scheduler callback for synchronously changing the Local model used by the 
//  Phantom schedular callback
// This function is hardware dependent - use the SendtoHapticInterface function
//******************************************************************************/
HDCallbackCode HDCALLBACK setHapticDisplayForceCallback(void *pUserData)
{
    HapticParameters				*param		= (HapticParameters *) pUserData;

	param->Interface->switchModel();
	hlc_model_initialize_flag = false;	
	hlc_initial_pos = Vector<Real>(3,"0.0 0.0 0.0");
	hlc_initial_vel = Vector<Real>(3,"0.0 0.0 0.0");	
    return HD_CALLBACK_DONE;    
}


// *******************************************************************************
// This is called every servo loop tick.  
// *******************************************************************************
HDCallbackCode HDCALLBACK HapticLoopCallback(void *data) 
{
    HDErrorInfo error;
    HDdouble position[3];
	HDdouble velocity[3];
    HDdouble HDforce[3];
	Vector<Real> force(3, "0.0 0.0 0.0");	
 
    HHD hHD = PhantomPtr[1]->myhandle;
  
	// Begin haptics frame.  In general, all state-related haptics calls
    //   should be made within a frame.
	hdBeginFrame(hHD);

    // Get the current position+velocity of the device.
    hdGetDoublev(HD_CURRENT_POSITION, position);
	hdGetDoublev(HD_CURRENT_VELOCITY, velocity);

	// Convert unit from mm to m
	for(int i=0;i<3;i++)
	{
		position[i] *= 1E-3;
		velocity[i] *= 1E-3;
	}

	int activeModel = PhantomPtr[1]->activeModel;
	
	GiPSiLowOrderLinearHapticModel* active = &PhantomPtr[1]->Models[activeModel];
	unsigned int n = active->_n;
	unsigned int k = active->_k;
	unsigned int m = active->_m;
	unsigned int n_2 = n/2;	
	
	Vector<Real> z_new(n, 0.0);
	Vector<Real> z_new1(n_2, 0.0);
	Vector<Real> z_new2(n_2, 0.0);

	Vector<Real> z_old(n, 0.0);
	Vector<Real> z_old1(n_2, 0.0);
	Vector<Real> z_old2(n_2, 0.0);

	Vector<Real> vu(m, 0.0);	

	z_old = *(PhantomPtr[1]->ModelState[activeModel]);		

	if(!hlc_model_initialize_flag)	
	{			
		hlc_model_initialize_flag = true;
		hlc_initial_pos = position;
		hlc_initial_vel = velocity;			
	}
	else
	{			
		for(int i = 0; i < 3; i++)
		{
			vu[i+3] = position[i] - hlc_initial_pos[i];
			vu[i]   = velocity[i] - hlc_initial_vel[i];
		}				
	}
	
	for(unsigned int i = 0; i < n_2; i++)
	{		
		z_old1[i] = z_old[i];
		z_old2[i] = z_old[n_2 + i];		
	}

	// Force to be sent to the haptic interface is calculated
    memset(HDforce, 0, 3*sizeof(HDdouble));

	// Calculate z_new	= ( zdot_0 + A * z_old + B * u ) * SamplingTime
	if(n > 0)
	{
		for(unsigned int i = 0; i < n_2; i++)
		{
			z_new1[i] += (*(active->zdot_0))[i];
			z_new2[i] += (*(active->zdot_0))[n_2 + i];
		}		

		z_new1 += (*(active->A11)) * z_old1;
		z_new1 += (*(active->A12)) * z_old2;
		//multMVp(z_new1, (*(active->A11)), z_old1, 0,   0,  0,  0, n_2);
		//multMVp(z_new1, (*(active->A12)), z_old2, 0,   0,  0,  0, n_2);	
		
		z_new2 += z_old1;		

		z_new1 += (*(active->B1)) * vu;		
		
		for(unsigned int i=0; i<n_2; i++) {
			z_new[i] = z_new1[i];
			z_new[n_2 + i] = z_new2[i];			
		}
	    
		z_new = z_new * PhantomPtr[1]->ReportSamplingTime();				
  	}

	// Calculate force	= f_0 + (C * z_new) + (D * u)
	force = (*(active->f_0));
	
	if(n>0)
	{			
		for(unsigned int i=0; i<k; i++)
			for(unsigned int j=0; j<n_2; j++)			
			{
				force[i] += (*(active->C11))[i][j] * z_new[j];
				force[i] += (*(active->C12))[i][j] * z_new[n_2 + j];
			}									
	}	 
	
	force += (*(active->D)) * vu;	
	
	// Check dot product of force and normal
	double fdotn = 0.0;
	fdotn = force * (*(active->normal));
	if(fdotn<0.0) 
		force = zero_vector3;
	
	//printf("vel: [%f, %f, %f]\n", vu[0], vu[1], vu[2]);
	//printf("pos: [%f, %f, %f]\n", vu[3], vu[4], vu[5]);
    //printf("force: [%f, %f, %f]\n", force[0], force[1], force[2]);
	
	// set z_new to modelstate	
	if(z_new.dim() > 0)	
		*(PhantomPtr[1]->ModelState[activeModel]) = z_new;
	

    // Send the force to the device.
	for(unsigned int i=0; i<3; i++)
	   HDforce[i] = force[i];
	
	hdSetDoublev(HD_CURRENT_FORCE, HDforce);
    
    // End haptics frame. 
    hdEndFrame(hHD);

    // Check for errors and abort the callback if a scheduler error is detected 
    if (HD_DEVICE_ERROR(error = hdGetError()))  {
        hduPrintError(stderr, &error, "Error detected inside Haptic Loop Callback Function\n");
        if (hduIsSchedulerError(&error))  {
            return HD_CALLBACK_DONE;
        }
    }

    // Signify that the callback should continue running, i.e. that
    //   it will be called again0 the next scheduler tick. 
    return HD_CALLBACK_CONTINUE;
}


// pxj18: slow implementation of multiplication of partial matrix-vector pairs
template<class T>
void multMVp(Vector<T>& result, Matrix<T>& M, Vector<T>& V, int m0, int n0, int r0, int l0, int l)
{
    for(unsigned int i = 0; i < l; i++)
		for(unsigned int j = 0; j < l; j++)
			result[r0+i] += M[m0+i][n0+j] * V[l0+j];	
}

#endif	//_USE_PHANTOM

void CPNMouse_Initialize(void)
{
#ifdef _USE_CPNMOUSE
	// reset CPN Mouse first
	hCleanup();	
	// initialize CPN Mouse
	NumMousePtrs = hInitialise(0, NULL, NULL, BUTTON | MOVEMENT | ACCELERATE | SUSPEND);
	printf("Found %d cpnmouse.\n", NumMousePtrs);
#else
	NumMousePtrs = 1;	
#endif //_USE_CPNMOUSE
}