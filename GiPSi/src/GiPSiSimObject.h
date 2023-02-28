/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Simulation Object API Definitions (GiPSiSimObject.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu, Suriya Natsupakpong.
*/

////	GiPSiSimObject.h v0.1.0
////
////    Definitions for the part of Core GiPSi API
////			for the simulation objects,
////			base class for connectors, and
////			base class for integrators
////	
////
////////////////////////////////////////////////////////////////


#ifndef _GiPSiSIMOBJECT_H
#define _GiPSiSIMOBJECT_H

#include "GiPSiGeometry.h"
#include "GiPSiDisplay.h"
#include "XMLNode.h"
#include "XMLNodeList.h"

#include <vector>
#include <set>
using std::vector;
using std::set;

using namespace GiPSiXMLWrapper;
// Constant vector definitions
static const Vector<Real>	zero_Vector(3, 0.0);
const Vector<Real>	zero_vector2(2, 0.0);
const Vector<Real>	zero_vector3(3, 0.0);
// Constant vector definitions
static const Matrix<Real>	zero_Matrix(3, 3, 0.0);
const Matrix<Real>	zero_matrix3(3, 3, 0.0);
const Matrix<Real>	zero_matrix4(4, 4, 0.0);

void ToTransformationMatrix(Matrix<Real> &result, const Matrix<Real> &rotation, const Vector<Real> &translation, const Vector<Real> &scale);
void GetRotationMatrix(Matrix<Real> &result, const Matrix<Real> &a);
void GetTranslationVector(Vector<Real> &result, const Matrix<Real> &a);
void ToRotationMatrix(Matrix<Real> &R, const Real &angle, const Real &ax, const Real &ay, const Real &az);

///****************************************************************
// *						SIMULATION OBJECT					  *  
// ****************************************************************

class LoaderUnitTest;

// Base class for a simulatable object
class SIMObject {
public:
	DisplayManager	*displayMngr;

	/**
	 * Constructor.
	 * 
	 * @param simObjNode XML project file 'simObj' node.
	 */
	SIMObject(	XMLNode * simObjNode)
				:	geometry(NULL), boundary(NULL), domain(NULL)
	{
		try
		{
			XMLNodeList * simObjChildren = simObjNode->GetChildren();
			this->name = simObjChildren->GetNode("name")->GetValue();
			this->type = simObjChildren->GetNode("type")->GetValue();
			char * CDCRStr = simObjChildren->GetNode("collision")->GetValue();
			XMLNodeList * NHParametersChildren = simObjChildren->GetNode("objParameters")->GetChildren()->GetNode("NHParameters")->GetChildren();
			char * timeStr = NHParametersChildren->GetNode("time")->GetValue();
			char * timeStepStr = NHParametersChildren->GetNode("timeStep")->GetValue();
			this->time = (Real)atof(timeStr);
			this->timestep = (Real)atof(timeStepStr);			
			maxTimestep = timestep;			
			
			if (strcmp(CDCRStr, "NONE") == 0)
				this->CDCR = 0;
			else if (strcmp(CDCRStr, "CD") == 0)
				this->CDCR = 1;
			else if (strcmp(CDCRStr, "CDCR") == 0)
				this->CDCR = 2;
			else this->CDCR = -1;

			Id = 0;

			delete CDCRStr;
			delete timeStr;
			delete timeStepStr;			
		}
		catch (...)
		{
			throw;
			return;
		}
	}

	/**
	 * Destructor.
	 */
	~SIMObject()
	{
		if (name)
		{
			delete name;
			name = NULL;
		}
		if (displayMngr)
		{
			delete displayMngr;
			displayMngr = NULL;
		}
		if (geometry)
		{
			delete geometry;
			geometry = NULL;
		}
		if (boundary)
		{
			delete boundary;
			boundary = NULL;
		}
		if (domain)
		{
			delete domain;
			domain = NULL;
		}
	}

	// Access to private data
	/**
	 * Return the object name.
	 */
	char*			GetName(void)		const { return name; }
	/**
	 * Set the object name.
	 */
	void			SetName(char *newname)	  { delete[] name;
												name = new char[strlen(newname)+1];
												sprintf(this->name,"%s",newname); }
	/**
	 * Return the object type.
	 */
	char*			GetType(void)		const { return type; }
	/**
	 * Set the object name.
	 */
	void			SetType(char *newtype)	  { delete[] type;
												type = new char[strlen(newtype)+1];
												sprintf(this->type,"%s",newtype); }

	/**
	 * Return timestep the object uses.
	 */
	Real			GetTimestep(void)	const { return timestep; }
	/**
	 * Set timestep the object uses.
	 * 
	 * @param dt New timestep.
	 */
	//void			SetTimestep(Real dt)	{ timestep = dt; }
	/**
	 * Return current object time.
	 */
	Real			GetTime(void)		const { return time; }
	/**
	 * Set current object time.
	 * 
	 * @param t New time..
	 */
	void			SetTime(Real t)			{ time = t; }
	/**
	 * Return object geometry pointer.
	 */
	Geometry*		GetGeometryPtr(void)	const { return geometry; }
	/**
	 * Return object boundary pointer.
	 */
	Boundary*		GetBoundaryPtr(void)	const { return boundary; }
	/**
	 * Return object domain pointer.
	 */
	Domain*			GetDomainPtr(void)		const { return domain; }

	/**
	 * Return CDCR value.
	 */
	bool			isCDEnable(void) { return CDCR && 1; }
	bool			isCREnable(void) { return CDCR && 2; }

	/**
	 * Get and Set ID.
	 */
	void			SetID(int id) { Id = id; }
	int				GetID(void) { return Id; }

	// Loader
	/**
	 * Load object geometry from file.
	 * 
	 * @param filename Formatted file name where geometry is located.
	 */
	virtual void	Load(char* filename) {}

	// Display functions
	/**
	 * Populate DisplayBuffer with latest display data.
	 */
    virtual void	Display(void) {}
	/**
	 * Setup object DisplayManager.
	 */
    virtual void	SetupDisplay(void) {}

	// Haptics API
	/**
	 * Get low order linear haptic model at the specified boundary node.
	 * 
	 * @param BoundaryNodeIndex Index of boundary node.
	 * @param Model Is set to the low order haptic model that is constructed.
	 */
	virtual int		ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model) {return 0; }
	/**
	 * Get low order linear haptic model for the specified face.
	 * 
	 * @param FaceNodeIndex1 Vertex 1 of face.
	 * @param FaceNodeIndex2 Vertex 2 of face.
	 * @param FaceNodeIndex3 Vertex 3 of face.
	 * @param Model Is set to the low order haptic model that is constructed.
	 */
	virtual int		ReturnHapticModel(unsigned int FaceNodeIndex1,
									  unsigned int FaceNodeIndex2,
									  unsigned int FaceNodeIndex3, GiPSiLowOrderLinearHapticModel &Model){return 0;}

	// Simulation API
	/**
	 * Performs one simulation step for the object.
	 */
	virtual void	Simulate(void) {}

	Real			GetMaxTimestep(void)	const { return maxTimestep; }	
	bool			SetTimestep(Real dt)
	{
		bool result = false;		
		if (dt > 0) {
			if (dt <= maxTimestep) {
				timestep = dt;
				result = true;			
			}  		
			else
				timestep = maxTimestep;
		}
		return result;
	}

protected:
	char			*name;				// Name
	char			*type;				// Type
	Geometry		*geometry;			// Display Geometry
	Boundary		*boundary;			// Boundary Geometry
	Domain			*domain;			// Domain Geometry
	Real			time;				// Local time of the object
	Real			timestep;			// The local timestep	
	Real			maxTimestep;		// Maximum local simulation time step
	int				CDCR;				// Collision detection and respinse flag
	int				Id;					// Identity number of simulation object use for CDCR

	friend LoaderUnitTest;
};


