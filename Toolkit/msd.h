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

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu, Suriya Natsupakpong.
*/

////	MSD.H v0.1.0
////
////	Defines:
////		Spring		-	Spring class
////		MSDObject	-	Mass-Spring-Damper (MSD) class.
////		MSDBoundary -	Mass-Spring-Damper Boundary class.
////
////////////////////////////////////////////////////////////////

#ifndef _MSD_H
#define _MSD_H

#include <vector>
#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

class LoaderUnitTest;
class MSDObject;

/**
 * MassNode Class.
 * Base class of massnode which collects the spring index that connected to mass node.
 */
/*
class MassNode {
public:	
	MassNode();
	~MassNode();
	unsigned int	getNumberSpring(void);				// get number of spring
	unsigned int	getSpringIndex(unsigned int num);	// get spring index
	void			addSpringIndex(unsigned int index);	// add and set spring index
	unsigned int	refid;								// Reference id
protected:	
	void			resize(void);						// resize the spring array
	bool			isInSpringIndex(unsigned int index);// check if index is already in spring_index or not
	unsigned int	num_spring;							// number of connected spring
	unsigned int	*spring_index;						// array of spring index	
};
*/
/**
 * MSDModel Class.
 * Base class of MSDModel which uses for msd_haptic model.
 * Type: 0=contact, 1=internal, 2=boundary
 */
/*
class MSDModel {
public:
	MSDModel();
	~MSDModel();
	unsigned int	num_mass[3];		//number of mass in each type
	unsigned int	num_spring[3];		//number of spring in each type
	unsigned int*	mass_index[3];		//mass indices in each mass type 
	unsigned int*	spring_index[3];	//spring indices in each spring type
	void			addSpringIndex(unsigned int type, unsigned int index);
	void			addMassIndex(unsigned int type, unsigned int index);
	unsigned int	getNumMass(unsigned int type) { return num_mass[type]; }
	unsigned int	getNumSpring(unsigned int type) { return num_spring[type]; }	
	unsigned int*	getMassIndex(unsigned int type);
	unsigned int*	getSpringIndex(unsigned int type);
	unsigned int	getMassIndex(unsigned int type, unsigned int num);
	unsigned int	getSpringIndex(unsigned int type, unsigned int num);
	int				getMassNum(unsigned int type, unsigned int index);
	int				getSpringNum(unsigned int type, unsigned int index);
	bool			isInSpringIndex(unsigned int type, unsigned int index);
	bool			isInMassIndex(unsigned int type, unsigned int index);
	void			refit(void);
};
*/

/**
 * MassNode Class.
 * Base class of massnode which collects the spring index that connected to mass node.
 */
class MassNode {
public:
	MassNode();
	unsigned int	refid;	
	unsigned int	getSpringSize() { return spring_index.size(); }
	void			addSpringIndex(unsigned int index);
	unsigned int	getSpringIndex(unsigned int num);
	bool			isInSpringIndex(unsigned int index);
protected:
	vector<unsigned int>	spring_index;
};

/**
 * MSDModel Class.
 * Base class of MSDModel which uses for msd_haptic model.
 * Type: 0=contact, 1=internal, 2=boundary
 */
class MSDModel {
public:
	MSDModel();
	vector<unsigned int>	mass_index[3];		//mass indices in each mass type 
	vector<unsigned int>	spring_index[3];	//spring indices in each spring type
	void					addSpringIndex(unsigned int type, unsigned int index);
	void					addMassIndex(unsigned int type, unsigned int index);
	unsigned int			getMassSize(unsigned int type) { return mass_index[type].size(); }
	unsigned int			getSpringSize(unsigned int type) { return spring_index[type].size(); }	
	vector<unsigned int>	getMassIndex(unsigned int type);
	vector<unsigned int>	getSpringIndex(unsigned int type);
	unsigned int			getMassIndex(unsigned int type, unsigned int num);		//get global index from local num
	unsigned int			getSpringIndex(unsigned int type, unsigned int num);	//get global index from local num
	int						getMassNum(unsigned int type, unsigned int index);		//get local num from global index
	int						getSpringNum(unsigned int type, unsigned int index);	//get local num from global index	
	bool					isInSpringIndex(unsigned int type, unsigned int index);
	bool					isInMassIndex(unsigned int type, unsigned int index);
};

