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
	                            // positions + velocities
	unsigned int	_m;			// number of inputs	: Dimension of the position+velocity measurement
	unsigned int	_k;			// number of outputs: Dimension of force ouput
	Matrix<Real>	*A11;		// _n/2 x _n/2 matrix
	Matrix<Real>	*A12;		// _n/2 x _n/2 matrix
	Matrix<Real>	*B1;		// _n/2 x _m matrix
	Matrix<Real>	*C11;		// _k x _n/2 matrix
	Matrix<Real>	*C12;		// _k x _n/2 matrix
	Matrix<Real>	*D;			// _k x _m matrix
	Vector<Real>	*f_0;		// _k x  1 vector
	Vector<Real>	*zdot_0;	// _n x  1 vector
	Vector<Real>	*normal;	// _k x  1 vector	
} GiPSiLowOrderLinearHapticModel ;

#include "GiPSiSimObject.h"
#include "XMLNode.h"
#include "XMLNodeList.h"

using namespace GiPSiXMLWrapper;

// ****************************************************************
// *			HAPTIC INTERFACE ABSTRACTION					  *  
// ****************************************************************

class	HapticInterface {
public:
	HapticInterface						(	unsigned int	HI_identifier,
											Real			RequestedUpdateRate = 1000.0 ) 
		{
			Identifier=HI_identifier;
			init_flag=false; 
			enable_flag=false; 
		}
	HapticInterface()
	{
         Identifier = 0;
		 init_flag = false;
		 enable_flag = false;
	}

	virtual	int		Enable				(void) { return 0; }
	virtual	int		Disable				(void) { return 0; }
	virtual	int		Terminate			(void) { return 0; }
    
	unsigned int	ReturnHapticInterfaceIdentifier (void)	{ return Identifier; }

	bool			IsInitialized		(void) { return init_flag; }
	bool			IsEnabled			(void) { return enable_flag&&init_flag; }
	Real			ReportUpdateRate	(void) { return init_flag?UpdateRate:0.0; }
	Real			ReportSamplingTime	(void) { return init_flag?SamplingTime:0.0; }

	virtual	void	ReadConfiguration	(Vector<Real> &Position, Matrix<Real> &Orientation, unsigned int &ButtonState) 
		{
			Vector<Real>	p(3,0.0);
			Matrix<Real>	R(3,3,0.0);
			R[0][0] = R[1][1] = R[2][2] = 1.0;
			Position = p;
			Orientation = R;
			ButtonState = 0x0000;
		}
	virtual	void	UseHapticModel		(GiPSiLowOrderLinearHapticModel &Model) {}
	virtual			~HapticInterface()
		{
			init_flag=false; 
			enable_flag=false;
		}

protected:
	unsigned int	Identifier;
	bool			init_flag;
	bool			enable_flag;
	Real			UpdateRate;
	Real			SamplingTime;	
};

// ****************************************************************
// *			HAPTIC INTERFACE OBJECT							  *  
// ****************************************************************
//
//	Haptic Interface Object is the representation of the physical
//    haptic interface in the virtual environment simulation
//
//
class	HapticInterfaceObject : public RigidSolidObject {
public:
	// Constructors
	HapticInterfaceObject(XMLNode * simObjNode);

	// Haptics API
	// A haptic interface object is not allowed to be interacted with a haptic interface
	int		ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model) {return 0; }

	// Attach and Detach API between HapticInterface and HapticInterfaceObject
	int					Attach(HapticInterface *hinterface);
	void				Detach(void)			{ attach_flag=false; }
	bool				IsAttached (void)		{ return attach_flag; }
	unsigned int		GetHIIdentifier(void)	{ return Identifier; }
	HapticInterface*	getHI(void)				{ return HI; }
	
	// Get and Set Configuration
	Matrix<Real>		GetConfiguration(void);
	void				SetBaseConfiguration(const Matrix<Real> &config);

	// Simulation API
	virtual	void		Simulate(void) { if (attach_flag) HI->ReadConfiguration(HI_Position, HI_Orientation, HI_ButtonState); }

protected:
	void				LoadParameter(XMLNodeList * simObjectChildren);
	bool				attach_flag;
	unsigned int		Identifier;
	HapticInterface*	HI;

	Vector<Real>		HI_Position;
	Matrix<Real>		HI_Orientation;
	unsigned int		HI_ButtonState;

	Matrix<Real>		R_w_lh;	// rotation from local HI to world
	Vector<Real>		t_w_lh;	// translation from local HI to world
	Vector<Real>		s_w_lh;	// scaling the translation from local HI to world

	Matrix<Real>		R_h_e;	// rotation from HI to HIO
	Vector<Real>		t_h_e;	// translation from HI to HIO
	Vector<Real>		s_h_e;	// scaling the translation from HI to HIO

	GiPSiLowOrderLinearHapticModel	HapticModel;
};

/****************************************************************
 *						BASE BOUNDARY							*  
 ****************************************************************/
class HapticInterfaceObjectBoundary : public RigidSolidBoundary {
public:
	void			Init(HapticInterfaceObject *object) { Object = (SolidObject*)object; SetHapticAttached(true);}
	Vector<Real>	GetPosition(unsigned int index) { return zero_vector3; }	
	Vector<Real>	GetVelocity(unsigned int index) { return zero_vector3; }	
	Vector<Real>	GetReactionForce(unsigned int index) { return zero_vector3; }	
	void			Set(unsigned int index, unsigned int boundary_type, 
								Vector<Real> boundary_value, 
								Real boundary_value2_scalar, Vector<Real> boundary_value2_vector) { }	
	void			ResetBoundaryCondition(void) {};
	virtual void	SetHapticModel(unsigned int index, GiPSiLowOrderLinearHapticModel  HapticModel)=0;
};

/****************************************************************
 *						BASE DOMAIN								*  
 ****************************************************************/
class HapticInterfaceObjectDomain : public RigidSolidDomain {	
public:
	void			Init(HapticInterfaceObject *object) { Object = (SolidObject*)object; }
	void			ResetDomainVariables(void) {}	
	void			SetBaseConfiguration(Matrix<Real> config) { ((HapticInterfaceObject*)Object)->SetBaseConfiguration(config); } 
	Matrix<Real>	GetConfiguration(void) { return ((HapticInterfaceObject*)Object)->GetConfiguration(); }
};

#endif
