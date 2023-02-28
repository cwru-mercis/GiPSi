/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Balloon Object Model Definition (balloon.h).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	BALLOON.H v0.1.0
////
////	Defines:
////		BalloonObject	-	Balloon object class.
////
////////////////////////////////////////////////////////////////

#ifndef _BALLOON_H
#define _BALLOON_H

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

class LoaderUnitTest;

// Base class for Balloon Object
class BalloonObject: public DeformableSolidObject {
public:
	
	// Constructors
	BalloonObject(XMLNode * simObjectNode);
    
	// Loaders
	void			Load(char *filename);
	void			Init(void);	

	// Display functions
	void			Display(void);
    void			SetupDisplay(XMLNodeList * simObjectChildren);

	// Update the states from current geometry - this is used for initialization
	void			Geom2State(void);

	// Updating  Geometry and Boundary from current State
	void			State2Geom(void);
	void			State2Bound(void);
	
	// Inflate and Deflate balloon
	void			Inflate(void) { bScale += Vector<Real>(3,"0.2 0.0 0.2"); }
	void			Deflate(void) {	bScale -= Vector<Real>(3,"0.2 0.0 0.2"); } 

	// Simulation API
	void			Simulate(void);

	Geometry*		GetGeometryPtr(void) const { return geometry; }

	void			SetBaseConfiguration(Matrix<Real> config);	

protected:
	void			SetParameters(XMLNodeList * simObjectChildren);
	void			InitializeVisualization(XMLNodeList * simObjectChildren);
	void			LoadGeometry(XMLNodeList * simObjectChildren);
	void			InitializeTransformation(XMLNodeList * simObjectChildren);

	Real			initialcolor[4];// Initial color specified in constructor
									//   It is used in the Load() function  
	TriSurface	*	InitialGeometry;

	Vector<Real>	bScale;	// balloon inflate and deflate

	Matrix<Real>	g_w_lb;	// transformatio from local balloon to world	

	Matrix<Real>	R_lb_b;	// rotation from balloon to local balloon
	Vector<Real>	t_lb_b;	// translation from balloon to local balloon
	Vector<Real>	s_lb_b;	// scaling the translation from balloon to local balloon

	friend LoaderUnitTest;
};

/**
 * BalloonBoundary Class
 * Base balloon boundary class
 */
class BalloonBoundary : public DeformableSolidBoundary {
public:
	// Initializer
	void				Init(BalloonObject *ParentObj, unsigned int num_vertex, unsigned int num_face);

	// Get and Set interfaces
	Vector<Real>		GetPosition(unsigned int index);
	void				GetPosition(Vector<Real> *Bpos);	
	Vector<Real>		GetVelocity(unsigned int index);
	void				GetVelocity(Vector<Real> *Bvel);	
	Vector<Real>		GetReactionForce(unsigned int index);
	void				GetReactionForce(Vector<Real> *Bforce);	

	void				Set(unsigned int index, unsigned int boundary_type, 
							Vector<Real> boundary_value, 
							Real boundary_value2_scalar, Vector<Real> boundary_value2_vector);
	void				Set(unsigned int *boundary_type, 
							Vector<Real> *boundary_value, 
							Real *boundary_value2_scalar, Vector<Real> *boundary_value2_vector);
	void				ResetBoundaryCondition(void);		
};

/**
 * BalloonDomain Class
 * Base balloon domain class
 */
class BalloonDomain : public DeformableSolidDomain {
public:
	void Init(BalloonObject *object) { Object = (DeformableSolidObject*)object; }
	void SetBaseConfiguration(Matrix<Real> config) { ((BalloonObject*)Object)->SetBaseConfiguration(config); }
};

#endif
