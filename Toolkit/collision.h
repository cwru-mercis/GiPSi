/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Collision Detection and Response Definition (collision.h).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2006-2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	COLLISION.H v0.1.0
////
////	Defines:
////		Collision	-	Collision class.
////		BasicCollision
////		HapticCollision
////		PDepthCollision
////
////////////////////////////////////////////////////////////////

#ifndef _COLLISION_H
#define _COLLISION_H

#include <stdio.h>
#include <math.h>

#include "algebra.h"
#include "GiPSiAPI.h"
#include "GiPSiGeometry.h"
#include "GiPSiCompToolset.h"
#include "collision_utils.h"
#include "OPCODE.h"

using namespace Opcode;

/**
 * Basic Collision Information Class.
 */
class BasicCollisionInfo : public CollisionInfo {
public:
	vector<Point>			contacts0;		/**< List of Contact0 points. */
	vector<Point>			contacts1;		/**< List of Contact1 points. */
	vector<Point>			forces;			/**< List of Force for each contact point. */	
};

/**
 * Basic Collision 
 * use triangle-triangle collisions detection 
 *	perform the tri-tri intersection test for all triangles in objects 1 and 2.
 */
class BasicCollision : public Collision {		
public:
	 /**
     * a constructor.
     */
	BasicCollision() {}	
	/**
     * detection function.
     */
	void detection(void);
	/**
     * response function.
     */
	void response(void);
	 /**
     * a print.
     */
	void print(void);
protected:
	/**
     * check collision between triangle and line.
	 * return number of collision.
     */
	int	checkCollisionTL(TriSurface *geom1, TriSurface *geom2, vector<Point> *contacts0);
	int	checkCollisionTL2(TriSurface *geom1, TriSurface *geom2, vector<Point> *contacts0);
	/**
     * check collision between triangle and triangle.
	 * return number of collision.
     */
	int	checkCollisionTT(TriSurface *geom1, TriSurface *geom2, vector<Point> *contacts0, vector<Point> *contacts1);	
};

/** Header of Haptic Collision. */
/**
 * Haptic Collision Information Structure.
 * bObj1=haptics object bObj2=sim object
 */
class HapticCollisionInfo : public CollisionInfo {
public:	
	Point						HTip;			/**< Haptic tip. */
	Point						contacts;		/**< Contact point. */	
	int							faceIndex;		/**< Face index. */	
	int							HapticID;		/**< Haptic ID */
	void						print(void) {}
};

/**
 * Last Haptic Collision to memory the last time of haptic collision.
 */
struct LastHapticCollision 
{
	CollisionEnabledBoundary		*bound;		/**< Boundary Simulation Object Pointer. */
	int								nodeIndex;	/**< Node index. */
	LastHapticCollision():bound(NULL),nodeIndex(-1) {}
};

/**
 * Haptic Collision
 *  Triangle-Line collisions detection 
 *	perform the tri-line intersection test in objects 1 and 2.
 */
class TLHapticCollision : public HapticCollision {		
public:
	 /**
     * a constructor.
     */
	TLHapticCollision() {}
	/**
     * detection function.
     */
	void detection(void);
	/**
     * response function.
     */
	void response(void);
	/**
     * a print.
     */
	void print(void);
protected:		
	/**
     * check collision between triangle and line.
	 * return number of collision. 
     */
	int		checkCollision(TriSurface *Hgeom, TriSurface *geom, Point *HTip, Point *contacts, int *faceIndex);
	/**
	 * find the nearest vertex from point pt
	 */
	unsigned int findNearestVertex(Vector<Real> v, Vector<Real> v0, Vector<Real> v1, Vector<Real> v2);		
	//LastHapticCollision	*lastHapticCollision;
};

/**
 * A vertex face index structure. A structure to keep the face indices that belong to vertex.
 */
struct VertexFaceIndex 
{
	set<int> faceIndexSet;	/**< set of face index. */
};

/**
 * A face owner class. A class to keep the vertexFaceIndex of all vertex.
 */
class FaceOwner
{
public:
    /**
     * a constructor.
     * @param n_vertex an integer argument.
     */
	FaceOwner(int n_vertex);
    /**
     * a destructor.
     */
	~FaceOwner();
	/**
	 * add face_index into the faceIndexList for the vertexFaceIndexArray with vertex_index.
	 * @param vertex_index an integer argument.
	 * @param face_index an integer argument.	 
	 */
	void add(int vertex_index, int face_index);
	/**
	 * get face owner with vertex_index argument.
	 * @param vertex_index an integer argument.
	 */
	set<int> getFaceOwner(int vertex_index);		
	/**
	 * print information of FaceOwner class (for debugging).
	 */
	void print(void);
private:		
	VertexFaceIndex *vertexFaceIndexArray;	/**< a vertexFaceIndexArray variable. */	
	int	num_vertex;							/**< an num_vertex variable. */
};