// ****************************************************************
// *						BASE CONNECTOR						  *  
// ****************************************************************
class Connector {
public:

	Connector():name(NULL) {}
	~Connector() 
	{
		if (name)
		{
			delete name;
			name = NULL;
		}
	}
	/**
	 * Return the object name.
	 */
	char*			GetName(void)		const { return name; }
	/**
	 * Set the object name.
	 */
	void			SetName(char *newname)	  { delete[] name;
												name = new char[strlen(newname)+1];
												sprintf(this->name,"%s",newname); }	
	virtual void	process(void) {}

protected:
	char		* name;					// Name
	Boundary	* boundaries;			// Boundaries
	Domain		* domains;				// Domains

};


// ****************************************************************
// *						BASE INTEGRATOR CLASS				  *  
// ****************************************************************

// The base Integrator Class
template <class S>
class Integrator {
public:
  typedef typename				S::State State;

  /**
   * Placeholder text.
   */
  virtual void				Integrate(S &system, Real h) {}
};


/****************************************************************
 *				           SOLID OBJECT							*  
 ****************************************************************/
// Base class for solid object
class SolidObject: public SIMObject {
public:
	SolidObject(XMLNode * simObjNode):SIMObject(simObjNode) {}
	~SolidObject() {}

	// Get and Set interfaces for the Boundary and the Domain
	virtual Vector<Real>	GetNodePosition(unsigned int index) { return NULL; } 
	virtual Vector<Real>	GetNodeVelocity(unsigned int index) { return NULL; } 
	virtual Vector<Real>	GetNodeForce(unsigned int index) { return NULL; } 

