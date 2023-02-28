/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is VisualizationI Definition (VisualizationI.h).

The Initial Developers of the Original Code are Qingbo Cai and Suriya Natsupakpong.  
Portions created by Qingbo Cai and Suriya Natsupakpong are Copyright (C) 2007-2008.
All Rights Reserved.

Contributor(s): Qingbo Cai, Suriya Natsupakpong.
*/

////	VISUALIZATIONI.H v0.0
////
////	VisualizationI Definition
////
////////////////////////////////////////////////////////////////

#ifndef _VISUALIZATIONI_H_
#define _VISUALIZATIONI_H_

#ifdef GIPSINET
#include "VisualizationS.h"
#include "GiPSiNetSKS.h"

#include "simulator.h"
#include "GiPSiVisualizationEngine.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

using namespace std;
#include <iostream>


class SimulationKernelProxy : public SimulationKernel
{
public:
	SimulationKernelProxy (	XMLNode *, ObjectLoader *, ConnectorLoader *, CollisionDARLoader *, Collision *);
	void			SimulationThread(void);
	int				setArray(char * name, DisplayArray * inArray);
	DisplayBuffer   *GetDisplayBufferHead() { return buffers[0]; }
	void			setCORBASK(CORBAGIPSINETSK_var sk) { skvar = sk; }
	bool			setUICommand(const char * command);
protected:
	int	num_Buffers;
	DisplayBuffer ** buffers;
	int CountBuffers(XMLNode *);
	CORBAGIPSINETSK_var skvar;
};


class  CORBASKP_i
  : public virtual POA_CORBASKP
{
public:
  // Constructor 
  CORBASKP_i (SimulationKernelProxy * s);
  
  // Destructor 
  virtual ~CORBASKP_i (void);
  
  void setSKP (SimulationKernelProxy *);

  virtual
  ::CORBA::Short setArray (
      const char * displayBufferName,
      const ::CORBADisplayArray & displayArray
    )
    ACE_THROW_SPEC ((
      ::CORBA::SystemException
    ));

private:
	SimulationKernelProxy *skptr;
};

   
class VisualizationEngineProxy : public VisualizationEngine
{
public:
	VisualizationEngineProxy (DisplayBuffer * dbHead, const char * name);
	void		Start (void);
	void		Stop (void);
	int			setArray(const char *, DisplayArray *);
	void		setSimKernel(SimulationKernel * newSimKernel);
protected:
	void OnDisplay		(void) {};
	void OnKeyboard		(unsigned char, int, int) {};
	void OnMouseClick	(int, int, int, int) {};
	void OnMouseMotion	(int, int) {};
	void OnReshape		(int, int) {};
	CORBASKP_var skptr;
	bool STOP;
};

#endif
#endif /* _VISUALIZATIONI_H_  */