class AABBBoundingVolumeInfo : public BoundingVolumeInfo {
public:
	AABBBoundingVolumeInfo():NbPairs(0) {}	
	void setNbPairs(unsigned int value) { NbPairs = value; }
	void setCollidingPairs(Pair * value) { CollidingPairs = value; }
	unsigned int getNbPairs(void) { return NbPairs; }
	unsigned int getFaceIndex0(unsigned int i) { return (CollidingPairs+i)->id0; }
	unsigned int getFaceIndex1(unsigned int i) { return (CollidingPairs+i)->id1; }
protected:
	Pair*			CollidingPairs;
	unsigned int	NbPairs; 
};

/**
 * Axis Align Bounding Box Class.
 *  A class for building AABB bounding volume and bv overlapping test.
 */
class AABBBoundingVolume : public BoundingVolume {
public:
	/**
     * a constructor.
     */
	AABBBoundingVolume():opcodeModel(NULL), faceOwner(NULL), iMesh(NULL), tris(NULL), verts(NULL) {}
	 /**
     * a destructor.
     */
	~AABBBoundingVolume();
	/**
	 * set the collision enabled boundary
	 */
	void setBoundary(CollisionEnabledBoundary * bObj);
	/**
	 * build collision model and collect FaceOwner information.
	 */
	void build(void);
	/**
	 * refit the bounding volume
	 */
	void refit(void);	
	/**
	 * overlapping test with another bounding volume
	 */
	int overlappingTest(BoundingVolume * bv, BoundingVolumeInfo * bvInfo, bool firstContact);
	
	Model * getModel(void) { return opcodeModel; }
	/**
	 * get boundary object pointer
	 * @return Boundary pointer
	 */
	CollisionEnabledBoundary *getBoundary(void);
	/**
	 * get face owner pointer
	 * @return FaceOwner pointer
	 */
	FaceOwner *getFaceOwner(void);	
	/**
	 * get triangle information with faceindex argument
	 * @param faceindex an integer argument.
	 * @return v0[3] vertex 0 position.
	 * @return v1[3] vertex 1 position.
	 * @return v2[3] vertex 2 position.
	 * @return vIndex[3] vertex index.
	 */
	void getTriangle(int faceindex, float v0[3], float v1[3], float v2[3], int vIndex[3]);		
private:	
	Model						*opcodeModel;	/**< an Opcode collision model. */	
	FaceOwner					*faceOwner;		/**< a FaceOwner pointer. */	
	MeshInterface				*iMesh;			/**< a MeshInterface of Opcode. */
	IndexedTriangle				*tris;			/**< a IndexedTriangle of Opcode. */
	OPC_Point					*verts;			/**< a OPC_Point of Opcode. */	
	AABBTreeCollider			TC;		
};

/** Header of PDepth Collision. */
/**
 * Face Collision Information Class.
 */
class FaceCollisionInfo {
public:
	/**
     * a constructor.
     */
	FaceCollisionInfo():faceState(0) {}
	/**
     * set face state.
	 * @param state an integer argument.
     */
	void		setState(int state) { faceState = state; }
	/**
     * get face state.
	 * @return face state.
     */
	int			getState(void) { return faceState; }
	/**
     * set vertex index.
	 * @param vIndex vertex index of face.
     */
	void		setVertexIndex(int vIndex[3]);
	/**
     * get vertex index.
	 * @param index of face (0,1,2).
	 * @return vertex index.
     */
	int			getVertexIndex(int index) { return vertexIndex[index]; }
	/**
	 * add face index of other object.
	 * @param faceIndex.
	 */
	void		addOtherObjFaceIndex(int faceIndex);
	/**
	 * get face index of other object.
	 * @return set of face index of other object.
	 */
	set<int>	getOtherObjFaceIndexList(void) { return otherObjFaceIndexList; }
	/**
	 * print.
	 */
	void		print(void);
private:
	/**
	 * face state variable.
	 * 0:“unkown”, face is still unknown (initial state)                              	
	 * 1:“intersect”, face is intersected
	 * 2:“inside”, face is inside other object
	 * 3:“outside”, face is outside other object.
	 */
	int			faceState;		
	int			vertexIndex[3];			/**< vertexIndex variable. */
	set<int>	otherObjFaceIndexList;	/**< set of face index of other object/ */
};

/**
 * Vertex Collision Information Class.
 */