	virtual void			ResetInitialBoundaryCondition(void) {}

	// Haptics API
	virtual int				ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model) { return 0; }
	virtual int				ReturnHapticModel(unsigned int BoundaryFaceIndex, Vector<Real> BarycentricCoord,
										Vector<Real> position, GiPSiLowOrderLinearHapticModel &Model)  { return 0; }
	// from Paul Model...
	virtual int				ReturnHapticModel(unsigned int FaceNodeIndex1,
										unsigned int FaceNodeIndex2,
										unsigned int FaceNodeIndex3, GiPSiLowOrderLinearHapticModel &Model) { return 0; }
	
	friend LoaderUnitTest;
};

/****************************************************************
 *	 	            DEFORMABLE SOLID OBJECT						*  
 ****************************************************************/
// Base class for deformable solid object
class DeformableSolidObject: public SolidObject {
public:
	DeformableSolidObject(XMLNode * simObjNode):SolidObject(simObjNode) {}
	~DeformableSolidObject() {}
	// Haptics API
	virtual int				ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model) { return 0; }
};

/****************************************************************
 *	 				  RIGID SOLID OBJECT						*  
 ****************************************************************/
// Base class for rigid solid object
class RigidSolidObject: public SolidObject {
public:
	RigidSolidObject(XMLNode * simObjNode):SolidObject(simObjNode) {}
	~RigidSolidObject() {}
	// Haptics API
	virtual int				ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model) { return 0; }
};

/****************************************************************
 *			           COLLISION BOUNDARY						*  
 ****************************************************************/
class CollisionEnabledBoundary : public Boundary {
protected:
	bool			CDEnable;
	bool			CREnable;
	bool			HapticAttached;
public:	
	CollisionEnabledBoundary():CDEnable(false),CREnable(false),HapticAttached(false) { CollisionEnabledBoundaryType = true; }
	void	SetDetection(bool enable) { CDEnable = enable; }
	void	SetResponse(bool enable) { CREnable = enable; }	
	bool	GetDetection(void) { return CDEnable && CollisionEnabledBoundaryType; }
	bool	GetResponse(void) { return CREnable && CollisionEnabledBoundaryType; }

	void	SetHapticAttached(bool enable) { HapticAttached = enable; }
	bool	IsHapticAttached(void) { return HapticAttached; }
	
	virtual Vector<Real>	GetPosition(unsigned int index) = 0;
	virtual void		GetPosition(Vector<Real> *Bpos) = 0;
	virtual Vector<Real>	GetVelocity(unsigned int index) = 0;
	virtual void		GetVelocity(Vector<Real> *Bvel) = 0;
	virtual Vector<Real>	GetReactionForce(unsigned int index) = 0;
	virtual void		GetReactionForce(Vector<Real> *Bforce) = 0;
	
	virtual void	Set(unsigned int index, unsigned int boundary_type, 
								Vector<Real> boundary_value, 
								Real boundary_value2_scalar, Vector<Real> boundary_value2_vector) = 0;
	virtual void	Set(unsigned int *boundary_type, 
								Vector<Real> *boundary_value, 
								Real *boundary_value2_scalar, Vector<Real> *boundary_value2_vector) = 0;