/**
 * MSDModelBuilder Class.
 * Base class of MSDModelBuilder which create all msdModel.
 */
class MSDModelBuilder {
public:
	MSDModelBuilder();
	~MSDModelBuilder();
	void			BuildModel(MSDModel* msdModel, MSDObject* msd, unsigned int depth);	
	void			print(MSDModel* msdModel);
protected:			
	unsigned int	depth;
	unsigned int	total_msdModel;
	MassNode*		massnode;	
	MSDObject*		msd;
};

/** 
 * Spring Class.
 * Base class of spring.
 */
class Spring : public Element {
public:
	unsigned int	node[2];	/**< node index */
	Vector<Real>	dir;		/**< Direction of the spring */
	Real			l_zero;		/**< Rest length */
	Real			k_stiff;	/**< Stiffness of the string */
	Real			b_damp;		/**< Internal damping of the spring */

	/**
	 * Constructors
	 * initilize the dir.
	 */
	Spring() : dir(3,0.0) { }

	/**
	 * Spring::length()
	 * @return real the length of the spring.
	 */
	Real			length() { return dir.length(); }

	// Additional functions
	void			setProperties(Real lzero, Real kstiff, Real bdamp);
	void			setMassIndex(int m1, int m2);	
	Real			getEnergy();
};

/**
 * MSDObject Class.
 * Base class for Mass Spring Damper Object.
 */
class MSDObject: public DeformableSolidObject {
public:
	
	/**< State parameter. the state information for the MSD */
	typedef struct {
		Vector<Real>	*POS;		/**< Global position vector */
		Vector<Real>	*pos;		/**< position of nodes	[cm] */
		Vector<Real>	*VEL;		/**< Global velocity vector */
		Vector<Real>	*vel;		/**< velocity of nodes	[cm/s] */			
		unsigned int	size;		/**< state size */
	} State;

	/**< Jacobian parameter for the MSD with Implicit method */
	typedef struct 
	{
		Matrix<Real>	*A11;		/**< Matrix A11  : 3*size x 3*size */
		Matrix<Real>	*A12;		/**< Matrix A12  : 3*size x 3*size */
		Real			dA21;		/**< Real dA21 : value of diagonal matrix A21 */
		Real			dA22;		/**< Real dA22 : value of diagonal matrix A22 */
		unsigned int	size;		/**< Jabobian size (=state size) */
	} Jacobian;

	// Constructors
	MSDObject(	XMLNode * simObjectNode,
				Real g			= 0.0);

	void SetParameters(XMLNodeList * simObjectChildren);
	void InitializeVisualization(XMLNodeList * simObjectChildren);
	void LoadGeometry(XMLNodeList * simObjectChildren);
	void InitializeTransformation(XMLNodeList * simObjectChildren);
	void LoadModelFiles(XMLNodeList * simObjectChildren);

	// Force calculators
	void				UpdateForces(State	&state);	
	void				UpdateForces(unsigned int index);	
	void				GravityForce(void);
	void				SpringForce(void);
	void				DampingForce(void);
	void				ExternalForce(void);

	// Integrator interface
	void				AccumState(State &new_state, const State &state, const State &deriv, const Real &h);
	void				DerivState(State &deriv, State &state);
	void				AllocState(State &s);
	void				Simulate(void);

	void				AllocJacobian(Jacobian &J);
	void				AddState(State &new_state, const State &state1, const State &state2, const Real h);
	void				ScaleState(State &new_state, const State &state, const Real h);
	Real				NormState(const State &state);
	Real				StateDotState(const State &state1, const State &state2);
	void				MultiplyJacobianState(State &out_state, const Jacobian &J, const State &state);
	void				IdentityMinushJacobian(Jacobian &J, const State &state, const Real h); 
	void				AccumStateSemiExplicit(State &new_state, const State &state, const State &deriv, const Real &h);
	void				PrintJacobian(const Jacobian &J);
	void				PrintState(const State &state);

	void				SetIntegrationMethod(int method);
	int					getIntegrationMethod(const char * method);