class VertexCollisionInfo {
public:
	/**
     * a constructor.
	 * @param index.
	 * @param vertex position.
     */
	VertexCollisionInfo(int index, Point pt):vertexIndex(index), point(pt), vertexState(0), counter(0),penetrationDepth(0.0) {}
	/**
	 * set vertex position.
	 * @param point.
	 */
	void setPoint(Point pt);
	/**
	 * get vertex position.
	 * @return point.
	 */
	Point getPoint(void) { return point; }
	/**
	 * get vertex index.
	 * @return vertex index.
	 */
	int getIndex(void) { return vertexIndex; }
	/**
	 * update vertex state.
	 */
	void updateState(void);
	/**
	 * set vertex state.
	 * @param vertex state.
	 */
	void setState(int state) { vertexState = state; }
	/**
	 * get vertex state.
	 * @return vertex state.
	 */
	int	getState(void) { return vertexState; }
	/** 
	 * check vertex index is index or not.
	 * @param vertex index
	 * @return true,if index==vertex index, otherwise false.
	 */
	bool isVertexIndex(int index) { return (index==vertexIndex); }
	/**
	 * add intersection point, normal vector, and penetration depth.
	 * @param intersection point.
	 * @param normal vector.
	 * @param penetration depth (default=0).
	 */
	void addIntersectionPoint(Point iPoint, Point n, Real pDepth = 0);
	/**
	 * calculate penetration depth.
	 * if num>1 the penetration depth will be distributed
	 * @param num (default=1)
	 */
	void calculatePDepth(int num=1);	
	/**
	 * get penetration depth.
	 * @return penetration depth.
	 */
	Real getPDepth(void) { return penetrationDepth; }
	/** 
	 * get penetration direction.
	 * @return penetration direction.
	 */
	Vector<Real> getPDepthDir(void) { return penetrationDirection; }
	/**
	 * get penetration force.
	 * @return penetration force.
	 */
	Vector<Real> getPDepthForce(void) { return penetrationForce; }
	/**
	 * add force.
	 * @param force.
	 */
	void addRForce(Vector<Real> rForce) { penetrationForce = penetrationForce+rForce; }
	/**
	 * increase counter.
	 */
	void incCounter(void) { counter++; }
	/**
	 * decrease counter.
	 */
	void decCounter(void) { counter--; }
	/** 
	 * print.
	 */
	void print(void);
private:
	Point point;		/**< vertex position */
	int	vertexIndex;	/**< vertex index */
	/**
	 * vertex state.
	 * 0:“unkown”, vertex is still unknown (initial state)
	 * 1:“non-colliding”, vertex is non-colliding
	 * 2:“border”, vertex is in the border	
	 * 3:“processing”, vertex is in the processing
	 * 4:“a_border”, vertex is the artificial border.
	 */
	int				vertexState;		
	int				counter; 				/**< vertex counter */
	Real			penetrationDepth;		/**< penetration depth: d(p) */
	Vector<Real>	penetrationDirection;	/**< penetration direction: r(p) */
	Vector<Real>	penetrationForce;		/**< penetration force: f(p) */		
	vector<Point>	normalVectorList;		/**< list of normal vector: n(i) */
	vector<Point>	intersectionPointList;	/**< list of intersection point: x(i) */
	vector<Real>	pDepth_iPointList;		/**< list of penetration depth: d(q) in propagation step, otherwise they are zero */
};

/**
 * Penetration Depth Collision Information Class.
 */
