/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is HapticsI Implementation (HapticsI.cpp).

The Initial Developers of the Original Code are Qingbo Cai and Suriya Natsupakpong.  
Portions created by Qingbo Cai and Suriya Natsupakpong are Copyright (C) 2007-2008.
All Rights Reserved.

Contributor(s): Qingbo Cai, Suriya Natsupakpong.
*/

////	HAPTICSI.CPP v0.0
////
////	HapticsI Implementation
////
////////////////////////////////////////////////////////////////

#ifdef GIPSINET

#include "HapticsI.h"

#include "algebra.h"
#include "GiPSiHaptics.h"

using namespace std;

// Implementation skeleton constructor
CORBAHapticInterface_i::CORBAHapticInterface_i (void)
{
}

CORBAHapticInterface_i::CORBAHapticInterface_i (HapticInterface *h)
{	
	hi = h;
}

// Implementation skeleton destructor
CORBAHapticInterface_i::~CORBAHapticInterface_i (void)
{
}

::CORBA::Boolean CORBAHapticInterface_i::Enable (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAHapticInterface_i::Enable()..." << endl;
	return hi->Enable();
}

::CORBA::Boolean CORBAHapticInterface_i::Disable (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAHapticInterface_i::Disable()..." << endl;
	return hi->Disable();
}

::CORBA::Boolean CORBAHapticInterface_i::Terminate (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAHapticInterface_i::Terminate()..." << endl;
	return hi->Terminate();
}

::CORBA::Short CORBAHapticInterface_i::ReturnHapticInterfaceIdentifier (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAHapticInterface_i::ReturnHapticInterfaceIdentifier()..." << endl;
	return hi->ReturnHapticInterfaceIdentifier();
}

::CORBA::Boolean CORBAHapticInterface_i::IsInitialized (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAHapticInterface_i::IsInitialized()..." << endl;
	return hi->IsInitialized();
}

::CORBA::Boolean CORBAHapticInterface_i::IsEnabled (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAHapticInterface_i::IsEnabled()..." << endl;
	return hi->IsEnabled();
}

::CORBA::Double CORBAHapticInterface_i::ReportUpdateRate (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAHapticInterface_i::ReportUpdateRate()..." << endl;
	return hi->ReportUpdateRate();
}

::CORBA::Double CORBAHapticInterface_i::ReportSamplingTime (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAHapticInterface_i::ReportSamplingTime()..." << endl;
	return hi->ReportSamplingTime();
}

::CORBAHapticConfiguration CORBAHapticInterface_i::ReadConfiguration (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	CORBAHapticConfiguration hc;
	Vector<Real>		HI_Position (3, 0.0);
	Matrix<Real>		HI_Orientation (3,3,0.0);
	HI_Orientation[0][0] = HI_Orientation[1][1] = HI_Orientation[2][2] = 1.0;
	unsigned int		HI_ButtonState = 0x0000;

	//cout << "CORBAHapticInterface_i::ReadConfiguration()..." << endl;
	hi->ReadConfiguration (HI_Position, HI_Orientation, HI_ButtonState);

	for (int i = 0; i < 3; i++)
		hc.pos[i] = HI_Position[i];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			hc.orient[i][j] = HI_Orientation[i][j];
	hc.buttonState = HI_ButtonState;

	return hc;
}

