/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Haptic Test Object Definition (haptictestobjects.h).

The Initial Developers of the Original Code are Paul Jacobs.  
Portions created by Paul Jacobs are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): Paul Jacobs.
*/

////	HAPTICTESTOBJECTS.H v0.1.0
////
////	Haptic Test Object
////
////////////////////////////////////////////////////////////////

#ifndef _HAPTIC_TEST_OBJECTS_H
#define _HAPTIC_TEST_OBJECTS_H

#include "GiPSiAPI.h"

class PlaneTestObject;

class PlaneTestObjectBoundary: public Boundary
{
public:
	void SetHapticModel(unsigned int index, GiPSiLowOrderLinearHapticModel HapticModel)
	{
		hm = HapticModel;
	}

	GiPSiLowOrderLinearHapticModel ReturnHapticModel()
	{
		return hm;
	}

protected:
	PlaneTestObject	*Object;
	GiPSiLowOrderLinearHapticModel hm;
}

class PlaneTestObject: public SimpleTestObject
{
public:
    PlaneTestObject(char* name,
                    Real *RGBAcolor,
                    Real time               = 0.0,
                    Real timestep   = 0.01):
                    SIMObject(name, time, timestep) 
	{
        for (int i=0; i<4; i++) initialcolor[i]=RGBAcolor[i];
		y_height = 0.0;
	}

	GiPSiLowOrderLinearHapticModel BuildHapticModel(Vector <Real> pos, Vector<Real> vel, double y_height)
	{
		GiPSiLowOrderLinearHapticModel hm;
		
		unsigned int n = 0;
		unsigned int m = 6;
		unsigned int k = 3;

		model->A11 = NULL;
		model->A12 = NULL;
		model->B1  = NULL;
		model->C11 = NULL;
		model->C12 = NULL;

		model->D   = new Matrix<Real>(k, m, 0.0); 
		model->f_0 = new Vector<Real>(k, 0.0);
		model->zdot_0 = NULL;
		model->normal = new Vector<Real>(k, 0.0);

		model->normal[0] = 0.0;
		model->normal[1] = 1.0;
		model->normal[2] = 0.0;

		Vector<Real> n_pos(3,0.0);

		n_pos[0] = pos[0];
		n_pos[1] = y_height;
		n_pos[2] = pos[2];

		double k_stiff = 1.0;
		double l_zero  = 0.0;
		double b_damp  = 1.5;

		Matrix<Real> K = BuidKMatrix (pos, n_pos, k_stiff, l_zero);
		Matrix<Real> B = BuildBMatrix(pos, n_pos, b_damp);
		Matrix<Real> V = BuildVMatrix(pos, n_pos, vel, Vector<Real>(3,0.0), b_damp);

		SplatMatrix(*(model->D), K, 3, 0, -1.0);
		SplatMatrix(*(model->D), B, 0, 0, -1.0);
		SplatMatrix(*(model->D), V, 0, 0, -1.0);

	}

protected:

	double y_height;

}

#endif