	// Get and Set interfaces for the Boundary and the Domain
	Vector<Real>		GetNodePosition(unsigned int index);
	Vector<Real>		GetNodeVelocity(unsigned int index);
	Vector<Real>		GetNodeForce(unsigned int index);

	// Display functions
	void				Display(void);
    void				SetupDisplay(XMLNodeList * simObjectChildren);

	void				Geom2State(void);
	void				State2Geom(void);
	void				State2Bound(void);
	void				ResetInitialBoundaryCondition(void);	

	// Transform massPoint
	void				Translate(float tx, float ty, float tz);
	void				Translate(Vector<Real> p);
	void				Rotate(Real angle, Real ax, Real ay, Real az);
	void				Rotate(Matrix<Real> R);
	void				Scale(float sx, float sy, float sz);


	State&				GetState(void)	{	return	state; }	

	// Utitity functions
	Real				getEnergy(void);
	unsigned int		getNumMass(void) { return num_mass; }
	unsigned int		getNumSpring(void) { return num_spring; }
	Spring				getSpring(unsigned int index) { return spring[index]; }
	bool				isFixedBoundary(unsigned int index);
	int					getMSDIndex(unsigned int OBJIndex);
	int					getOBJIndex(unsigned int MSDIndex);
	// Set IC for doing experiment, user hard code
	void				setInitialCondition(void);

	// Haptics API
	int					ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model);
	int					ReturnHapticModel(unsigned int BoundaryFaceIndex, Vector<Real> BarycentricCoord,
											Vector<Real> position, GiPSiLowOrderLinearHapticModel &Model)  {return 0; }
	// from Paul Model...
	int					ReturnHapticModel(unsigned int FaceNodeIndex1,
											unsigned int FaceNodeIndex2,
											unsigned int FaceNodeIndex3, GiPSiLowOrderLinearHapticModel &Model);

	
protected: // protected methods.
	// Loaders
	void				Load(const char *filename);		// .OBJ Loader
	void				LoadMAP(const char *filename);	// .MAP Loader
	void				LoadMSD(const char *filename);	// .MSD Loader
	void				Init(void);					// Initilization MSD Model

protected: // protected variables.
	//---[sxn66]---> should be remove due to no use the global value of K D <---//
	//const					Real K;			// Global stiffness of the mesh			[g/s2]
	//const					Real D;			// Global damping of the mesh			[g/s]
	const					Real g;			// Local copy of the gravity			[cm/s2]
	//Real					defaultMass;	// The default mass value of the nodes	[g]

	State						state;			/**< state information */	
	Vector<Real>				*force;			/**< Force vector	[g cm/s2 = 10 N] */
	Real						*mass;			/**< Mass vector	[g] */
	Point						*massPoint;		/**< Position of Mass */
	Spring						*spring;		/**< spring */	
	unsigned int				num_mass;		/**< number of mass */
    unsigned int				num_spring;		/**< number of spring */
	MSDModelBuilder				msdModelBuilder;
	MSDModel					*msdModel;

	Vector<Real>				*ground_pos;	/**< ground position */
	Spring						*vspring;		/**< virtual spring */
	unsigned int				num_ground;		/**< number of ground mass */
	unsigned int				num_vspring;	/**< number of virtual spring */
	
	unsigned int				*fix_boundary;	/**< fix boundary */
	unsigned int				num_boundary;	/**< number of fix boundary */

	unsigned int				*mapping;		/**< mapping array size = 2*num_mapping */
	unsigned int				num_mapping;	/**< number of mapping */

	Integrator<MSDObject>		*integrator;	/**< intergrator pointer */
	unsigned int				int_method;		/**< integrator method */

	Real						initialcolor[4];/**< Initial color specified in constructor. It is used in the Load() function */
	
	friend LoaderUnitTest;
};

/**
 * MSDBoundary Class
 * Base MSD boundary class
 */
class MSDBoundary : public DeformableSolidBoundary {
public:
	// Initializer
	void				Init(MSDObject *ParentObj, unsigned int num_vertex, unsigned int num_face);

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
 * MSDDomain class
 * Base MSD domain class. Not implement yet. The sturcture comes from FEMDomain class.
 */
class MSDDomain : public DeformableSolidDomain {
public:
	// Initializer
	void				Init(MSDObject *ParentObj);

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
