/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is HapticsI Definition (HapticsI.h).

The Initial Developers of the Original Code are Qingbo Cai and Suriya Natsupakpong.  
Portions created by Qingbo Cai and Suriya Natsupakpong are Copyright (C) 2007-2008.
All Rights Reserved.

Contributor(s): Qingbo Cai, Suriya Natsupakpong.
*/

////	HAPTICSI.H v0.0
////
////	HapticsI Definition
////
////////////////////////////////////////////////////////////////

#ifndef _HAPTICSI_H_
#define _HAPTICSI_H_

#ifdef GIPSINET
#include "HapticsS.h"

#include "GiPSiHaptics.h"
#include "PhantomHapticInterface.h"
#include "MouseHapticInterface.h"
#include "OpenHapticsManager.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include <iostream>
using namespace std;

class  CORBAHapticInterface_i
  : public virtual POA_CORBAHapticInterface
{
public:
  // Constructor 
  CORBAHapticInterface_i (void);

  CORBAHapticInterface_i (HapticInterface *);
  
  // Destructor 
  virtual ~CORBAHapticInterface_i (void);
  
  virtual
  ::CORBA::Boolean Enable (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean Disable (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean Terminate (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Short ReturnHapticInterfaceIdentifier (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean IsInitialized (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean IsEnabled (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Double ReportUpdateRate (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Double ReportSamplingTime (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBAHapticConfiguration ReadConfiguration (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Short UseHapticModel (
      const ::CORBAGiPSiLowOrderLinearHapticModel & model
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));

private:
	HapticInterface *hi;
};

class  CORBAPhantomHapticInterface_i
  : public virtual POA_CORBAPhantomHapticInterface
{
public:
  // Constructor 
  CORBAPhantomHapticInterface_i (PhantomHapticInterface *);

  void setPhantom(PhantomHapticInterface *);
  
  // Destructor 
  virtual ~CORBAPhantomHapticInterface_i (void);
  
  virtual
  ::CORBA::Short SetHandle (
      ::CORBA::Short handler
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Short switchModel (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Short ReturnHandle (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean Enable (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean Disable (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean Terminate (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Short ReturnHapticInterfaceIdentifier (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean IsInitialized (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean IsEnabled (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Double ReportUpdateRate (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Double ReportSamplingTime (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBAHapticConfiguration ReadConfiguration (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Short UseHapticModel (
      const ::CORBAGiPSiLowOrderLinearHapticModel & model
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));

private:
	PhantomHapticInterface *phi;
};

class  CORBAMouseHapticInterface_i
  : public virtual POA_CORBAMouseHapticInterface
{
public:
  // Constructor 
  CORBAMouseHapticInterface_i (MouseHapticInterface *);

  void setMouse(MouseHapticInterface *);
  
  // Destructor 
  virtual ~CORBAMouseHapticInterface_i (void);
  
  virtual
  ::CORBA::Boolean Enable (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean Disable (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean Terminate (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Short ReturnHapticInterfaceIdentifier (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean IsInitialized (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean IsEnabled (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Double ReportUpdateRate (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Double ReportSamplingTime (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBAHapticConfiguration ReadConfiguration (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Short UseHapticModel (
      const ::CORBAGiPSiLowOrderLinearHapticModel & model
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  private:
	MouseHapticInterface *mhi;
};

class  CORBAHapticsManager_i
  : public virtual POA_CORBAHapticsManager
{
public:
  // Constructor 
  CORBAHapticsManager_i (void);
  
  // Destructor 
  virtual ~CORBAHapticsManager_i (void);
  
  virtual
  ::CORBA::Short StartHapticsThread (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Short EndHapticsThread (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean isHapticsThreadRunning (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Short GetNumHapticInterfaces (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
};

class  CORBAOpenHapticsManager_i
  : public virtual POA_CORBAOpenHapticsManager
{
public:
  // Constructor 
	CORBAOpenHapticsManager_i (void);
  
  // Destructor 
  virtual ~CORBAOpenHapticsManager_i (void);
  
  virtual
  ::CORBA::Short StartHapticsThread (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Short EndHapticsThread (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Boolean isHapticsThreadRunning (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  ::CORBA::Short GetNumHapticInterfaces (
      
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));

private:
	OpenHapticsManager *ohm;
};




/**************GiPSiNetHaptics.h*************/


class	HapticInterfaceProxy : virtual public HapticInterface 
{public:
	HapticInterfaceProxy() {}

	virtual	int		Enable				(void) { return 0; }
	virtual	int		Disable				(void) { return 0; }
	virtual	int		Terminate			(void) { return 0; }
    
	unsigned int	ReturnHapticInterfaceIdentifier (void);
	bool			IsInitialized		(void);
	bool			IsEnabled			(void);

	Real			ReportUpdateRate	(void);
	Real			ReportSamplingTime	(void);

	virtual	void	ReadConfiguration	(Vector<Real> &Position, Matrix<Real> &Orientation, unsigned int &ButtonState) 
		{
			Vector<Real>	p(3,0.0);
			Matrix<Real>	R(3,3,0.0);
			R[0][0]=R[1][1]=R[2][2]=1.0;
			Position=p;
			Orientation=R;
			ButtonState=0x0000;
		}

	virtual	void	UseHapticModel		(GiPSiLowOrderLinearHapticModel &Model) {}

	virtual			~HapticInterfaceProxy(void)	{}

protected:
	CORBAHapticInterface_var hi;
};



//  Phantom Haptic Interface Proxy Class definition
class	PhantomHapticInterfaceProxy: public HapticInterfaceProxy, public PhantomHapticInterface {
public:
	PhantomHapticInterfaceProxy	(const char * phi);

	int				Enable				(void);
	int				Disable				(void);
	int				Terminate			(void);

	bool			IsEnabled			(void);
	bool			IsInitialized		(void);

	Real			ReportUpdateRate	(void);
	unsigned int	ReturnHapticInterfaceIdentifier (void);
	Real			ReportSamplingTime	(void);

	void			SetHandle			(unsigned int handle);

	void			ReadConfiguration	(Vector<Real> &Position, Matrix<Real> &Orientation, unsigned int &ButtonState);
	void			UseHapticModel		(GiPSiLowOrderLinearHapticModel &NewModel);

	void			switchModel			(void);
	unsigned int	ReturnHandle		(void);

	~PhantomHapticInterfaceProxy(void){};

private:
	CORBAPhantomHapticInterface_var hi;
};

//  Mouse Haptic Interface Proxy Class definition
class	MouseHapticInterfaceProxy: public HapticInterfaceProxy, public MouseHapticInterface {
public:
	MouseHapticInterfaceProxy	(const char * mhi);

	int				Enable				(void);
	int				Disable				(void);
	int				Terminate			(void);

	bool			IsEnabled			(void);
	bool			IsInitialized		(void);

	Real			ReportUpdateRate	(void);
	unsigned int	ReturnHapticInterfaceIdentifier (void);
	Real			ReportSamplingTime	(void);

	void			ReadConfiguration	(Vector<Real> &Position, Matrix<Real> &Orientation, unsigned int &ButtonState);
	void			UseHapticModel		(GiPSiLowOrderLinearHapticModel &NewModel);

	~MouseHapticInterfaceProxy(void){};

private:
	CORBAMouseHapticInterface_var hi;
};

class HapticsManagerProxy : virtual public HapticsManager
{
public:
	virtual int StartHapticsThread(void);
	virtual void EndHapticsThread(void);
	virtual bool isHapticsThreadRunning(void);

	virtual HapticInterface* GetHapticInterface(unsigned int ) {return NULL;}// to be completed
	virtual unsigned int GetNumHapticInterfaces();

protected:
	CORBAHapticsManager_var hm;
};


class OpenHapticsManagerProxy : public OpenHapticsManager, public HapticsManagerProxy
{
public:
	int StartHapticsThread();
	void EndHapticsThread();
	bool isHapticsThreadRunning();
	void RegisterHIO(HapticInterfaceObject *) {}; // to be deleted

	HapticInterface* GetHapticInterface(unsigned int n)	{return NULL;}// to be completed later
	unsigned int GetNumHapticInterfaces();

	static OpenHapticsManagerProxy * GetOpenHapticsManagerProxy(const char *);

protected:
	OpenHapticsManagerProxy (CORBAOpenHapticsManager_var h) : OpenHapticsManager()
	{hm = h;};

	~OpenHapticsManagerProxy(){};

	CORBAOpenHapticsManager_var hm;
};

#endif
#endif /* HAPTICSI_H_  */