::CORBA::Short CORBAHapticInterface_i::UseHapticModel (
    const ::CORBAGiPSiLowOrderLinearHapticModel & model
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	GiPSiLowOrderLinearHapticModel hm;
	//cout << "CORBAHapticInterface_i::UseHapticModel()..." << endl;

	hm._n = model.n; 
	hm._m = model.m; 
	hm._k = model.k; 
	hm.A11 = new Matrix<Real>(hm._n/2, hm._n/2);
	hm.A12 = new Matrix<Real>(hm._n/2, hm._n/2);
	hm.B1 = new Matrix<Real>(hm._n/2, hm._m);
	hm.C11 = new Matrix<Real>(hm._k, hm._n/2);
	hm.C12 = new Matrix<Real>(hm._k, hm._n/2);
	hm.D = new Matrix<Real>(hm._k, hm._m);
	hm.f_0 = new Vector<Real>(hm._k);
	hm.zdot_0 = new Vector<Real> (hm._n);
	hm.normal = new Vector<Real> (hm._k);

	unsigned int i,j;
	for (i = 0; i < hm._n/2; i++)
		for (j = 0; j < hm._n/2; j++)
			(*hm.A11)[i][j] = model.A11[i * hm._n/2 + j];

	for (i = 0; i < hm._n/2; i++)
		for (j = 0; j < hm._n/2; j++)
			(*hm.A12)[i][j] = model.A12[i * hm._n /2 + j];

	for (i = 0; i < hm._n/2; i++)
		for (j = 0; j < hm._m; j++)
			(*hm.B1)[i][j] = model.B1[i * hm._m + j];

	for (i = 0; i < hm._k; i++)
		for (j = 0; j < hm._n/2; j++)
			(*hm.C11)[i][j] = model.C11[i * hm._n/2 + j];

	for (i = 0; i < hm._k; i++)
		for (j = 0; j < hm._n/2; j++)
			(*hm.C12)[i][j] = model.C12[i * hm._n/2 + j];

	for (i = 0; i < hm._k; i++)
		for (j = 0; j < hm._m; j++)
			(*hm.D)[i][j] = model.D[i * hm._m + j];

	for (i = 0; i < (hm._k); i++)
		(*hm.f_0)[i] = model.f_0[i];

	for (i = 0; i < (hm._n); i++)
		(*hm.zdot_0)[i] = model.zdot_0[i];
	
	for (i = 0; i < (hm._k); i++)
		(*hm.normal)[i] = model.normal[i];

	hi->UseHapticModel(hm);

	return 1;
}

// Implementation skeleton constructor
CORBAPhantomHapticInterface_i::CORBAPhantomHapticInterface_i (PhantomHapticInterface *p)
{	
	phi = p;
}

void CORBAPhantomHapticInterface_i::setPhantom(PhantomHapticInterface *p)
{
	phi = p;
}

// Implementation skeleton destructor
CORBAPhantomHapticInterface_i::~CORBAPhantomHapticInterface_i (void)
{
}

::CORBA::Short CORBAPhantomHapticInterface_i::SetHandle (
    ::CORBA::Short handler
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAPhantomHapticInterface_i::SetHandle()..." << endl;
	phi->SetHandle(handler);

	return 1;
}

::CORBA::Short CORBAPhantomHapticInterface_i::switchModel (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAPhantomHapticInterface_i::switchModel()..." << endl;	
	phi->switchModel();

	return 1;
}

::CORBA::Short CORBAPhantomHapticInterface_i::ReturnHandle (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAPhantomHapticInterface_i::ReturnHandle()..." << endl;	
	return phi->ReturnHandle();
}

::CORBA::Boolean CORBAPhantomHapticInterface_i::Enable (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAPhantomHapticInterface_i::Enable()..." << endl;
	return phi->Enable();
}

::CORBA::Boolean CORBAPhantomHapticInterface_i::Disable (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAPhantomHapticInterface_i::Disable()..." << endl;
	return phi->Disable();
}

::CORBA::Boolean CORBAPhantomHapticInterface_i::Terminate (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAPhantomHapticInterface_i::Terminate()..." << endl;
	return phi->Terminate();
}

::CORBA::Short CORBAPhantomHapticInterface_i::ReturnHapticInterfaceIdentifier (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAPhantomHapticInterface_i::ReturnHapticInterfaceIdentifier()..." << endl;
	return phi->ReturnHapticInterfaceIdentifier();
}

::CORBA::Boolean CORBAPhantomHapticInterface_i::IsInitialized (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAPhantomHapticInterface_i::IsInitialized()..." << endl;
	return phi->IsInitialized();
}

::CORBA::Boolean CORBAPhantomHapticInterface_i::IsEnabled (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAPhantomHapticInterface_i::IsEnabled()..." << endl;
	return phi->IsEnabled();
}