	virtual int		ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model) = 0;
	
	virtual void	ResetBoundaryCondition(void) = 0;
	virtual bool	isTypeOneBoundary(unsigned int index) = 0;
	
	// Pointer to the parent object
	SolidObject		*Object;
};

/****************************************************************
 *						  SOLID BOUNDARY						*  
 ****************************************************************/
// Base solid boundary class
class SolidBoundary : public CollisionEnabledBoundary {
public:
	unsigned int			*global_id;					/**< array of global id */
	unsigned int			*facetovertexids;			/**< array of face to vertex id */
	unsigned int			*boundary_type;				/**< array of boundary type */
	Vector<Real>			*boundary_value;			/**< array of boundary value */
	Real					*boundary_value2_scalar;	/**< array of boundary value 2 scalar */
	Vector<Real>			*boundary_value2_vector;	/**< array of boundary value 2 vector */

public:
	// Get and Set interfaces
	//unsigned int			GetGlobal_id(unsigned int index) { return global_id[index]; }
	//unsigned int			GetBoundary_type(unsigned int index) { return boundary_type[index]; }
	//Vector<Real>			GetBoundary_value(unsigned int index) { return boundary_value[index]; }
	//Real					GetBoundary_value2_scalar(unsigned int index) { return boundary_value2_scalar[index]; }
	//Vector<Real>			GetBoundary_value2_vector(unsigned int index) { return boundary_value2_vector[index]; }
	//DeformableSolidObject	GetObject(void) { return Object; }
	//void					SetGlobal_id(unsigned int index, unsigned int value) { global_id[index] = value; }	
	virtual Vector<Real>	GetPosition(unsigned int index);
	virtual void			GetPosition(Vector<Real> *Bpos);
	virtual Vector<Real>	GetVelocity(unsigned int index);
	virtual void			GetVelocity(Vector<Real> *Bvel);
	virtual Vector<Real>	GetReactionForce(unsigned int index);
	virtual void			GetReactionForce(Vector<Real> *Bforce);

	//void					SetGlobal_id(unsigned int index, unsigned int value) { global_id[index] = value; }
	virtual int				GetBoundaryType(unsigned int index) { return boundary_type[index]; }
	virtual void			Set(unsigned int index, unsigned int boundary_type, 
								Vector<Real> boundary_value, 
								Real boundary_value2_scalar, Vector<Real> boundary_value2_vector);
	virtual void			Set(unsigned int *boundary_type, 
								Vector<Real> *boundary_value, 
								Real *boundary_value2_scalar, Vector<Real> *boundary_value2_vector);

	virtual int				ReturnHapticModel(unsigned int BoundaryNodeIndex, GiPSiLowOrderLinearHapticModel &Model);
	
	virtual void			ResetBoundaryCondition(void);
	virtual bool			isTypeOneBoundary(unsigned int index);
};

/****************************************************************
 *							SOLID DOMAIN						*  
 ****************************************************************/
// Base solid domain class
class SolidDomain : public Domain {
public:
	Matrix<Real>			*DomStress;	/**< Stress matrix */
	SolidObject				*Object;	/**< Pointer to the parent object */	

public:	
	// Get and Set interfaces
	virtual Matrix<Real>	GetStress(unsigned int element_index);
	virtual void			GetStress(Matrix<Real> *StressArray);
	virtual Matrix<Real>	GetStrain(unsigned int element_index);
	virtual void			GetStrain(Matrix<Real> *StrainArray);

	virtual void			SetDomainStress(unsigned int element_index, Matrix<Real> Stress_Tensor);
	virtual void			SetDomainStress(Matrix<Real> *Stress_Tensor_Array);
	virtual void			SetForce(unsigned int node_index, Vector<Real> Force_Vector);
	virtual void			SetForce(Vector<Real> *Force_Vector);
	
	void					ResetDomainVariables(void);
};

/****************************************************************
 *				    DEFORMABLE SOLID BOUNDARY					*  
 ****************************************************************/
