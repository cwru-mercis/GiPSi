/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for Simple Test Object Model (simple.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	SIMPLE.H v0.1.0
////
////	Defines:
////		SimpleTestObject	-	Simple test object class.
////
////////////////////////////////////////////////////////////////

#ifndef _SIMPLE_H
#define _SIMPLE_H

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

class LoaderUnitTest;

// Base class for Simple Test Object
// This simple object does not do anything other than displaying itself
class SimpleTestObject: public SIMObject {
public:
	// State info for the SimpleTestObjec
	typedef struct {
		Real	t; //time as dummy state 
	} State;


	// Constructors
	SimpleTestObject(	XMLNode * simObjectNode);
    
	// Loaders
	void				Load(char *filename);

	// Integrator interface
	void				AccumState(State &new_state, const State &state, const State &deriv, const Real &h);
	void				DerivState(State &deriv, State &state);
	void				AllocState(State &s);
	void				Simulate(void);

	// Display functions
	void				Display(void);
    void				SetupDisplay(XMLNodeList * simObjectChildren);

	// Update the states from current geometry - this is used for initialization
	void				Geom2State(void);

	// Updating  Geometry and Boundary from current State
	void				State2Geom(void);
	void				State2Bound(void);

	State&				GetState(void)	{	return	state; }

protected:
	void SetParameters(XMLNodeList * simObjectChildren);
	void InitializeVisualization(XMLNodeList * simObjectChildren);
	void LoadGeometry(XMLNodeList * simObjectChildren);
	void InitializeTransformation(XMLNodeList * simObjectChildren);

	State							state;

	Integrator<SimpleTestObject>	*integrator;

	Real							initialcolor[4];// Initial color specified in constructor
													//   It is used in the Load() function  

	friend LoaderUnitTest;
};



#endif