::CORBA::Double CORBAPhantomHapticInterface_i::ReportUpdateRate (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAPhantomHapticInterface_i::ReportUpdateRate()..." << endl;
	return phi->ReportUpdateRate();
}

::CORBA::Double CORBAPhantomHapticInterface_i::ReportSamplingTime (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAPhantomHapticInterface_i::ReportSamplingTime()..." << endl;
	return phi->ReportSamplingTime();
}

::CORBAHapticConfiguration CORBAPhantomHapticInterface_i::ReadConfiguration (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	CORBAHapticConfiguration hc;
	Vector<Real>		HI_Position (3, 0.0);
	Matrix<Real>		HI_Orientation (3,3,0.0);
	HI_Orientation[0][0] = HI_Orientation[1][1] = HI_Orientation[2][2] = 1.0;
	unsigned int		HI_ButtonState = 0x0000;

	//cout << "CORBAPhantomHapticInterface_i::ReadConfiguration()..." << endl;
	phi->ReadConfiguration (HI_Position, HI_Orientation, HI_ButtonState);

	for (int i = 0; i < 3; i++)
		hc.pos[i] = HI_Position[i];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			hc.orient[i][j] = HI_Orientation[i][j];
	hc.buttonState = HI_ButtonState;
	/*
	cout << "Position : ";
	for (int i = 0; i < 3; i++)
		cout << "  " << HI_Position[i];
	cout << endl;
	cout << "Orientation : ";
	for (int i = 0; i < 3; i++)
	{	for (int j = 0; j < 3; j++)
			cout << "  " << HI_Orientation[i][j];
		cout << endl;
	}
	*/
	return hc;
}

::CORBA::Short CORBAPhantomHapticInterface_i::UseHapticModel (
    const ::CORBAGiPSiLowOrderLinearHapticModel & model
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	GiPSiLowOrderLinearHapticModel hm ;
	//cout << "CORBAPhantomHapticInterface_i::UseHapticModel()..." << endl;

	hm._n = model.n; 
	hm._m = model.m; 
	hm._k = model.k; 
	hm.A11 = new Matrix<Real>(hm._n/2, hm._n/2);
	hm.A12 = new Matrix<Real>(hm._n/2, hm._n/2);
	hm.B1 = new Matrix<Real>(hm._n/2, hm._m);
	hm.C11 = new Matrix<Real>(hm._k, hm._n/2);
	hm.C12 = new Matrix<Real>(hm._k, hm._n/2);
	hm.D = new Matrix<Real>(hm._k, hm._m);
	hm.f_0 = new Vector<Real>(hm._k);
	hm.zdot_0 = new Vector<Real> (hm._n);
	hm.normal = new Vector<Real> (hm._k);

	unsigned int i,j;
	for (i = 0; i < hm._n/2; i++)
		for (j = 0; j < hm._n/2; j++)
			(*hm.A11)[i][j] = model.A11[i * hm._n/2 + j];

	for (i = 0; i < hm._n/2; i++)
		for (j = 0; j < hm._n/2; j++)
			(*hm.A12)[i][j] = model.A12[i * hm._n/2 + j];

	for (i = 0; i < hm._n/2; i++)
		for (j = 0; j < hm._m; j++)
			(*hm.B1)[i][j] = model.B1[i * hm._m + j];

	for (i = 0; i < hm._k; i++)
		for (j = 0; j < hm._n/2; j++)
			(*hm.C11)[i][j] = model.C11[i * hm._n/2 + j];

	for (i = 0; i < hm._k; i++)
		for (j = 0; j < hm._n/2; j++)
			(*hm.C12)[i][j] = model.C12[i * hm._n/2 + j];

	for (i = 0; i < hm._k; i++)
		for (j = 0; j < (hm._m); j++)
			(*hm.D)[i][j] = model.D[i * hm._m + j];

	for (i = 0; i < (hm._k); i++)
		(*hm.f_0)[i] = model.f_0[i];

	for (i = 0; i < (hm._n); i++)
		(*hm.zdot_0)[i] = model.zdot_0[i];
	
	for (i = 0; i < (hm._k); i++)
		(*hm.normal)[i] = model.normal[i];

	phi->UseHapticModel(hm);

	return 1;
}

