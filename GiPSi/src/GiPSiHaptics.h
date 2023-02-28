/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for GiPSi Haptics API (GiPSiHaptics.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	GISPIHAPTICS.H v0.0
////
////	Header for GiPSi Haptics API 
////
////////////////////////////////////////////////////////////////


#ifndef _GiPSiHAPTICS_H
#define _GiPSiHAPTICS_H

#include "algebra.h"

// ****************************************************************
// *			LOW ORDER LINEAR MODEL DEFINITION				  *  
// ****************************************************************
// This is the Low Order Linear Model to be operated at the haptic rate at the haptic interface
// For details, please refer to:
// "Multirate Simulation for High Fidelity Haptic Interaction with Deformable Objects 
//  in Virtual Environments."  M. C. Cavusoglu, F. Tendick.  In Proceedings of the IEEE 
//  International Conference on Robotics and Automation (ICRA 2000), San Francisco, CA, 
//  April 24-28, 2000, pp. 2458-2465.
//
//   The model simulated at high update rate is:
//   u		= CurrentPosition-InitialPosition
//   z_new	= ( zdot_0 + A * z_last + B * u ) * SamplingTime
//   force	= ( f_0    + C * z_new  + D * u )
//	 if  (  (force . normal) >0  )   then  force = 0   
//
//   with initial condition
//   z_last	= 0
//   and InitialPosition = Haptic instrument CurrentPosition at model start time
//   
//   SamplingTime = 1 / (Haptic Interface Update Rate)
//
//  The if-then condition is put so that surface does not stick to the instrument if desired
//

typedef struct {
	unsigned int	_n;			// number of states of the low order linear model
	unsigned int	_m;			// number of inputs	: Dimension of the position measurement
	unsigned int	_k;			// number of outputs: DImension of force ouput
	Matrix<Real>	*A;			// _n x _n matrix
	Matrix<Real>	*B;			// _n x _m matrix
	Matrix<Real>	*C;			// _k x _n matrix
	Matrix<Real>	*D;			// _k x _m matrix
	Vector<Real>	*f_0;		// _k x  1 vector
	Vector<Real>	*zdot_0;	// _n x  1 vector
	Vector<Real>	*normal;	// _k x  1 vector
} GiPSiLowOrderLinearHapticModel ;


// ****************************************************************
// *			HAPTIC INTERFACE ABSTRACTION					  *  
// ****************************************************************
class	HapticInterface {
public:
	HapticInterface						(Real	RequestedUpdateRate = 1000.0 ) 
		{
			init_flag=false; 
			enable_flag=false; 
		}

	virtual	int		Enable				(void) { return 0; }
	virtual	int		Disable				(void) { return 0; }
	virtual	int		Terminate			(void) { return 0; }
    
	bool			IsInitialized		(void) { return init_flag; }
	bool			IsEnabled			(void) { return enable_flag&&init_flag; }
	Real			ReportUpdateRate	(void) { return init_flag?UpdateRate:0.0; }
	Real			ReportSamplingTime	(void) { return init_flag?SamplingTime:0.0; }

	virtual	void	ReadConfiguration	(Vector<Real> &Position, Matrix<Real> &Orientation, unsigned int state) 
		{
			Vector<Real>	p(3,0.0);
			Matrix<Real>	R(3,3,0.0);
			R[0][0]=R[1][1]=R[2][2]=1.0;
			Position=p;
			Orientation=R;
		}
	virtual	void	UseHapticModel		(GiPSiLowOrderLinearHapticModel &Model) {}
	virtual			~HapticInterface()
		{
			init_flag=false; 
			enable_flag=false;
		}

protected:
	bool	init_flag;
	bool	enable_flag;
	Real	UpdateRate;
	Real	SamplingTime;
};


#endif