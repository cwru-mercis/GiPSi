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

#ifndef _RIGID_INSTRUMENT_H
#define _RIGID_INSTRUMENT_H

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "XMLNode.h"


using namespace GiPSiXMLWrapper;


class RigidInstrumentBoundary : public HapticInterfaceObjectBoundary {

public:
	void	SetHapticModel(unsigned int index, GiPSiLowOrderLinearHapticModel  HapticModel) {
		hm=HapticModel; 
		// The last specified Haptic Model survives
		// We may need to handle coordinate transformations here
	}
	
	GiPSiLowOrderLinearHapticModel	ReturnHapticModel() {return hm;}	// This function will be called from
																		//  RigidInstrument.Simulate()
																		//  to get the haptic model.

protected:
	GiPSiLowOrderLinearHapticModel  hm;

};

// Base class for Simple Test Object
// This simple object does not do anything other than displaying itself
class RigidInstrument: public HapticInterfaceObject {
public:
	// State info for the SimpleTestObjec
	typedef struct {
		Real	t; //time as dummy state 
	} State;


	// Constructors
	RigidInstrument(XMLNode * simObjectNode);
    
	void				InitializeVisualization(XMLNodeList * simObjectChildren);

	// Loaders
	void				Load(char *filename);  // from simple object, but also initialize the boundary etc.

	// Simulation interface
	void				Simulate(void) {
		time+=timestep;
		if (attach_flag) {
			HI->ReadConfiguration(HI_Position, HI_Orientation, HI_ButtonState);
			HI->UseHapticModel((dynamic_cast<RigidInstrumentBoundary*>(boundary))->ReturnHapticModel());
		}
		State2Geom();
		State2Bound();
	}

	// Display functions
	void				Display(void);
    void				SetupDisplay(void);

	// Updating  Geometry and Boundary from current State
	void				State2Geom(void);  // apply the proper transformation to base geometry
	void				State2Bound(void); // apply the proper transformation to base geometry

protected:
	State							state;
	Real							initialcolor[4];	// Initial color specified in constructor
														//   It is used in the Load() function

};

#endif