// Implementation skeleton constructor
CORBAMouseHapticInterface_i::CORBAMouseHapticInterface_i (MouseHapticInterface *m)
{
	mhi = m;
}

void CORBAMouseHapticInterface_i::setMouse(MouseHapticInterface *m)
{
	mhi = m;
}

// Implementation skeleton destructor
CORBAMouseHapticInterface_i::~CORBAMouseHapticInterface_i (void)
{
}

::CORBA::Boolean CORBAMouseHapticInterface_i::Enable (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	return mhi->Enable();
}

::CORBA::Boolean CORBAMouseHapticInterface_i::Disable (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	return mhi->Disable();
}

::CORBA::Boolean CORBAMouseHapticInterface_i::Terminate (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	return mhi->Terminate();
}

::CORBA::Short CORBAMouseHapticInterface_i::ReturnHapticInterfaceIdentifier (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	return mhi->ReturnHapticInterfaceIdentifier();
}

::CORBA::Boolean CORBAMouseHapticInterface_i::IsInitialized (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	return mhi->IsInitialized();
}

::CORBA::Boolean CORBAMouseHapticInterface_i::IsEnabled (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	return mhi->IsEnabled();
}

::CORBA::Double CORBAMouseHapticInterface_i::ReportUpdateRate (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	return mhi->ReportUpdateRate();
}

::CORBA::Double CORBAMouseHapticInterface_i::ReportSamplingTime (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	return mhi->ReportSamplingTime();
}

::CORBAHapticConfiguration CORBAMouseHapticInterface_i::ReadConfiguration (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	CORBAHapticConfiguration hc;
	Vector<Real>		HI_Position (3, 0.0);
	Matrix<Real>		HI_Orientation (3,3,0.0);
	HI_Orientation[0][0] = HI_Orientation[1][1] = HI_Orientation[2][2] = 1.0;
	unsigned int		HI_ButtonState = 0x0000;

	mhi->ReadConfiguration (HI_Position, HI_Orientation, HI_ButtonState);

	for (int i = 0; i < 3; i++)
		hc.pos[i] = HI_Position[i];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			hc.orient[i][j] = HI_Orientation[i][j];
	hc.buttonState = HI_ButtonState;

	return hc;
}

::CORBA::Short CORBAMouseHapticInterface_i::UseHapticModel (
    const ::CORBAGiPSiLowOrderLinearHapticModel & model
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	return 1;
}

// Implementation skeleton constructor
CORBAHapticsManager_i::CORBAHapticsManager_i (void)
{
}

// Implementation skeleton destructor
CORBAHapticsManager_i::~CORBAHapticsManager_i (void)
{
}

::CORBA::Short CORBAHapticsManager_i::StartHapticsThread (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	return 0;
}

::CORBA::Short CORBAHapticsManager_i::EndHapticsThread (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	return 0;
}

::CORBA::Boolean CORBAHapticsManager_i::isHapticsThreadRunning (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	return false;
}

::CORBA::Short CORBAHapticsManager_i::GetNumHapticInterfaces (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	return 0;
}

// Implementation skeleton constructor
CORBAOpenHapticsManager_i::CORBAOpenHapticsManager_i (void)
{	 //ohm = OpenHapticsManager :: GetHapticsManager();  
}

// Implementation skeleton destructor
CORBAOpenHapticsManager_i::~CORBAOpenHapticsManager_i (void)
{
}

::CORBA::Short CORBAOpenHapticsManager_i::StartHapticsThread (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAOpenHapticsManager_i::StartHapticsThread()..." << endl;
	ohm = OpenHapticsManager :: GetHapticsManager();  
	ohm->StartHapticsThread();

	return 0;
}

::CORBA::Short CORBAOpenHapticsManager_i::EndHapticsThread (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	cout << "CORBAOpenHapticsManager_i::EndHapticsThread()..." << endl;
	ohm->EndHapticsThread();

	return 0;
}

