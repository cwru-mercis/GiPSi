/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for Quasi Static Decouple Spring Model (qsds.h).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	QSDS.H v0.1.0
////
////	Defines:
////		QSDSObject	-	Quasi Static Decouple Spring class.
////		QSDSBoundary -	Quasi Static Decouple Spring Boundary class.
////
////////////////////////////////////////////////////////////////

#ifndef _QSDS_H
#define _QSDS_H

#include <vector>
#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "XMLNode.h"
#include "msd.h"

using namespace GiPSiXMLWrapper;

class LoaderUnitTest;
extern void vprint(Vector<Real> vec);
extern void vprint(Matrix<Real> mat);

/**
 * QSDSObject Class.
 * Base class for Quasi Static Decouple Spring Object.
 * vertices and faces in boundary and geometry are shared
 */
class QSDSObject: public DeformableSolidObject {
public:
	
	/**< State parameter. the state information for the QSDS */
	typedef struct {		
		Vector<Real>	*pos;		/**< position of nodes	[cm] */	
		Vector<Real>	*vel;		/**< velocity of nodes	[cm/s] */	
		unsigned int	size;		/**< state size */
	} State;

	// Constructors
	QSDSObject(	XMLNode * simObjectNode);

	void SetParameters(XMLNodeList * simObjectChildren);
	void InitializeVisualization(XMLNodeList * simObjectChildren);
	void LoadGeometry(XMLNodeList * simObjectChildren);
	void InitializeTransformation(XMLNodeList * simObjectChildren);	

	void			Simulate(void);
	
	// Get and Set interfaces for the Boundary and the Domain	
	Vector<Real>	GetNodePosition(unsigned int index);
	Vector<Real>	GetNodeVelocity(unsigned int index);
	Vector<Real>	GetNodeForce(unsigned int index);

	// Display functions
	void			Display(void);
    void			SetupDisplay(XMLNodeList * simObjectChildren);

	void			Bound2State(void);	
	void			InitGeom2State(void);
	void			State2Geom(void);
	void			State2Bound(void);
	void			ResetInitialBoundaryCondition(void);	
	
	// Haptics API
	int				ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model);	
	int				ReturnHapticModel(unsigned int BoundaryFaceIndex, Vector<Real> BarycentricCoord,
										Vector<Real> position, GiPSiLowOrderLinearHapticModel &Model)  {return 0; }
	
protected: // protected methods.
	// Loaders
	void			Load(const char *filename);		// .OBJ Loader
	void			LoadQSDS(const char *filename);	// .QSDS Loader
	void			Init(void);						// Initilization MSD Model

protected: // protected variables.	
	State			state;			/**< state information */
	Real			mass;			/**< Mass [g] */	
	Spring			spring;			/**< spring */	
	Geometry		*init_geometry;	/**< initial geometry */
	
	Real			initialcolor[4];/**< Initial color specified in constructor. It is used in the Load() function */
	
	friend LoaderUnitTest;
};

/**
 * QSDSBoundary Class
 * Base QSDS boundary class
 */
class QSDSBoundary : public DeformableSolidBoundary {
public:
	// Initializer
	void				init(QSDSObject *ParentObj, unsigned int num_vertex, unsigned int num_face);

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
	int					ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model);
	
	//from Paul model...
	int					ReturnHapticModel(unsigned int FaceNodeIndex1,
									  unsigned int FaceNodeIndex2,
									  unsigned int FaceNodeIndex3, GiPSiLowOrderLinearHapticModel &Model);
};

/**
 * QSDSDomain class
 * Base QSDS domain class. Not implement yet. The sturcture comes from FEMDomain class.
 */
class QSDSDomain : public DeformableSolidDomain {
public:
	// Initializer
	void				init(QSDSObject *ParentObj);

	// Get and Set interfaces
	Matrix<Real>		GetStress(unsigned int element_index) { return zero_Matrix; }
	void				GetStress(Matrix<Real> *StressArray) {}
	Matrix<Real>		GetStrain(unsigned int element_index) { return zero_Matrix; }
	void				GetStrain(Matrix<Real> *StrainArray) {} 

	void				SetDomainStress(unsigned int element_index, Matrix<Real> Stress_Tensor);
	void				SetDomainStress(Matrix<Real> *Stress_Tensor_Array);
	void				SetForce(unsigned int node_index, Vector<Real> Force_Vector) {}
	void				SetForce(Vector<Real> *Force_Vector) {}
};

#endif