// Base deformable solid boundary class
class DeformableSolidBoundary : public SolidBoundary {
};

/****************************************************************
 *	 				  RIGID SOLID BOUNDARY						*  
 ****************************************************************/
// Base rigid solid boundary class
class RigidSolidBoundary : public SolidBoundary {
};

/****************************************************************
 *					DEFORMABLE SOLID DOMAIN						*  
 ****************************************************************/
// Base deformable solid domain class
class DeformableSolidDomain : public SolidDomain {
};

/****************************************************************
 *			 		  RIGID SOLID DOMAIN						*  
 ****************************************************************/
// Base rigid solid domain class
class RigidSolidDomain : public SolidDomain {
};


// ****************************************************************
// *						BASE COLLISION CLASS				  *  
// ****************************************************************

// The base Collision Information Class
class CollisionInfo {
public:
	CollisionEnabledBoundary	*bObj1;			/**< Boundary Object 1 Pointer. */
	CollisionEnabledBoundary	*bObj2;			/**< Boundary Object 2 Pointer. */
	int							numOfContact;	/**< Number of contact point. */
	virtual void				print(void) {}
};

// The Collision Information constainer Class
class CollisionInfos {
private:
	typedef vector<CollisionInfo*> CollisionInfoContainer;
	typedef CollisionInfoContainer::iterator CCIterator;
	CollisionInfoContainer collisionInfos;	
public:
	CCIterator		begin(void) { return collisionInfos.begin(); }
	CCIterator		end(void) { return collisionInfos.end(); }
	int				size(void) { return collisionInfos.size(); }
	void			clear(void) { collisionInfos.clear(); }
	void			add(CollisionInfo * collisionInfo) { collisionInfos.push_back(collisionInfo); }
	CollisionInfo*	get(int n) { return collisionInfos[n]; }
};

// The Collision Enabled Boundary container Class
class CollisionEnabledBoundaries {
private:
	typedef vector<CollisionEnabledBoundary*> CollisionEnabledBoundaryContainer;
	typedef CollisionEnabledBoundaryContainer::iterator CCIterator;
	CollisionEnabledBoundaryContainer collisionEnabledBoundaries;
public:
	CCIterator					begin(void) { return collisionEnabledBoundaries.begin(); }
	CCIterator					end(void) { return collisionEnabledBoundaries.end(); }
	int							size(void) { return collisionEnabledBoundaries.size(); }
	void						add(CollisionEnabledBoundary * collisionEnabledBoundary) { collisionEnabledBoundaries.push_back(collisionEnabledBoundary); }
	CollisionEnabledBoundary*	get(int n) { return collisionEnabledBoundaries[n]; }
};

// The Bounding Volume Info
class BoundingVolumeInfo {
public:
	BoundingVolumeInfo() : bObj1(NULL), bObj2(NULL) {}
	CollisionEnabledBoundary	*bObj1;			/**< Boundary Object 1 Pointer. */
	CollisionEnabledBoundary	*bObj2;			/**< Boundary Object 2 Pointer. */	
	virtual void				print(void) {}
};

// The Bounding Volume Class
class BoundingVolume {
public:
	BoundingVolume():bObject(NULL) {}
	~BoundingVolume() {}
	virtual void setBoundary(CollisionEnabledBoundary * bObj) { bObject = bObj; }
	CollisionEnabledBoundary * getBoundary(void) { return bObject; }
	virtual void build(void) {}
	virtual void refit(void) {}
	virtual int overlappingTest(BoundingVolume * bv, BoundingVolumeInfo * bvInfo) { return 0; }
protected:
	CollisionEnabledBoundary	*bObject;		/**< a boundary object pointer. */
};

// The Bounding Volume container Class
class BoundingVolumes {
private:
	typedef vector<BoundingVolume*> BoundingVolumeContainer;
	typedef BoundingVolumeContainer::iterator BVIterator;
	BoundingVolumeContainer boundingVolumes;
public:
	BVIterator					begin(void) { return boundingVolumes.begin(); }
	BVIterator					end(void) { return boundingVolumes.end(); }
	int							size(void) { return boundingVolumes.size(); }
	void						add(BoundingVolume * boundingVolume) { boundingVolumes.push_back(boundingVolume); }
	BoundingVolume*				get(int n) { return boundingVolumes[n]; }
};