::CORBA::Boolean CORBAOpenHapticsManager_i::isHapticsThreadRunning (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAOpenHapticsManager_i::isHapticsThreadRunning()..." << endl;
	return ohm->isHapticsThreadRunning();
}

::CORBA::Short CORBAOpenHapticsManager_i::GetNumHapticInterfaces (
    
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{
  // Add your implementation here
	//cout << "CORBAOpenHapticsManager_i::GetNumHapticInterfaces()..." << endl;
	return ohm->GetNumHapticInterfaces();
}


unsigned int	HapticInterfaceProxy :: ReturnHapticInterfaceIdentifier ()
{	
	try{
		return hi->ReturnHapticInterfaceIdentifier();	
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in HapticInterfaceProxy::ReturnHapticInterfaceIdentifier()!" << endl;
		return -1;
	}
}


bool	HapticInterfaceProxy :: IsInitialized ()
{	
	try{
		return hi->IsInitialized();	
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in HapticInterfaceProxy::IsInitialized() !" << endl;
		return false;
	}
}


bool	HapticInterfaceProxy :: IsEnabled () 
{ 
	try{
		return (hi->IsEnabled() && hi->IsInitialized());	
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in HapticInterfaceProxy::IsEnabled() !" << endl;
		return false;
	}
}


Real	HapticInterfaceProxy :: ReportUpdateRate () 
{ 
	try{
		return hi->IsInitialized() ? hi->ReportUpdateRate() : 0.0;
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in HapticInterfaceProxy::ReportUpdateRate() !" << endl;
		return -1;
	}
}


Real	HapticInterfaceProxy :: ReportSamplingTime	() 
{ 
	try{
		return hi->IsInitialized() ? hi->ReportSamplingTime() : 0.0; 
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in HapticInterfaceProxy::ReportSamplingTime() !" << endl;
		return -1;
	}
}


int		PhantomHapticInterfaceProxy :: Enable ( )	
{
	try{
		return hi->Enable();
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in PhantomHapticInterfaceProxy::Enable() !" << endl;
		return -1;
	}
}


int		PhantomHapticInterfaceProxy :: Disable	()	
{
	try{
		return hi->Disable();
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in PhantomHapticInterfaceProxy::Disable() !" << endl;
		return -1;
	}
}


int		PhantomHapticInterfaceProxy :: Terminate ()	
{
	try{
		return hi->Terminate();
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in PhantomHapticInterfaceProxy::Terminate() !" << endl;
		return -1;
	}
}


bool	PhantomHapticInterfaceProxy :: IsEnabled () 
{ 
	try{
		return (hi->IsEnabled() && hi->IsInitialized()); 
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in PhantomHapticInterfaceProxy::IsEnabled() !" << endl;
		return false;
	}
}

bool	PhantomHapticInterfaceProxy :: IsInitialized () 
{ 
	try{
		return hi->IsInitialized(); 
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in PhantomHapticInterfaceProxy::IsInitialized() !" << endl;
		return false;
	} 
}

Real	PhantomHapticInterfaceProxy :: ReportUpdateRate	() 
{ 
	try{
		return hi->IsInitialized()?hi->ReportUpdateRate():0.0; 
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in PhantomHapticInterfaceProxy::ReportUpdateRate() !" << endl;
		return -1;
	}
}

unsigned int	PhantomHapticInterfaceProxy :: ReturnHapticInterfaceIdentifier ()	
{
	try{
		return hi->ReturnHapticInterfaceIdentifier();
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in PhantomHapticInterfaceProxy::ReturnHapticInterfaceIdentifier() !" << endl;
		return -1;
	}	
}

Real	PhantomHapticInterfaceProxy :: ReportSamplingTime	() 
{ 
	try{
		return hi->ReportSamplingTime(); 
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in PhantomHapticInterfaceProxy::ReportSamplingTime() !" << endl;
		return -1;
	}
}


void	PhantomHapticInterfaceProxy :: SetHandle(unsigned int handle) 
{ 
	try{
		hi->SetHandle(handle);
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in PhantomHapticInterfaceProxy::SetHandle() !" << endl;
	}
}

void	PhantomHapticInterfaceProxy :: ReadConfiguration	(Vector<Real> &Position, Matrix<Real> &Orientation, unsigned int &ButtonState)
{	
	try{
		CORBAHapticConfiguration hc = hi->ReadConfiguration();		
		for (int i = 0; i < 3; i++)
			Position[i] = hc.pos[i];
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				Orientation[i][j] = hc.orient[i][j];
		ButtonState = hc.buttonState;	
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in PhantomHapticInterfaceProxy::ReadConfiguration() !" << endl;
	}	
};

void	PhantomHapticInterfaceProxy :: UseHapticModel		(GiPSiLowOrderLinearHapticModel &NewModel)
{	//cout << "PhantomHapticInterfaceProxy UseHapticModel()..."<< endl;
	CORBAGiPSiLowOrderLinearHapticModel tmp;
	unsigned int i, j;
	tmp.n = NewModel._n;
	tmp.m = NewModel._m;
	tmp.k = NewModel._k;

	tmp.A11.length(NewModel._n/2 * NewModel._n/2);
	for (i = 0; i < NewModel._n/2; i++)
		for (j = 0; j < NewModel._n/2; j++)
			tmp.A11[i * NewModel._n/2 + j] = (*NewModel.A11)[i][j];

	tmp.A12.length(NewModel._n/2 * NewModel._n/2);
	for (i = 0; i < NewModel._n/2; i++)
		for (j = 0; j < NewModel._n/2; j++)
			tmp.A12[i * NewModel._n/2 + j] = (*NewModel.A12)[i][j];

	tmp.B1.length(NewModel._n/2 * NewModel._m);
	for (i = 0; i < NewModel._n/2; i++)
		for (j = 0; j < NewModel._m; j++)
			tmp.B1[i * NewModel._m + j] = (*NewModel.B1)[i][j];

	tmp.C11.length(NewModel._k * NewModel._n/2);
	for (i = 0; i < NewModel._k; i++)
		for (j = 0; j < NewModel._n/2; j++)
			tmp.C11[i * NewModel._n/2 + j] = (*NewModel.C11)[i][j];
							
	tmp.C12.length(NewModel._k * NewModel._n/2);
	for (i = 0; i < NewModel._k; i++)
		for (j = 0; j < NewModel._n/2; j++)
			tmp.C12[i * NewModel._n/2 + j] =(*NewModel.C12)[i][j];

	tmp.D.length(NewModel._k * NewModel._m);
	for (i = 0; i < NewModel._k; i++)
		for (j = 0; j < NewModel._m; j++)
			tmp.D[i * NewModel._m + j] = (*NewModel.D)[i][j];

	tmp.f_0.length(NewModel._k);
	for (i = 0; i < NewModel._k; i++)
		tmp.f_0[i] = (*NewModel.f_0)[i];
									
	tmp.zdot_0.length(NewModel._n);
	for (i = 0; i < NewModel._n; i++)
		tmp.zdot_0[i] = (*NewModel.zdot_0)[i];

	tmp.normal.length(NewModel._k);
	for (i = 0; i < NewModel._k; i++)
		tmp.normal[i] = (*NewModel.normal)[i];

	try{
		hi->UseHapticModel(tmp);
	}catch(CORBA::Exception &e){
		cerr << "CORBA exception " << e._name() << " raised in PhantomHapticInterfaceProxy::UseHapticModel() !" << endl;
	}
}

void	PhantomHapticInterfaceProxy :: switchModel	()	
{	
	try{
		hi->switchModel(); 
	}catch(CORBA::Exception &e){
		cerr << "CORBA exception " << e._name() << " raised in PhantomHapticInterfaceProxy::switchModel() !" << endl;
	}
}

unsigned int	PhantomHapticInterfaceProxy :: ReturnHandle () 
{ 
	try{
		return hi->ReturnHandle(); 
	}catch(CORBA::Exception &e){
		cerr << "CORBA exception " << e._name() << " raised in PhantomHapticInterfaceProxy::ReturnHandle() !" << endl;
		return -1;
	}
}

int		MouseHapticInterfaceProxy :: Enable ( )	
{
	try{
		return hi->Enable();
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in MouseHapticInterfaceProxy::Enable() !" << endl;
		return -1;
	}
}


int		MouseHapticInterfaceProxy :: Disable	()	
{
	try{
		return hi->Disable();
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in MouseHapticInterfaceProxy::Disable() !" << endl;
		return -1;
	}
}


int		MouseHapticInterfaceProxy :: Terminate ()	
{
	try{
		return hi->Terminate();
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in MouseHapticInterfaceProxy::Terminate() !" << endl;
		return -1;
	}
}


bool	MouseHapticInterfaceProxy :: IsEnabled () 
{ 
	try{
		return (hi->IsEnabled() && hi->IsInitialized()); 
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in MouseHapticInterfaceProxy::IsEnabled() !" << endl;
		return false;
	}
}

bool	MouseHapticInterfaceProxy :: IsInitialized () 
{ 
	try{
		return hi->IsInitialized(); 
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in MouseHapticInterfaceProxy::IsInitialized() !" << endl;
		return false;
	} 
}

Real	MouseHapticInterfaceProxy :: ReportUpdateRate	() 
{ 
	try{
		return hi->IsInitialized()?hi->ReportUpdateRate():0.0; 
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in MouseHapticInterfaceProxy::ReportUpdateRate() !" << endl;
		return -1;
	}
}

unsigned int	MouseHapticInterfaceProxy :: ReturnHapticInterfaceIdentifier ()	
{
	try{
		return hi->ReturnHapticInterfaceIdentifier();
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in MouseHapticInterfaceProxy::ReturnHapticInterfaceIdentifier() !" << endl;
		return -1;
	}	
}

Real	MouseHapticInterfaceProxy :: ReportSamplingTime	() 
{ 
	try{
		return hi->ReportSamplingTime(); 
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in MouseHapticInterfaceProxy::ReportSamplingTime() !" << endl;
		return -1;
	}
}


void	MouseHapticInterfaceProxy :: ReadConfiguration	(Vector<Real> &Position, Matrix<Real> &Orientation, unsigned int &ButtonState)
{	
	try{
		CORBAHapticConfiguration hc = hi->ReadConfiguration();		
		for (int i = 0; i < 3; i++)
			Position[i] = hc.pos[i];
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				Orientation[i][j] = hc.orient[i][j];
		ButtonState = hc.buttonState;		
	}catch (CORBA::Exception &e) {
		cerr << "CORBA exception " << e._name() << " raised in MouseHapticInterfaceProxy::ReadConfiguration() !" << endl;
	}	
};

void	MouseHapticInterfaceProxy :: UseHapticModel		(GiPSiLowOrderLinearHapticModel &NewModel)
{	//cout << "MouseHapticInterfaceProxy UseHapticModel()..."<< endl;
	CORBAGiPSiLowOrderLinearHapticModel tmp;
	unsigned int i, j;
	tmp.n = NewModel._n;
	tmp.m = NewModel._m;
	tmp.k = NewModel._k;

	tmp.A11.length(NewModel._n/2 * NewModel._n/2);
	for (i = 0; i < NewModel._n/2; i++)
		for (j = 0; j < NewModel._n/2; j++)
			tmp.A11[i * NewModel._n/2 + j] = (*NewModel.A11)[i][j];

	tmp.A12.length(NewModel._n/2 * NewModel._n/2);
	for (i = 0; i < NewModel._n/2; i++)
		for (j = 0; j < NewModel._n/2; j++)
			tmp.A12[i * NewModel._n/2 + j] = (*NewModel.A12)[i][j];

	tmp.B1.length(NewModel._n/2 * NewModel._m);
	for (i = 0; i < NewModel._n/2; i++)
		for (j = 0; j < NewModel._m; j++)
			tmp.B1[i * NewModel._m + j] = (*NewModel.B1)[i][j];

	tmp.C11.length(NewModel._k * NewModel._n/2);
	for (i = 0; i < NewModel._k; i++)
		for (j = 0; j < NewModel._n/2; j++)
			tmp.C11[i * NewModel._n/2 + j] = (*NewModel.C11)[i][j];
							
	tmp.C12.length(NewModel._k * NewModel._n/2);
	for (i = 0; i < NewModel._k; i++)
		for (j = 0; j < NewModel._n/2; j++)
			tmp.C12[i * NewModel._n/2 + j] =(*NewModel.C12)[i][j];

	tmp.D.length(NewModel._k * NewModel._m);
	for (i = 0; i < NewModel._k; i++)
		for (j = 0; j < NewModel._m; j++)
			tmp.D[i * NewModel._m + j] = (*NewModel.D)[i][j];

	tmp.f_0.length(NewModel._k);
	for (i = 0; i < NewModel._k; i++)
		tmp.f_0[i] = (*NewModel.f_0)[i];
									
	tmp.zdot_0.length(NewModel._n);
	for (i = 0; i < NewModel._n; i++)
		tmp.zdot_0[i] = (*NewModel.zdot_0)[i];

	tmp.normal.length(NewModel._k);
	for (i = 0; i < NewModel._k; i++)
		tmp.normal[i] = (*NewModel.normal)[i];

	try{
		hi->UseHapticModel(tmp);
	}catch(CORBA::Exception &e){
		cerr << "CORBA exception " << e._name() << " raised in MouseHapticInterfaceProxy::UseHapticModel() !" << endl;
	}
}

int HapticsManagerProxy :: StartHapticsThread()	
{
	try{
		return hm->StartHapticsThread();
	}catch(CORBA::Exception &e){
		cerr << "CORBA exception " << e._name() << " raised in HapticsManagerProxy :: StartHapticsThread() !" << endl;
		return -1;
	}
}


void HapticsManagerProxy :: EndHapticsThread()		
{
	try{
		hm->EndHapticsThread();
	}catch(CORBA::Exception &e){
		cerr << "CORBA exception " << e._name() << " raised in HapticsManagerProxy :: EndHapticsThread() !" << endl;
	}
}


 bool HapticsManagerProxy :: isHapticsThreadRunning()	
{
	try{
		return hm->isHapticsThreadRunning();
	}catch(CORBA::Exception &e){
		cerr << "CORBA exception " << e._name() << " raised in HapticsManagerProxy :: isHapticsThreadRunning() !" << endl;
		return false;
	}
 }

	
unsigned int HapticsManagerProxy :: GetNumHapticInterfaces()	
{
	try{
		return hm->GetNumHapticInterfaces();
	}catch(CORBA::Exception &e){
		cerr << "CORBA exception " << e._name() << " raised in HapticsManagerProxy :: GetNumHapticInterfaces() !" << endl;
		return -1;
	}
}


int OpenHapticsManagerProxy :: StartHapticsThread()	
{
	try{
		return hm->StartHapticsThread();
	}catch(CORBA::Exception &e){
		cerr << "CORBA exception " << e._name() << " raised in OpenHapticsManagerProxy :: StartHapticsThread() !" << endl;
		return -1;
	}
}


void OpenHapticsManagerProxy :: EndHapticsThread()		
{
	try{
		hm->EndHapticsThread();		
	}catch(CORBA::Exception &e){
		cerr << "CORBA exception " << e._name() << " raised in OpenHapticsManagerProxy :: EndHapticsThread() !" << endl;
	}
}


bool OpenHapticsManagerProxy :: isHapticsThreadRunning()	
{
	try{
		return hm->isHapticsThreadRunning();
	}catch(CORBA::Exception &e){
		cerr << "CORBA exception " << e._name() << " raised in OpenHapticsManagerProxy :: isHapticsThreadRunning() !" << endl;
		return false;
	}
}


unsigned int OpenHapticsManagerProxy :: GetNumHapticInterfaces()	
{
	try{
		return hm->GetNumHapticInterfaces();
	}catch(CORBA::Exception &e){
		cerr << "CORBA exception " << e._name() << " raised in OpenHapticsManagerProxy :: GetNumHapticInterfaces() !" << endl;
		return -1;
	}
}

#endif