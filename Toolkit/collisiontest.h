/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Collision Test Object Model Definition (collisiontest.h).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	COLLISIONTEST.H v0.1.0
////
////	Defines:
////		CollisionTestObject	-	Collision test object class.
////
////////////////////////////////////////////////////////////////

#ifndef _COLLISIONTEST_H
#define _COLLISIONTEST_H

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"

/**
 * A collision test object class. This collision test object does general propose of collision debugging.
 */
class CollisionTestObject: public SolidObject {
public:
	Geometry		*VF_geometry;			/**< Display Vector Field Geometry */
	
	/**
	 * a state info for the CollisionTestObject.
	 */
	typedef struct {
		Vector<Real>			*pos;		/**< position of nodes [cm] */
		Vector<Real>			*vel;		/**< velocity of nodes [cm/s] */
		unsigned int			size;		/**< size of state */
	} State;


	/**
	 * a constructor.
	 * @param name of SIMObject.
	 * @param RGBAcolor colore of SIMObject.
	 * @param time start time.
	 * @param timestep of SIMObject.
	 */
	CollisionTestObject(	XMLNode * simObjectNode);

	/**
	 * Initialize visualization parameters
	 */
	void InitializeVisualization(XMLNodeList * simObjectChildren);	
	void LoadGeometry(XMLNodeList * simObjectChildren);
	void InitializeTransformation(XMLNodeList * simObjectChildren);
	void LoadModelFiles(XMLNodeList * simObjectChildren);

	// Loader interface
	/**
	 * a load function. to load model file into SIMObject.
	 */
	void				Load(char *filename);
	/**
	 * a init function. to initilize model.
	 */
	void				Init(void);

	/**
	 * a update force function. force calculators.
	 * @param state.
	 */
	void				UpdateForces(State	&state);	

	// Integrator interface
	/**
	 * a accumulate state calculation function.
	 * @param new_state.
	 * @param state.
	 * @param deriv state.
	 * @param h time step.
	 */
	void				AccumState(State &new_state, const State &state, const State &deriv, const Real &h);
	/**
	 * a derive state calculation function.
	 * @param derive state.
	 * @param state.
	 */
	void				DerivState(State &deriv, State &state);
	/**
	 * a memory allocation for state.
	 * @param s state
	 */
	void				AllocState(State &s);
	/**
	 * a simulate function.
	 */
	void				Simulate(void);

	// Get and Set interfaces for the Boundary and the Domain
	/**
	 * a get node position function.
	 * @param index of node.
	 * @return position.
	 */
	Vector<Real>	GetNodePosition(unsigned int index);
	/**
	 * a get node velocity function.
	 * @param index of node.
	 * @return velocity.
	 */
	Vector<Real>	GetNodeVelocity(unsigned int index);
	/**
	 * a get node force function.
	 * @param index of node.
	 * @return force.
	 */
	Vector<Real>	GetNodeForce(unsigned int index);

	// Display functions
	/**
	 * a display function.
	 */
	void				Display(void);
	/**
	 * a setup display function.
	 */
    void				SetupDisplay(XMLNodeList * simObjectChildren);

	/** 
	 * update the states from current geometry - this is used for initialization.
	 */
	void				Geom2State(void);

	/**
	 * updating Geometry from current state.
	 */
	void				State2Geom(void);
	/**
	 * updating Boundary from current state.
	 */
	void				State2Bound(void);
	void				Bound2State(void);
	/**
	 * get current state.
	 * @return state.
	 */
	State&				GetState(void)	{	return	state; }
	/**
	 * get initilize color
	 * @return color
	 */
	Vector<Real>		GetInitColor(void);
	/**
	 * set vector field flag - to display vector field.
	 * @param boolean
	 */
	void				setVF(bool value) { VF_flag = value; }
	/** 
	 * get vector fiedl flag.
	 * @return boolean
	 */
	bool				getVF(void) { return VF_flag; }

	void				ResetInitialBoundaryCondition();

	bool				isBoundaryFix(void) { return boundaryfix; }

protected:
	State							state;			/**< State variable */

	const							Real g;
	Vector<Real>					*force;			/**< Force vector [g cm/s2 = 10 N] */
	Real							mass;
	bool							boundaryfix;	/**< boundary is fix in space or not */

	unsigned int					num_vertex;		/**< number of vertex */
	unsigned int					num_boundary;	/**< number of boundary */

	bool							VF_flag;		/**< vector field flag variable */


	Integrator<CollisionTestObject>	*integrator;	/**< integrator */

	Real							initialcolor[4];/**< initial color specified in constructor - it is used in the Load() function  */

	DisplayManager					*VFdisplayMngr;

};

/**
 * A Collision Test boundary class.
 */
class CollisionTestBoundary : public CollisionEnabledBoundary {
public:
	unsigned int		*global_id;			/**< global id */
	unsigned int		*facetovertexids;	/**< face to vertex id */
	unsigned int		*boundary_type;		/**< boundary type */
	Vector<Real>		*boundary_value;	/**< boundary value */
	Real				*boundary_value2_scalar;	/**< boundary value 2 scalar */
	Vector<Real>		*boundary_value2_vector;	/**< boundary value 2 vector */

	/**
	 * Initializer.
	 * @param parent object pointer.
	 * @param number of vertex.
	 * @param number of face.
	 */
	void				init(CollisionTestObject *ParentObj, unsigned int num_vertex, unsigned int num_face);

	// Get and Set interfaces
	/**
	 * get position of vertex index.
	 * @param index.
	 * @return position.
	 */
	Vector<Real>		GetPosition(unsigned int index);
	/**
	 * get position of all vertices
	 * @return all position
	 */
	void				GetPosition(Vector<Real> *Bpos);
	/**
	 * get velocity of vertex index.
	 * @param index.
	 * @return velocity.
	 */
	Vector<Real>		GetVelocity(unsigned int index);
	/**
	 * get velocity of all vertices.
	 * @return all velocity.
	 */
	void				GetVelocity(Vector<Real> *Bvel);	
	/**
	 * get reaction force of vertex index.
	 * @param index.
	 * @return reaction force.
	 */
	Vector<Real>		GetReactionForce(unsigned int index);
	/**
	 * get reaction force of all vertices.
	 * @return all reaction force.
	 */
	void				GetReactionForce(Vector<Real> *Bforce);	
	/**
	 * set boundary for vertex index.
	 */
	void				Set(unsigned int index, unsigned int boundary_type, 
							Vector<Real> boundary_value, 
							Real boundary_value2_scalar, Vector<Real> boundary_value2_vector);
	/**
	 * set boundary for all vertices.
	 */
	void				Set(unsigned int *boundary_type, 
							Vector<Real> *boundary_value, 
							Real *boundary_value2_scalar, Vector<Real> *boundary_value2_vector);
	/**
	 * set vector field for vertex index.
	 */
	void				SetVF(unsigned int index, Vector<Real> boundary_value);

	/**
	 * reset boundary state.
	 */
	void				ResetBoundState(void);
	/**
	 * update current boundary to geometry
	 */
	void				Bound2Geom(void);
	/**
	 * update current bounary to state
	 */
	void				Bound2State(void);

	virtual void		ResetBoundaryCondition(void);
	virtual bool		isTypeOneBoundary(unsigned int index);
	virtual int			ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model) { return 0; }
protected:
};

#endif
