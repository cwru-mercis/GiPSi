/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for Simple Mass-Spring-Damper Model (msd.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Paul Jacobs, Suriya Natsupakpong.
*/
////	MSD_HAPTICS.H v0.1.0
////
////	Defines:
////		BuildHapticLowerOrderModelFromMSD	-	Generator function to linearize 
////                                                  MSD models
////
////////////////////////////////////////////////////////////////

#ifndef _MSD_HAPTICS_H
#define _MSD_HAPTICS_H

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "msd.h"
#include "GiPSiHaptics.h"

#include <iostream>

using namespace std;

void AddInternalSpringEntries(GiPSiLowOrderLinearHapticModel* model, int ind1, int ind2, 
								Vector<Real> p, Vector<Real> q, Vector<Real> u, Vector<Real> v, 
								double k, double lzero, double b);

void AddContactSpringEntries(GiPSiLowOrderLinearHapticModel* model, int ind, 
								Vector<Real> p, Vector<Real> q, Vector<Real> u, Vector<Real> v, 
								double k, double lzero, double b);

void AddBoundarySpringEntries(GiPSiLowOrderLinearHapticModel* model, int ind, 
								Vector<Real> p, Vector<Real> q, Vector<Real> u, Vector<Real> v, 
								double k, double lzero, double b);

// for virtual spring connected with contact node
void AddContactSpringEntries(GiPSiLowOrderLinearHapticModel* model,  
							 Vector<Real> p, Vector<Real> q, Vector<Real> v, Vector<Real> w, 
								double k, double lzero, double b);

// for implicit state
void AddInternalSpringEntries(Matrix<Real>* A11, Matrix<Real>* A12, int ind1, int ind2, 
								Vector<Real> p, Vector<Real> q, Vector<Real> v, Vector<Real> w, 
								double k, double lzero, double b);
// end for implicit state

#endif