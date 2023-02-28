/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSiNetSKI Definition (GiPSiNetSK_i.h).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Auriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	GIPSINETSK_I.H v0.0
////
////	GiPSiNetSK_i Definition
////
////////////////////////////////////////////////////////////////

#ifndef GIPSINETSKI_H_
#define GIPSINETSKI_H_

#ifdef GIPSINET
#include "GiPSiNetSKS.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "simulator.h"
#include "GiPSiNetVisualization.h"
#include "GiPSiNetHaptics.h"
#include "HapticsI.h"

class  CORBAGIPSINETSK_i
  : public virtual POA_CORBAGIPSINETSK
{
public:
  // Constructor 
  CORBAGIPSINETSK_i (void);
  
  // Destructor 
  virtual ~CORBAGIPSINETSK_i (void);
  
  void setSK (SimulationKernel * ptr);

  virtual
  void getProjectXML (
      const char * filename,
      ::CORBA::String_out xmldata
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  void loadedProject (
      const char * filename
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  void startedService (
      const char * service,
      const char * nameservice
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  void setUICommand (
      const char * command
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  void closeConnection (
      const char * service
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));
  
  virtual
  void testConnection (
      char *& data
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));

private:
	  SimulationKernel * sim;	
	  VisualizationEngineProxy * visEngineProxy;
	  HapticsManagerProxy * hapticsManProxy; 
};

#endif
#endif /* GIPSINETSKI_H_  */

