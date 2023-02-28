/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Catheter Haptic Interface Object Definition (catheter.h).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	CATHETER.H v0.0
////
////	Header for Catheter Haptic Interface Object 
////
////////////////////////////////////////////////////////////////

#ifndef _CATHETER_H
#define _CATHETER_H

#include "GiPSiAPI.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

class CatheterHIO: public HapticInterfaceObject {
public:
	CatheterHIO(XMLNode * simObjectNode);
	~CatheterHIO();
	void InitializeVisualization(XMLNodeList * simObjectChildren);
	void LoadGeometry(XMLNodeList * simObjectChildren);
	void InitializeTransformation(XMLNodeList * simObjectChildren);

	// Loaders
	void Load(char *filename);
	void LoadBoundary(char *filename);
	void LoadBoundaryMap(char *filename);
	void Init(void);	

	// Display functions
	void Display(void);
	void SetupDisplay(XMLNodeList * simObjectChildren);
	void Geom2State(void);
	void State2Geom(void);
	void State2Bound(void);
	void Bound2Geom(void);
	void Geom2Bound(void);

	// Simulation API
	void Simulate(void);

	Geometry* GetGeometryPtr(void) const { return geometry; }

protected:
	Real			initialcolor[4];
	TriSurface	*	InitialGeometry;
	
	unsigned int	*mapping;		/**< mapping array size = 2*num_mapping */
	unsigned int	num_mapping;	/**< number of mapping */

	friend LoaderUnitTest;
};

// Boundary for CatheterHIO 
class CatheterHIOBoundary : public HapticInterfaceObjectBoundary {
public:
	/*void SetNullModel() 
	{		
		hm._n=0;								// number of states of the low order linear model
		hm._m=6;								// number of inputs	: Dimension of the position measurement
		hm._k=3;								// number of outputs: DImension of force ouput
		hm.A11=new Matrix<Real>(0,0,0.0);		// _n x _n matrix
		hm.A12=new Matrix<Real>(0,0,0.0);
		hm.B1=new Matrix<Real>(0,6,0.0);		// _n x _m matrix
		hm.C11=new Matrix<Real>(3,0,0.0);		// _k x _n matrix
		hm.C12=new Matrix<Real>(3,0,0.0);
		hm.D=new Matrix<Real>(3,6,0.0);			// _k x _m matrix
		hm.f_0=new Vector<Real>(3,0.0);			// _k x  1 vector
		hm.zdot_0=new Vector<Real>(0,0.0);		// _n x  1 vector
		hm.normal=new Vector<Real>(3,0.0);
	}*/

	void	SetHapticModel(unsigned int index, GiPSiLowOrderLinearHapticModel  HapticModel)
	{
		hm = HapticModel;
		/*
		if(hm._n!=0) {
			// At this point we need to scale the matrices and vectors
			//   for proper unit conversion for the input output of haptic interface			
			*(hm.zdot_0)	*= 1E-2;		// [cm -> m]
			*(hm.C11)		*= 1E-3;		// [g -> kg]
			*(hm.C12)		*= 1E-3;		// [g -> kg]
			*(hm.D)			*= 1E-3;		// [g -> kg]
			*(hm.f_0)		*= 1E-5;		// [g cm -> kg m] 
		}*/
	};

	GiPSiLowOrderLinearHapticModel	ReturnHapticModel()
	{
		return hm;
	};

	void ResetBoundaryCondition(void) {}
	void Set(unsigned int index, unsigned int boundary_type, 
								Vector<Real> boundary_value, 
								Real boundary_value2_scalar, Vector<Real> boundary_value2_vector) { *(hm.f_0) = boundary_value; }
protected:
	GiPSiLowOrderLinearHapticModel hm;
};

class CatheterHIODomain : public HapticInterfaceObjectDomain {
};

#endif