// The Collision Rule Class 
class CollisionRule {
public:
	CollisionRule():rules(NULL),type(false) {}
	~CollisionRule() { if (rules!=NULL) delete rules; }
	void	initialize(int numBoundary); 
	void	initialize(int numBoundary, int numHBoundary); 
	void	setType(bool value) { type = value; }
	bool	getType(void) { return type; }
	void	setRule(const char * rulename, const char * filename, SIMObject * object[], int num_object);
	void	setCollisionTest(int value, int id1, int id2);
	void	setCollisionTest(int value, int id1, int face1, int id2, int face2);
	int		isCollisionTest(int id1, int id2);
	int		isCollisionTest(int id1, int face1, int id2, int face2);
protected:
	int		*rules;			// 0:no collision, 1:obj collision, 2:face collision
	int		length;			// length of rules
	int		numCEBoundary;	// number of collision enabled boundary
	int		numHIOBoundary;	// number of haptic interface object boundary
	bool	type;			// false: normal rule, true: haptic rule
	bool	isMax(int num1, int num2) { return (num1>=num2)? true : false; }
};

// The base Collision Class
class Collision {
public:
	/**
	 * Constructor.
	 */
	Collision():collisionRule(NULL), bvList(NULL) { enabled = false; }
	/**
	 * Set collision rule.
	 */
	virtual void	setBoundingVolume(BoundingVolumes * bvs) { bvList = bvs; }
	/**
	 * Set collision rule.
	 */
	void			setCollisionRule(CollisionRule * rule) { collisionRule = rule; }	
	/**
	 * Is collision is enabled or not.
	 */
	bool			isEnabled(void) { return enabled; }
	/**
	 * Initialize collision.
	 */
	virtual void	initialize(void) { enabled = true; }
	/**
	 * Perform collision detection on all objects.
	 */
	virtual void	detection(void) {}
	/**
	 * Perform collision response on all objects.
	 */
	virtual void	response(void) {}
	/**
	 * Add object boundary to boundary list.
	 * 
	 * @param bObject New object boundary.
	 */
	//virtual void	add(BoundingVolume * bv) { bvList.add(bv); }
protected:
	bool								enabled;
	BoundingVolumes						*bvList;	
	CollisionRule						*collisionRule;
	CollisionInfos						collisionInfos;
};

class HapticCollision: public Collision {
public:
	/**
	 * Constructor.
	 */
	HapticCollision() {}
	/**
	 * Initialize collision.
	 */
	virtual void	initialize(void) {	enabled = true; }
	/**
	 * Set collision rule.
	 */
	virtual void	setHBoundingVolume(BoundingVolumes * hbvs) { hbvList = hbvs; }
	/**
	 * Perform collision detection on all objects.
	 */
	virtual void	detection(void) {}
	/**
	 * Perform collision response on all objects.
	 */
	virtual void	response(void) {}
	/**
	 * Add object boundary to boundary list.
	 * 
	 * @param bObject New object boundary.
	 */
	//virtual void	add(BoundingVolume * bv) { bvList.add(bv); }
	/**
	 * Add haptic object boundary to haptic boundary list.
	 * 
	 * @param hbObject New object boundary.
	 */
	//virtual void	addHIOBV(BoundingVolume * hbv) { hbvList.add(hbv); }
protected:
	BoundingVolumes					*hbvList;	
};

// ****************************************************************
// *						SIMULATION ORDER CLASS				  *  
// ****************************************************************

// The Simulation Order Class
class SimOrder {
public:
	SimOrder() { object = NULL; name = NULL; type = 0; flag = false; }
	void *	getObjectPtr() { return object; }
	char *  getName() { return name; }
	int		getType() { return type; }
	bool	getFlag() { return flag; }	
	void	setObjectPtr(void * ptr) { object = ptr; }
	void	setName(char * val) { name = val; }
	void	setType(int val) { type = val; }
	void	setFlag(bool val) { flag = val; }
protected:
	void	*object;
	char	*name;
	int		type;	// 0=no definded, 1=simObject, 2=connector
	bool	flag;
};

#endif