class PDepthCollisionInfo : public CollisionInfo {
public:
	/**
     * a constructor.
	 * initial PDepthCollisionInfo.
	 * @param object 1 boundary.
	 * @param object 2 boundary.
	 * @param face owner 1.
	 * @param face owner 2.	 
     */
	PDepthCollisionInfo(CollisionEnabledBoundary *bObject1, CollisionEnabledBoundary *bObject2, FaceOwner *fOwner1, FaceOwner *fOwner2); 
	/**
     * a destructor.	 
     */
	~PDepthCollisionInfo();
	/**
	 * check the object 1 and 2 boundaries are the same as arguments or not.
	 * @param bObject1.
	 * @param bObject2.
	 * @return boolean.
	 */
	bool isInPDepthCollisionInfo(CollisionEnabledBoundary *bObject1, CollisionEnabledBoundary *bObject2);
	/**
	 * check the index is in the vertexCollisionInfo or not.
	 * @param vertex index.
	 * @param list of vertex collision info.
	 * @return index in vertex collision info, otherwise return -1.
	 */
	int	isInVertexCollisionInfo(int index, vector<VertexCollisionInfo*> vCollisionInfoList);
	/**
	 * clear processFaceList, nextProcessFaceList, vertexCollisionInfoList for objects 1 and 2
	 */
	void reset(void);
	/**
	 * allocate memory for face collision info for objects 1 and 2.
	 */
	void allocFaceCollisionInfo(void);
	/**
	 * set face state.
	 * @param objIndex.
	 * @param faceIndex.
	 * @param faceState.
	 */
	void setFaceState(int objIndex, int faceIndex, int faceState);
	/**
	 * set face index of other object.
	 * @param objIndex.
	 * @param faceIndex.
	 * @param otherObjFaceIndex.
	 */
	void setOtherObjFace(int objIndex, int faceIndex, int otherObjFaceIndex);
	/**
	 * set vertex index in face collision info.
	 * @param objIndex.
	 * @param faceIndex.
	 * @param vIndex[3].
	 */
	void setVertexIndexOfFaceCollisionInfo(int objIndex, int faceIndex, int vIndex[3]);
	/**
	 * add vertex collision info in collision info list.
	 * @param vPt[3] 3 vertices of object 1 face.
	 * @param uPt[3] 3 vertices of object 2 face.
	 * @param collidedCode from tri_tri_intersect_with_isectline_detail function.
	 * @param codeT1 from tri_tri_intersect_with_isectline_detail function.
	 * @param codeT2 tri_tri_intersect_with_isectline_detail function.
	 * @param codeTT1 tri_tri_intersect_with_isectline_detail function.
	 * @param codeTT2 tri_tri_intersect_with_isectline_detail function.
	 * @param vIndex[3] vertex indices of object 1.
	 * @param uIndex[3] vertex indices of object 2.
	 * @param nv normal vector of object 1 face.
	 * @param nu normal vector of object 2 face.
	 * @param pt1 intersection point 1.
	 * @param pt2 intersection point 2.
	 */
	void addVertexCollisionInfo(Point vPt[3],Point uPt[3], int collidedCode, int codeT1, int codeT2, int codeTT1, int codeTT2, 
								   int vIndex[3], int uIndex[3], Point nv, Point nu, Point pt1, Point pt2);
	/**
	 * calculate the penetration depth.
	 */
	void calculate(void);
	/**
	 * prapagation step.
	 */
	void propagate(void);
	/**
	 * resolve Artificial border.
	 */
	void resolveArtificialVertex(void);
	/**
	 * set constant.
	 * @param CC1, kk1, CC2, kk2.
	 */
	void setConstant(Real CC1, Real kk1, Real CC2, Real kk2);
	/**
	 * calculate total force.
	 */
	void calTotalForce(void);
	/** 
	 * set force back to boundary.
	 */
	void collision2Bound(void);
	/**
	 * update face state for display.
	 * @param objIndex.
	 * @param faceIndex.
	 * @param faceState.
	 */
	void updateFaceStateDisplay(int objIndex, int faceIndex, int faceState);
	/**
	 * update vertex state for display.
	 * @param objIndex.
	 * @param vertexIndex.
	 * @param pForce.
	 */	
	void updateVertexStateDisplay(int objIndex, int vertexIndex, int vertexState);	
	/**
	 * update display.
	 */
	void updateDisplay(int mode);

	void print(void) {}
	set<int> processFaceList_1;		/**< process face list 1 */
	set<int> nextProcessFaceList_1; /**< process face list 2 */
	set<int> processFaceList_2;		/**< next process face list 1 */ 
	set<int> nextProcessFaceList_2;	/**< next process face list 2 */ 
private:
	CollisionEnabledBoundary		*bObj1;						/**< Boundary Object 1 Pointer */
	CollisionEnabledBoundary		*bObj2;						/**< Boundary Object 2 Pointer */
	FaceOwner						*faceOwner1;				/**< Face Owner 1 Pointer */
	FaceOwner						*faceOwner2;				/**< Face Owner 2 Pointer */
	Real							C1, k1, totalForce_1;		/** constant for object 1 */
	Real							C2, k2, totalForce_2;		/** constant for object 2 */		
	FaceCollisionInfo				*faceCollisionInfoArray_1;	/**< array of face collision info of object 1 */
	vector<VertexCollisionInfo*>	vertexCollisionInfoList_1;	/**< list of vertex collision info of object 1 */
	FaceCollisionInfo				*faceCollisionInfoArray_2;	/**< array of face collision info of object 2 */
	vector<VertexCollisionInfo*>	vertexCollisionInfoList_2;	/**< list of vertex collision info of object 2 */
};

/**
 * Penetration Depth Collisions
 *  perform collision response : the penetration depth.
 */
class PDepthCollision : public Collision {	
public:
	/**
     * a constructor.
	 */
	PDepthCollision() {}	
	/**
	 * Initialize collision.
	 */
	void	initialize(void);
	/**
     * detection function.
     */
	void detection(void);
	/**
     * response function.
     */
	void response(void);
	/**
	 * print.
	 */
	void print(void);	
private:		
	/**
	 * check if this collisionInfo is in the collisionInfoList or not.
	 * @param bObj1.
	 * @param bObj2.
	 * @return -1 is not in the list, otherwise the index of collisionInfo.
	 */
	int	isInCollisionInfos(CollisionEnabledBoundary *bObj1, CollisionEnabledBoundary *bObj2);						
	//CollisionModels					collisionModels;		/**< list of collisionModel	*/
};

#endif