/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Collision Response with Penetration Depth Method Implementation (collisionPDepth.h).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2006-2008.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	COLLISIONPDEPTH.cpp	v0.0
////
////	Implementation of:
////		Bounding volume test with AABB method
////		Collision response with penetration depth method.
////
////////////////////////////////////////////////////////////////

#include "collision.h"
#include "collisiontest.h"

AABBBoundingVolume::~AABBBoundingVolume()
{
	if(faceOwner) {
		delete faceOwner;
		faceOwner = NULL;
	}
	if(iMesh) {
		delete iMesh;
		iMesh = NULL;
	}
	if(verts) {
		delete [] verts;
		verts = NULL;
	}
	if(tris) {
		delete [] tris;	
		tris = NULL;
	}
	if(opcodeModel) {
		delete opcodeModel;
		opcodeModel = NULL;
	}
}

void AABBBoundingVolume::setBoundary(CollisionEnabledBoundary * bObj) 
{ 	 
	int num_vertex;
	bObject = bObj;
	num_vertex = bObject->num_vertex;
	faceOwner = new FaceOwner(num_vertex);
	iMesh = new MeshInterface();
	verts = new OPC_Point[bObject->num_vertex];
	tris = new IndexedTriangle[bObject->num_face];
	opcodeModel = new Model();
}

void AABBBoundingVolume::build(void)
{
	int				v0,v1,v2;

	for(unsigned int i=0;i<bObject->num_vertex;i++) {
		verts[i].Set(bObject->vertex[i].pos[0],bObject->vertex[i].pos[1],bObject->vertex[i].pos[2]);		
	}

	for(unsigned int i=0;i<bObject->num_face;i++) {
		v0 = bObject->face[i].vertex[0]->refid;
		v1 = bObject->face[i].vertex[1]->refid;
		v2 = bObject->face[i].vertex[2]->refid;
		
		tris[i].mVRef[0] = v0;
		tris[i].mVRef[1] = v1;
		tris[i].mVRef[2] = v2;

		faceOwner->add(v0,i);
		faceOwner->add(v1,i);
		faceOwner->add(v2,i);		
	} 

	iMesh->SetNbTriangles(bObject->num_face);
	iMesh->SetNbVertices(bObject->num_vertex);
	iMesh->SetPointers(tris, verts);

	OPCODECREATE OPCC;
	OPCC.mIMesh = iMesh;
	OPCC.mSettings.mRules = SPLIT_BEST_AXIS;
	OPCC.mNoLeaf = true;
	OPCC.mQuantized = false;
	OPCC.mKeepOriginal = false; 
    OPCC.mCanRemap = false; 	
	
	opcodeModel->Build(OPCC);
}

void AABBBoundingVolume::refit(void)
{
	opcodeModel->Refit(); 
}
/**
 * overlappingTest()	 
 * overlappingTest with another bounding Volume
 * @param BoundingVolume bv.
 * @param BoundingVolumeInf bvInfo.
 * @param bool firstContact.
 * @return int contact result.
 */
int AABBBoundingVolume::overlappingTest(BoundingVolume * bv, BoundingVolumeInfo * bvInfo, bool firstContact)
{	
	int result = 0;			
	// setup opcode aabb collider	
	TC.SetFirstContact(firstContact);
	TC.SetFullBoxBoxTest(false);
	TC.SetFullPrimBoxTest(false);
	TC.SetTemporalCoherence(false);

	// setup opcode collision cache
	static BVTCache ColCache; 		
	ColCache.Model0 = opcodeModel;
	ColCache.Model1 = ((AABBBoundingVolume*)bv)->getModel();
	
	// overlapping test using Opcode
	TC.Collide(ColCache);

	// some collisions occur
	result = TC.GetContactStatus();	
	if (result) {
		((AABBBoundingVolumeInfo*)bvInfo)->setNbPairs(TC.GetNbPairs());
		//printf("AABB overlapping test has %d contacts\n",TC.GetNbPairs());		
		((AABBBoundingVolumeInfo*)bvInfo)->setCollidingPairs(const_cast<Pair*>(TC.GetPairs()));		
	}
	
	return result;
}

/**
 * getBoundary()
 * Get the boundary of simulation object.
 * @return *Boundary object boundary pointer.
 */
CollisionEnabledBoundary * AABBBoundingVolume::getBoundary(void)
{
	return bObject;
}

/**
 * getFaceOwner()
 * Get the set of FaceOwner.
 * @return FaceOwner face owner pointer.
 */
FaceOwner * AABBBoundingVolume::getFaceOwner(void)
{
	return faceOwner;
}

/**
 * getTriangle()
 * Get the geometry of triangle (vertex positions and indices) of faceindex.
 * @param faceindex an integer argument.
 * @return v0[3] vertex 0 position.
 * @return v1[3] vertex 1 position.
 * @return v2[3] vertex 2 position.
 * @return vIndex[3] vertex index.
 */
void AABBBoundingVolume::getTriangle(int faceindex, float v0[3], float v1[3], float v2[3], int vIndex[3])
{
	TriSurface *geom = (TriSurface *) bObject;
	vector_copy(v0, geom->face[faceindex].vertex[0]->pos);
	vector_copy(v1, geom->face[faceindex].vertex[1]->pos);
	vector_copy(v2, geom->face[faceindex].vertex[2]->pos);

	vIndex[0] = geom->face[faceindex].vertex[0]->refid;
	vIndex[1] = geom->face[faceindex].vertex[1]->refid;
	vIndex[2] = geom->face[faceindex].vertex[2]->refid;
}

/**
 * FaceOwner()
 * Constructor for FaceOwner class. Allocate the vertexFaceIndexArray.
 * @param n_vertex number of vertex.
 */
FaceOwner::FaceOwner(int n_vertex)
{
	num_vertex = n_vertex;
	vertexFaceIndexArray = new VertexFaceIndex[num_vertex];
	if(vertexFaceIndexArray == NULL)
	{
		printf("can not allocate memory for vertexFaceIndexArray\n");
		exit(1);
	}
}

/**
 * ~FaceOwner()
 * Destructor for FaceOwner class. 
 */
FaceOwner::~FaceOwner()
{
	if(vertexFaceIndexArray)
		delete [] vertexFaceIndexArray;
}

/**
 * add()
 * Add face_index into the faceIndexList in the vertex_index of vertexFaceIndexArray.
 * @param vertex_index vertex index.
 * @param face_index face index.
 */
void FaceOwner::add(int vertex_index, int face_index)
{
	vertexFaceIndexArray[vertex_index].faceIndexSet.insert(face_index);
}

/**
 * getFaceOwner()
 * Get the set of FaceOwner of vertex_index.
 * @param vertex_index vertex index.
 * @return set<int> face index set.
 */
set<int> FaceOwner::getFaceOwner(int vertex_index)
{
	return vertexFaceIndexArray[vertex_index].faceIndexSet;
}

/**
 * print()
 * Print all FaceOwner of every vertex.
 */
void FaceOwner::print(void)
{
	for(int i=0;i<num_vertex;i++)
	{
		set<int> faceIndex;
		set<int>::iterator j;
		faceIndex = vertexFaceIndexArray[i].faceIndexSet;
		printf("vertex %d : ",i);
		for (j = faceIndex.begin(); j != faceIndex.end(); ++j)
		{
			printf(" %d ",*j);
		}
		printf("\n");
	}
}

/** 
 * isInCollisionInfos()
 * Check if this collisionInfo is in the collisionInfoList or not. 
 * @param bObj1 object boundary 1.
 * @param bObj2 object boundary 2. 
 * @return int -1 is not in the list, otherwise the index of collisionInfo.
 */
int PDepthCollision::isInCollisionInfos(CollisionEnabledBoundary *bObj1, CollisionEnabledBoundary *bObj2)
{
	vector<CollisionInfo*>::iterator i;
	PDepthCollisionInfo *cInfo;
	int result = -1;
	int count = 0;
	
	i = collisionInfos.begin();
	while (i != collisionInfos.end())
	{
		cInfo = (PDepthCollisionInfo*)(*i);
		if ( cInfo->isInPDepthCollisionInfo(bObj1,bObj2) ) {	
			result = count;
			break;			
		} else {			
			i++;
		}
		count++;
	}
	return result;
}

/**
 * initialize()
 *  buile collision model of AABB from OPCODE
 */
void PDepthCollision::initialize(void)
{
	enabled = true;
	vector<BoundingVolume*>::iterator i;	
	i = bvList->begin();
	while (i != bvList->end())
	{
		//CollisionModel* collisionModel = new CollisionModel((CollisionEnabledBoundary*)(*i));
		//collisionModel->build();
		//collisionModels.add(collisionModel);
		i++;
	}
}

/**
 * detection()
 * The detection function for overlaping simulation objects does the following:
 *	- rebuild the bounding volume
 *	- the bounding volume overlaping detectiong
 *	- the triangle-triangle intersection test for overlapped bounding volumes
 *  - add the result to PDepthCollision info
 */
void PDepthCollision::detection(void)
{
	int result = 0;	
	int num_Object;
	int id1, id2;

	// clear collisionInfoList everytime
	collisionInfos.clear();
	// set num of collision model
	num_Object = bvList->size();//collisionModels.size();

	//printf("num of object = %d\n",num_Object);
	for(int i=0;i<num_Object;i++) {
		// rebuild the BV: need to change to refit
		bvList->get(i)->build();		
	}

	// for all collision object in collision object list
	for(int i=0;i<num_Object;i++) {
		for(int j=i+1;j<num_Object;j++) {	

			id1 = bvList->get(i)->getBoundary()->Object->GetID();
			id2 = bvList->get(j)->getBoundary()->Object->GetID();
			if (collisionRule->isCollisionTest(id1, id2)==0) continue;

			AABBBoundingVolumeInfo * bvInfo = new AABBBoundingVolumeInfo();
			AABBBoundingVolume * bv1 = (AABBBoundingVolume *)bvList->get(i);
			AABBBoundingVolume * bv2 = (AABBBoundingVolume *)bvList->get(j);
			bool overlap = bv1->overlappingTest(bv2, bvInfo, false);
						
			// some collisions occur
			if (overlap)
			{					
				PDepthCollisionInfo *cInfo;
				// check the exisiting of collisionInfo
				int result = isInCollisionInfos(bv1->getBoundary(), bv2->getBoundary());
				if (result == -1)
				{
					// creat a new collisionInfo;
					cInfo = new PDepthCollisionInfo(bv1->getBoundary(), bv2->getBoundary(),  bv1->getFaceOwner(),  bv2->getFaceOwner());
					if (cInfo==NULL)
					{
						printf("failed to allocated memory for cInfo\n");
						exit(0);
					}

					// clear processFaceLists and newProcessFaceLists
					cInfo->reset();
					// allocate the faceCollisionInfo
					cInfo->allocFaceCollisionInfo();					
				}
				else
				{
					cInfo = (PDepthCollisionInfo*)(collisionInfos.get(result));
				}				
				
				udword NbPairs = bvInfo->getNbPairs();
				//printf("%d contact %d with %d contacts\n",i,j,NbPairs);							

				// for each pair do the collision detail
				for(int k=0;k<NbPairs;k++){
					float v0[3], v1[3], v2[3];	// vertices' object i
					float u0[3], u1[3], u2[3];	// vertices' object j
					int	vIndex[3], uIndex[3];
					float nv[3], nu[3];
					float pt1[3], pt2[3];		// intersection points 1 and 2
					int coPlanar;
					int codeT1, codeT2;
					int codeTT1, codeTT2;
					float t, u, v;
					int collided = 0;
					
					udword faceindex0 = bvInfo->getFaceIndex0(k);
					udword faceindex1 = bvInfo->getFaceIndex1(k);
					//printf(" %d: %d <-> %d\n",k,faceindex0,faceindex1);	
					
					// set the faceState
					cInfo->setFaceState(0,faceindex0,1); // 1 = “intersect”
					cInfo->setFaceState(1,faceindex1,1); // 1 = “intersect”

					// add otherObjFaceIndex
					cInfo->setOtherObjFace(0,faceindex0,faceindex1);
					cInfo->setOtherObjFace(1,faceindex1,faceindex0);

					// add face index to processFaceList
					cInfo->processFaceList_1.insert(faceindex0);
					cInfo->processFaceList_2.insert(faceindex1);

					// retrive the vertex					
					bv1->getTriangle(faceindex0,v0,v1,v2,vIndex);
					bv2->getTriangle(faceindex1,u0,u1,u2,uIndex);
					
					// set the vertex index in face collision info
					cInfo->setVertexIndexOfFaceCollisionInfo(0,faceindex0,vIndex);
					cInfo->setVertexIndexOfFaceCollisionInfo(1,faceindex1,uIndex);

					//collided = tri_tri_intersect(v0,v1,v2,u0,u1,u2);
					collided = tri_tri_intersect_with_isectline_detail(v0,v1,v2,u0,u1,u2,&coPlanar,&codeT1,&codeT2,&codeTT1,&codeTT2,nv,nu,pt1,pt2);
					//printf("collided code = %d, codeT1 = %d, codeT2 = %d, codeTT1 = %d, codeTT2 = %d\n",collided,codeT1,codeT2,codeTT1,codeTT2);
			
					if (collided>0) { 	// overlap occur
						if(coPlanar==1) {	// if they coplanar, one point is returned
							u1[0] -= u0[0];
							u1[1] -= u0[1];
							u1[2] -= u0[2];
							intersect_triangle(u0, u1 , v0, v1, v2, &t, &u, &v);
							if ((t>=0) && (t<=1)) { 	
								Point pt;
								//convex_combination(&(pt.pos), v0, v1, v2, u, v);
								for (int i = 0; i < 3; i++) 
									pt.pos[i] = (1. - u - v) * v0[i] + u * v1[i] + v * v2[i];
								//printf("contact point:%f %f %f\n",pt.pos[0],pt.pos[1],pt.pos[2]);								
							}				
						}
						else {				// otherwist, two points are returned depending on collidied, codeT1 and codeT2

							Point pt_1,pt_2;
							Point pnv, pnu;
							Point vPt[3], uPt[3];

							for(int i=0;i<3;i++)
							{
								pt_1.pos[i] = pt1[i];
								pt_2.pos[i] = pt2[i];
								pnv.pos[i] = nv[i];
								pnu.pos[i] = nu[i];
								vPt[0].pos[i] = v0[i];
								vPt[1].pos[i] = v1[i];
								vPt[2].pos[i] = v2[i];
								uPt[0].pos[i] = u0[i];
								uPt[1].pos[i] = u1[i];
								uPt[2].pos[i] = u2[i];
							}
							
							//printf("contact point 1:%f %f %f\n",pt_1.pos[0],pt_1.pos[1],pt_1.pos[2]);
							//printf("contact point 2:%f %f %f\n",pt_2.pos[0],pt_2.pos[1],pt_2.pos[2]);	

							// add vertex collision info into collision info
							cInfo->addVertexCollisionInfo(vPt, uPt, collided, codeT1, codeT2, codeTT1, codeTT2, vIndex, uIndex, pnv, pnu, pt_1, pt_2);							
						}
				
					} // end if (collided==1)
				
				} // end for(int k=0;k<NbPairs;k++)
	
				// add cInfo into the collisionInfoList
				if (result == -1)
					collisionInfos.add(cInfo);
			} 
			else {
				//printf("%d does not overlap with %d\n",i,j);
			}

			delete bvInfo;
		}
	}	
}

/**
 * print()
 * Print the collision result.
 */
void PDepthCollision::print(void)
{
	int numOfCollision;

	numOfCollision = collisionInfos.size();
	printf("num of collision = %d\n",numOfCollision);
	printf("-----------------------------------------\n");
	for(int i=0;i<numOfCollision;i++)
	{		
		printf("%d with %d contacts\n",i,collisionInfos.get(i)->numOfContact);
		for(int j=0;j<collisionInfos.get(i)->numOfContact;j++)
		{
			PDepthCollisionInfo *cInfo = (PDepthCollisionInfo*)collisionInfos.get(i);
			//printf(" %d : at %.4lf %.4lf %.4lf\n",j,cInfo->contacts0[j].pos[0],cInfo->contacts0[j].pos[1],cInfo->contacts0[j].pos[2]);
		}
		printf("-----------------------------------------\n");
	}
	printf("=========================================\n");	
}

/** 
 * setVertexIndex()
 * Set the vertex index of FaceCollisionInfo.
 * @param vIndex[3] vertex indice.
 */
void FaceCollisionInfo::setVertexIndex(int vIndex[3])
{
	vector_copy(vertexIndex, vIndex);	
}

/**
 * addOtherObjFaceIndex()
 * Add the other object face index to FaceCollisionInfo.
 * @param faceIndex face index.
 */
void FaceCollisionInfo::addOtherObjFaceIndex(int faceIndex)
{
	otherObjFaceIndexList.insert(faceIndex);
}

/**
 * print()
 * Print the infomation of FaceCollisionInfo.
 */
void FaceCollisionInfo::print(void)
{
	printf("Face CInfo: state[%d], Vertex CInfo: index[%d,%d,%d] ",faceState,vertexIndex[0],vertexIndex[1],vertexIndex[2]);
	set<int>::iterator j;
	printf("Other FaceIndex[");
	for (j = otherObjFaceIndexList.begin(); j != otherObjFaceIndexList.end(); ++j)
	{
		printf(" %d ",*j);
	}
	printf("]\n");	
}

/**
 * setPoint()
 * Set the position of vertex in VertexCollisionInfo.
 * @param pt point.
 */
void VertexCollisionInfo::setPoint(Point pt)
{
	point = pt;
}

/**
 * updateState()
 * Update state of VertexCollisionInfo from the counter in step 3c.
 */
void VertexCollisionInfo::updateState(void)
{ 
	if(counter>0) 
		vertexState = 2;
	else 
		vertexState = 1; 
}

/**
 * addIntersectionPoint()
 * Add the intersection point, normal and penetration depth to VertexCollisionInfo.
 * @param iPoint intersection point.
 * @param n normal vector.
 * @param pDepth penetration depth (default=0).
 */
void VertexCollisionInfo::addIntersectionPoint(Point iPoint, Point n, Real pDepth)
{
	normalVectorList.push_back(n);
	intersectionPointList.push_back(iPoint);
	pDepth_iPointList.push_back(pDepth);
}

/**
 * calculatePDepth()
 * Calculate the penetration depth of VertexCollisionInfo. 
 * if num>1 the penetration depth will be distributed.
 * @param num number of artificial node in resolve artificial point step. (default num = 1)
 */
void VertexCollisionInfo::calculatePDepth(int num)
{
	Real *omaga;
	Vector<Real> *omaga_n;
	Real *omaga_n_xp;
	Real sum_omaga = 0.0;
	Vector<Real> sum_omaga_n(3,0.0);
	Real sum_omaga_n_xp = 0.0;
	Vector<Real> normalv;
	Vector<Real> pDepthDir;
	int num_iPoint;
	Vector<Real> result;
	
	num_iPoint = intersectionPointList.size();	
	omaga = new Real[num_iPoint];
	if(omaga==NULL) {
		printf("Failed to allocate memory for omaga\n");
		exit(0);
	}
	omaga_n = new Vector<Real>[num_iPoint];
	if(omaga_n==NULL) {
		printf("Failed to allocate memory for omaga_n\n");
		exit(0);
	}
	omaga_n_xp = new Real[num_iPoint];
	if(omaga_n_xp==NULL) {
		printf("Failed to allocate memory for omaga_n_xp\n");
		exit(0);
	}

	//printf("vertex index:%d has intersection points:%d\n", vertexIndex, num_iPoint);
	for(int i=0;i<num_iPoint;i++)
	{
		result = intersectionPointList[i].pos - point.pos;
		if(result.length_sq()==0)
			omaga[i] = 0;
		else
			omaga[i] = 1/result.length_sq();
		
		if(normalVectorList[i].pos.length()!=0)
			normalv = normalVectorList[i].pos/normalVectorList[i].pos.length();
		else
			normalv = zero_vector3;

		omaga_n[i] = omaga[i]*normalv;
		omaga_n_xp[i] = omaga[i] * ( (normalv*result) + pDepth_iPointList[i] );
		//printf(" iPoint:%d has omaga = %f, omaga_n = %f,%f,%f, omaga_n_xp = %f\n",i,omaga[i],omaga_n[i][0],omaga_n[i][1],omaga_n[i][2],omaga_n_xp[i]);
		sum_omaga += omaga[i];
		sum_omaga_n += omaga_n[i];
		sum_omaga_n_xp += omaga_n_xp[i];
	}

	if(sum_omaga>0) {
		pDepthDir = sum_omaga_n/sum_omaga;	
		penetrationDirection = pDepthDir/pDepthDir.length();
		if (num>1)
			penetrationDepth = (sum_omaga_n_xp/sum_omaga)/num;		
		else
			penetrationDepth = sum_omaga_n_xp/sum_omaga;
	}
	else {
		pDepthDir = zero_vector3;
		penetrationDirection = zero_vector3;
		penetrationDepth = 0.0;
	}
	
	penetrationForce = penetrationDepth*penetrationDirection;	

	//printf("sum omaga = %f, sum omaga n = [%f,%f,%f], sum omaga n xp = %f\n",sum_omaga,sum_omaga_n[0],sum_omaga_n[1],sum_omaga_n[2],sum_omaga_n_xp);
	//printf("# %d, pDepth = %f, pDepthDir = [%f,%f,%f] num_iPoint=%d\n",vertexIndex,penetrationDepth,penetrationDirection[0],penetrationDirection[1],penetrationDirection[2],num_iPoint);
	delete [] omaga_n_xp;
	delete [] omaga_n;
	delete [] omaga;
}

/**
 * print()
 * Print the infomation of VertexCollisionInfo.
 */
void VertexCollisionInfo::print(void)
{	
	printf("vertex collision information =========================\n");
	printf("Vertex CInfo: index [%d], state[%d], counter[%d]\n",vertexIndex, vertexState, counter);
	printf("position[%f,%f,%f]\n",point.pos[0],point.pos[1],point.pos[2]);

	vector<Point>::iterator j;
	if(vertexState>1) {
		printf("Normal:\n");
		for (j = normalVectorList.begin(); j != normalVectorList.end(); ++j)
		{
			printf(" [%f,%f,%f]\n",(*j).pos[0],(*j).pos[1],(*j).pos[2]);
		}
		
		printf("Intersection:\n");
		for (j = intersectionPointList.begin(); j != intersectionPointList.end(); ++j)
		{
			printf(" [%f,%f,%f]\n",(*j).pos[0],(*j).pos[1],(*j).pos[2]);
		}
		
		printf("pDepth[%f], pDirection[%f,%f,%f]\n",penetrationDepth,penetrationDirection[0],penetrationDirection[1],penetrationDirection[2]);
		printf("pForce[%f,%f,%f]\n",penetrationForce[0],penetrationForce[1],penetrationForce[2]);
	}
}

/**
 * PDepthCollisionInfo()
 * Constructor for PDepthCollisionInfo class. set the boundary pointers and faceowner pointers.
 * @param bObject1 object 1 boundary.
 * @param bObject2 object 2 boundary.
 * @param fOwner1 face owner 1.
 * @param fOwner2 face owner 2.	
 */
PDepthCollisionInfo::PDepthCollisionInfo(CollisionEnabledBoundary *bObject1, CollisionEnabledBoundary *bObject2, FaceOwner *fOwner1, FaceOwner *fOwner2)
{
	bObj1 = bObject1;
	bObj2 = bObject2;
	faceOwner1 = fOwner1;
	faceOwner2 = fOwner2;
	setConstant(0.2,0.2,0.2,0.2);
	faceCollisionInfoArray_1 = NULL;
	faceCollisionInfoArray_2 = NULL;
}

/**
 * ~PDepthCollisionInfo()
 * Destructor for PDepthCollisionInfo class.
 */
PDepthCollisionInfo::~PDepthCollisionInfo()
{
	if(faceCollisionInfoArray_1!=NULL)
		delete faceCollisionInfoArray_1;
	if(faceCollisionInfoArray_2!=NULL)
		delete faceCollisionInfoArray_2;

}

/**
 * isInPDepthCollisionInfo()
 * Check the pair of boundary object pointer is *bObject1 and *bObject2 or not.
 * @param bObject1 object boundary 1.
 * @param bObject2 object boundary 2.
 * @return boolean.
 */
bool PDepthCollisionInfo::isInPDepthCollisionInfo(CollisionEnabledBoundary *bObject1, CollisionEnabledBoundary *bObject2)
{
	if((bObject1==bObj1)&&(bObject2==bObj2))
		return true;
	else
		return false;
}

/**
 * isInVertexCollisionInfo()
 * Check the vertex index is in the VertexCollisionInfoList or not.
 * @param index vertex index.
 * @param vCollisionInfoList list of vertex collision info.
 * @return int -1 is not found, otherwise return the index of VertexCollisionInfo.
 */
int PDepthCollisionInfo::isInVertexCollisionInfo(int index, vector<VertexCollisionInfo*> vCollisionInfoList)
{	
	vector<VertexCollisionInfo*> vCollisionInfo;	
	vector<VertexCollisionInfo*>::iterator i;	
	int count = 0;
	i = vCollisionInfoList.begin();
	while(i != vCollisionInfoList.end())
	{
		if ((*i)->isVertexIndex(index))
			return count;		
		++i;
		count++;
	}
	return -1;
}

/** 
 * reset()
 * Clear PDepthCollisionInfo.
 */
void PDepthCollisionInfo::reset(void)
{
	processFaceList_1.clear();
	processFaceList_2.clear();
	nextProcessFaceList_1.clear();
	nextProcessFaceList_2.clear();
	vertexCollisionInfoList_1.clear();
	vertexCollisionInfoList_2.clear();
}

/**
 * allocFaceCollisionInfo()
 * Allocate memory for FaceCollisionInfo and set vertex index of that face.
 */
void PDepthCollisionInfo::allocFaceCollisionInfo(void)
{
	TriSurface *geom;	
	int	num_face;
	int index[3];

	if(faceCollisionInfoArray_1!=NULL) delete faceCollisionInfoArray_1;
	faceCollisionInfoArray_1 = new FaceCollisionInfo[bObj1->num_face];
	if(faceCollisionInfoArray_1==NULL)
	{
		printf("failed to allocate the memory of faceCollisionInfoArray_1\n");
		exit(0);
	}
	geom = (TriSurface *) bObj1;
	num_face = geom->num_face;
	for(int i=0;i<num_face;i++){	
		index[0] = geom->face[i].vertex[0]->refid;
		index[1] = geom->face[i].vertex[1]->refid;
		index[2] = geom->face[i].vertex[2]->refid;
		faceCollisionInfoArray_1[i].setVertexIndex(index);
	}

	if(faceCollisionInfoArray_2!=NULL) delete faceCollisionInfoArray_2;
	faceCollisionInfoArray_2 = new FaceCollisionInfo[bObj2->num_face];
	if(faceCollisionInfoArray_2==NULL)
	{
		printf("failed to allocate the memory of faceCollisionInfoArray_2\n");
		exit(0);
	}
	geom = (TriSurface *) bObj2;
	num_face = geom->num_face;
	for(int i=0;i<num_face;i++)		
	{
		index[0] = geom->face[i].vertex[0]->refid;
		index[1] = geom->face[i].vertex[1]->refid;
		index[2] = geom->face[i].vertex[2]->refid;	
		faceCollisionInfoArray_2[i].setVertexIndex(index);
	}
}

/** 
 * setFaceState()
 * Set the faceState of FaceCollisionInfoArray.
 * @param objIndex object index [0: obj1, 1: obj2].
 * @param faceIndex face index.
 * @param faceState face state. 
 */
void PDepthCollisionInfo::setFaceState(int objIndex, int faceIndex, int faceState)
{
	if(objIndex==0)
	{	
		faceCollisionInfoArray_1[faceIndex].setState(faceState);
	}
	else
	{	
		faceCollisionInfoArray_2[faceIndex].setState(faceState);
	}		
}

/**
 * setOtherObjFace()
 * Set the other face index of FaceCollisionInfoArray.
 * @param objIndex object index [0: obj1, 1: obj2].
 * @param faceIndex face index.
 * @param otherObjFaceIndex other object face index. 
 */
void PDepthCollisionInfo::setOtherObjFace(int objIndex, int faceIndex, int otherObjFaceIndex)
{
	if(objIndex==0)	
		faceCollisionInfoArray_1[faceIndex].addOtherObjFaceIndex(otherObjFaceIndex);	
	else
		faceCollisionInfoArray_2[faceIndex].addOtherObjFaceIndex(otherObjFaceIndex);	
}

/**
 * setVertexIndexOfFaceCollisionInfo()
 * Set the vertex index of FaceCollisionInfoArray.
 * @param objIndex object index [0: obj1, 1: obj2].
 * @param faceIndex face index .
 * @param vIndex[3] vertex indices. 	
 */
void PDepthCollisionInfo::setVertexIndexOfFaceCollisionInfo(int objIndex, int faceIndex, int vIndex[3])
{
	if(objIndex==0)	
		faceCollisionInfoArray_1[faceIndex].setVertexIndex(vIndex);	
	else
		faceCollisionInfoArray_2[faceIndex].setVertexIndex(vIndex);
}

/**
 * addVertexCollisionInfo()
 * Add vertex collision information into the collision information class. 
 * add or not add depend on collideCode, codeT1, codeT2, codeTT1, codeTT2, vIndex, uIndex, nv, nu.
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
void PDepthCollisionInfo::addVertexCollisionInfo(Point vPt[3],Point uPt[3], int collidedCode, int codeT1, int codeT2, 
												 int codeTT1, int codeTT2, int vIndex[3], int uIndex[3], Point nv, Point nu, 
												 Point pt1, Point pt2)
{
	VertexCollisionInfo *vInfo;
	int result;
	int bitmask[3] = {0x01,0x02,0x04};

	// check the existing of vertex in vertexCollisionInfoList_1	
	for(int i=0;i<3;i++)
	{		
		//printf("T1 vertex %d index %d : ",i,vIndex[i]);
		result = isInVertexCollisionInfo(vIndex[i],vertexCollisionInfoList_1);
		if( result<0 ) // not exist, create a new one
			vInfo = new VertexCollisionInfo(vIndex[i],vPt[i]);
		else			// exist, retrive the pointer
			vInfo = vertexCollisionInfoList_1[result];
	
		if((codeTT1!=4)&&((i!=codeTT1-1)||(codeTT1==0))){
		if((codeT1&bitmask[i])==bitmask[i])		// inside, increase the counter, except collidedCode=5
		{
			//printf(" inside \n");
			switch(collidedCode)
			{
			case 1:	//  1: overlap with isectpt1[T1] & isectpt2[T2]
			case 6:	//	6: overlap with isectpt1[T1,T2] & isectpt2[T2]
				vInfo->incCounter();
				vInfo->addIntersectionPoint(pt1,nu);							
				break;
			case 2: //  2: overlap with isectpt1[T2] & isectpt2[T1]
				vInfo->incCounter();
				vInfo->addIntersectionPoint(pt2,nu);							
				break;
			case 3:	//  3: overlap with isectpt1[T1] & isectpt2[T1]					
			case 4:	//	4: overlap with isectpt1[T1] & isectpt2[T1,T2]
			case 7:	//	7: overlap with isectpt1[T1,T2] & isectpt2[T1,T2]
				vInfo->incCounter();				
				vInfo->addIntersectionPoint(pt1,nu);
				vInfo->addIntersectionPoint(pt2,nu);				
				break;
			case 5: //	5: overlap with isectpt1[T2] & isectpt2[T2]
				break;	
			}			
		}
		else						// outside
		{
			//printf(" outside \n");
			if(collidedCode!=5)
				vInfo->decCounter();
			if((collidedCode==3)||(collidedCode==4)||(collidedCode==7))
				vInfo->decCounter();
		}
		} //else printf("\n");
 
		if( result<0 ) 
			vertexCollisionInfoList_1.push_back(vInfo);
		
	} // end for loop


	// check the existing of vertex in vertexCollisionInfoList_2
	for(int i=0;i<3;i++)
	{		
		//printf("T2 vertex %d index %d : ",i,uIndex[i]);
		result = isInVertexCollisionInfo(uIndex[i],vertexCollisionInfoList_2);
		if( result<0 ) // not exist, create a new one
			vInfo = new VertexCollisionInfo(uIndex[i],uPt[i]);
		else			// exist, retrive the pointer
			vInfo = vertexCollisionInfoList_2[result];
		if((codeTT2!=4)&&((i!=codeTT2-1)||(codeTT2==0))) {
		if((codeT2&bitmask[i])==bitmask[i])		// inside, increase the counter, except collidedCode=3
		{			
			//printf(" inside \n");			
			switch(collidedCode)
			{
			case 1: //  1: overlap with isectpt1[T1] & isectpt2[T2]
			case 4: //	4: overlap with isectpt1[T1] & isectpt2[T1,T2]
				vInfo->incCounter();
				vInfo->addIntersectionPoint(pt2,nv);				
				break;
			case 2: //  2: overlap with isectpt1[T2] & isectpt2[T1]
				vInfo->incCounter();
				vInfo->addIntersectionPoint(pt1,nv);				
				break;
			case 3: //  3: overlap with isectpt1[T1] & isectpt2[T1]				
				break;
			case 5: //	5: overlap with isectpt1[T2] & isectpt2[T2]				
			case 6: //	6: overlap with isectpt1[T1,T2] & isectpt2[T2]
			case 7: //	7: overlap with isectpt1[T1,T2] & isectpt2[T1,T2]
				vInfo->incCounter();				
				vInfo->addIntersectionPoint(pt1,nv);
				vInfo->addIntersectionPoint(pt2,nv);				
				break;	
			}					
		}
		else						// outside
		{
			//printf(" outside \n");
			if(collidedCode!=3)
				vInfo->decCounter();
			if((collidedCode==5)||(collidedCode==6)||(collidedCode==7))
				vInfo->decCounter();
		}
		} //else printf("\n");

		if( result<0 ) 
			vertexCollisionInfoList_2.push_back(vInfo);
	} // end for loop	
}

/**
 * calculate()
 * Calculate the penetration depth.
 */
void PDepthCollisionInfo::calculate(void)
{
	int num_vcInfo;
/*
	TriSurface *geom;	
	int num_face;

	geom = (TriSurface *) bObj1;
	num_face = geom->num_face;
	for(int i=0;i<num_face;i++)
		faceCollisionInfoArray_1[i].print();

	geom = (TriSurface *) bObj2;
	num_face = geom->num_face;
	for(int i=0;i<num_face;i++)
		faceCollisionInfoArray_2[i].print();
*/
	//printf("v info 1\n");
	num_vcInfo = vertexCollisionInfoList_1.size();
	for(int i=0;i<num_vcInfo;i++){		
		vertexCollisionInfoList_1[i]->updateState();
		if(vertexCollisionInfoList_1[i]->getState()>1) {
			vertexCollisionInfoList_1[i]->calculatePDepth();
			//vertexCollisionInfoList_1[i]->print();		
		}
	}

	//printf("v info 2\n");
	num_vcInfo = vertexCollisionInfoList_2.size();
	for(int i=0;i<num_vcInfo;i++){		
		vertexCollisionInfoList_2[i]->updateState();
		if(vertexCollisionInfoList_2[i]->getState()>1) {
			vertexCollisionInfoList_2[i]->calculatePDepth();
			//vertexCollisionInfoList_2[i]->print();		
		}
	}
}

/** 
 * propagate()
 * Do the propagation step.
 */
void PDepthCollisionInfo::propagate(void)
{
	set<int>::iterator i;
	int faceIndex;
	int vIndex, vInfoIndex, vGeoIndex;
	Real vInfo_pDepth;
	Point vGeoPt;
	Point vGeoN;
	TriSurface *geom;
	VertexCollisionInfo *vInfo;	
	vector<VertexCollisionInfo*>::iterator i_vInfo;	
	
	geom = (TriSurface *) bObj1;
	do {
		nextProcessFaceList_1.clear();
		// 4.a
		for(i=processFaceList_1.begin();i!=processFaceList_1.end();++i)
		{
			faceIndex = *i;
			for(int j=0;j<3;j++)
			{
				vIndex = faceCollisionInfoArray_1[(*i)].getVertexIndex(j);
				//vIndex = geom->face[(*i)].vertex[j]->refid;
				vInfoIndex = isInVertexCollisionInfo(vIndex,vertexCollisionInfoList_1);
				if(vInfoIndex==-1) continue;
				if(vertexCollisionInfoList_1[vInfoIndex]->getState() == 2)	// if it is border state
				{
					vertexCollisionInfoList_1[vInfoIndex]->setState(3);		// set its state to processing state
					//this->setVertexState(0,vIndex,3);
				}
			}			
		} // end 4.a

		// 4.b
		for(i=processFaceList_1.begin();i!=processFaceList_1.end();++i)
		{		
			faceIndex = *i;
			for(int j=0;j<3;j++)
			{
				vIndex = faceCollisionInfoArray_1[(*i)].getVertexIndex(j);
				//vIndex = geom->face[(*i)].vertex[j]->refid;
				vInfoIndex = isInVertexCollisionInfo(vIndex,vertexCollisionInfoList_1); // every vertices are in the list
				if(vInfoIndex==-1) continue;
				if(vertexCollisionInfoList_1[vInfoIndex]->getState() == 3)	// if it is processing state
				{
					vGeoIndex = vertexCollisionInfoList_1[vInfoIndex]->getIndex();
					vInfo_pDepth = vertexCollisionInfoList_1[vInfoIndex]->getPDepth();
					vGeoPt.pos = geom->vertex[vGeoIndex].pos;
					vGeoN.pos = vertexCollisionInfoList_1[vInfoIndex]->getPDepthDir();
					set<int> faceOwnerIndex = faceOwner1->getFaceOwner(vGeoIndex);
					set<int>::iterator k;				
					//printf("face index: ");
					for (k = faceOwnerIndex.begin(); k != faceOwnerIndex.end(); ++k)
					{						
						if((faceCollisionInfoArray_1[(*k)].getState()==0))// if it is unknown state							
						{
							int vvIndex;
							int vvInfoIndex;
							faceCollisionInfoArray_1[(*k)].setState(2); // set face state to inside
							nextProcessFaceList_1.insert(*k);			// add the face index to nextProcessFaceList
							for(int l=0;l<3;l++)	// do for all vertices in that face
							{
								vvIndex = faceCollisionInfoArray_1[(*k)].getVertexIndex(l);
								//vvIndex = geom->face[(*k)].vertex[l]->refid;
								vvInfoIndex = isInVertexCollisionInfo(vvIndex,vertexCollisionInfoList_1);
								
								if (vvInfoIndex < 0)	// vertex is not in the vertex collision info list, create a new one
								{					
									vInfo = new VertexCollisionInfo(vvIndex,geom->vertex[vvIndex]);
								}
								else					// vertex is in the vertex collision info list, retrive a vInfo
								{
									vInfo = vertexCollisionInfoList_1[vvInfoIndex];								
								}
								
								vInfo->setState(2); // set to border
								//this->setVertexState(0,vvIndex,2);
								vInfo->addIntersectionPoint(vGeoPt,vGeoN,vInfo_pDepth);						

								if (vvInfoIndex < 0)
									vertexCollisionInfoList_1.push_back(vInfo);
							}
						}
						//printf(" %d ",*k);						
					}
				}
			} // end for		
		} // end 4.b
		
		// 4.c	
		i_vInfo = vertexCollisionInfoList_1.begin();
		while(i_vInfo != vertexCollisionInfoList_1.end())
		{
			if ((*i_vInfo)->getState()==2)
			{
				(*i_vInfo)->calculatePDepth();
				//(*i_vInfo)->print();
			}
			++i_vInfo;
		}
/*
		set<int>::iterator fi;
		printf("nextProcessFaceList_1[ ");
		for (fi = nextProcessFaceList_1.begin(); fi != nextProcessFaceList_1.end(); ++fi)
		{
			printf(" %d ",*fi);
		}
		printf(" ]\n");	
*/
		processFaceList_1 = nextProcessFaceList_1;
	} while(nextProcessFaceList_1.size()>0);

	// for bObject 2 ////////////////////////////////////////////////////////////////////////
	geom = (TriSurface *) bObj2;
	do {
		nextProcessFaceList_2.clear();
		// 4.a
		for(i=processFaceList_2.begin();i!=processFaceList_2.end();++i)
		{
			faceIndex = *i;
			for(int j=0;j<3;j++)
			{
				vIndex = faceCollisionInfoArray_2[(*i)].getVertexIndex(j);
				//vIndex = geom->face[(*i)].vertex[j]->refid;
				vInfoIndex = isInVertexCollisionInfo(vIndex,vertexCollisionInfoList_2);
				if(vInfoIndex==-1) continue;
				if(vertexCollisionInfoList_2[vInfoIndex]->getState() == 2)	// if it is border state
				{
					vertexCollisionInfoList_2[vInfoIndex]->setState(3);		// set its state to processing state
					//this->setVertexState(1,vIndex,3);
				}
			}			
		} // end 4.a

		// 4.b
		for(i=processFaceList_2.begin();i!=processFaceList_2.end();++i)
		{		
			faceIndex = *i;
			for(int j=0;j<3;j++)
			{
				vIndex = faceCollisionInfoArray_2[(*i)].getVertexIndex(j);
				//vIndex = geom->face[(*i)].vertex[j]->refid;
				vInfoIndex = isInVertexCollisionInfo(vIndex,vertexCollisionInfoList_2); // every vertices are in the list
				if(vInfoIndex==-1) continue;
				if(vertexCollisionInfoList_2[vInfoIndex]->getState() == 3)	// if it is processing state
				{
					vGeoIndex = vertexCollisionInfoList_2[vInfoIndex]->getIndex();
					vInfo_pDepth = vertexCollisionInfoList_2[vInfoIndex]->getPDepth();
					vGeoPt.pos = geom->vertex[vGeoIndex].pos;
					vGeoN.pos = vertexCollisionInfoList_2[vInfoIndex]->getPDepthDir();
					set<int> faceOwnerIndex = faceOwner2->getFaceOwner(vGeoIndex);
					set<int>::iterator k;				
					//printf("face index: ");
					for (k = faceOwnerIndex.begin(); k != faceOwnerIndex.end(); ++k)
					{
						if(faceCollisionInfoArray_2[(*k)].getState()==0)// if it is unknown state
						{
							int vvIndex;
							int vvInfoIndex;
							faceCollisionInfoArray_2[(*k)].setState(2); // set face state to inside
							nextProcessFaceList_2.insert(*k);			// add the face index to nextProcessFaceList
							for(int l=0;l<3;l++)	// do for all vertices in that face
							{
								vvIndex = faceCollisionInfoArray_2[(*k)].getVertexIndex(l);
								//vvIndex = geom->face[(*k)].vertex[l]->refid;
								vvInfoIndex = isInVertexCollisionInfo(vvIndex,vertexCollisionInfoList_2);
								
								if (vvInfoIndex < 0)	// vertex is not in the vertex collision info list, create a new one
								{					
									vInfo = new VertexCollisionInfo(vvIndex,geom->vertex[vvIndex]);
								}
								else					// vertex is in the vertex collision info list, retrive a vInfo
								{
									vInfo = vertexCollisionInfoList_2[vvInfoIndex];								
								}
								
								vInfo->setState(2); // set to border
								//this->setVertexState(1,vvIndex,2);
								vInfo->addIntersectionPoint(vGeoPt,vGeoN,vInfo_pDepth);						

								if (vvInfoIndex < 0)
									vertexCollisionInfoList_2.push_back(vInfo);
							}
						}
						//printf(" %d ",*k);
						
					}
				}
			} // end for		
		} // end 4.b
		
		// 4.c	
		i_vInfo = vertexCollisionInfoList_2.begin();
		while(i_vInfo != vertexCollisionInfoList_2.end())
		{
			if ((*i_vInfo)->getState()==2)
			{
				(*i_vInfo)->calculatePDepth();
				//(*i_vInfo)->print();
			}
			++i_vInfo;
		}
/*		
		set<int>::iterator fi;
		printf("nextProcessFaceList_2[ ");
		for (fi = nextProcessFaceList_2.begin(); fi != nextProcessFaceList_2.end(); ++fi)
		{
			printf(" %d ",*fi);
		}
		printf(" ]\n");	
*/		
		processFaceList_2 = nextProcessFaceList_2;
	} while(nextProcessFaceList_2.size()>0);
/*
// debug ////////////
	int num_face;

	geom = (TriSurface *) bObj1;
	num_face = geom->num_face;
	for(int i=0;i<num_face;i++)		
	{
		printf("face %d: state %d\n",i,faceCollisionInfoArray_1[i].getState());		
	}

	geom = (TriSurface *) bObj2;
	num_face = geom->num_face;
	for(int i=0;i<num_face;i++)		
	{
		printf("face %d: state %d\n",i,faceCollisionInfoArray_2[i].getState());		
	}
/////////////////////
*/

}

/**
 * resolveArtificialVertex()
 * Resolve the artificial border.
 */
void PDepthCollisionInfo::resolveArtificialVertex(void)
{
	set<int> artificialVertexIndexList_1;
	set<int> artificialVertexIndexList_2;
	set<int>::iterator ai;
	int vvIndex, vvInfoIndex;
	int num_face;
	int num_ai;
	int num_isect1, num_isect2;
	Point vGeoPt;
	Point vGeoN;
	VertexCollisionInfo *vInfo;	
	VertexCollisionInfo *vInfo_OtherObj;
	vector<VertexCollisionInfo*>::iterator i_vInfo;	
	TriSurface *geom1, *geom2;
	bool allvertices_are_noncolliding;	//for checking vertices in the face should have all noncolliding state

	geom1 = (TriSurface *) bObj1;
	geom2 = (TriSurface *) bObj2;

	//printf("\n====== artificial vertices ======\n");
	//5.a
	artificialVertexIndexList_1.clear();	
	//5.b
	num_face = geom1->num_face;
	//printf("--> obj 1\n");
	num_isect1 = 0;
	for(int i=0;i<num_face;i++) // do it all face in faceCollisionInfoArray
	{
		if(faceCollisionInfoArray_1[i].getState()==1)	// if face state == intersect state
		{
			// check all vertices must no border or processing vertices
			allvertices_are_noncolliding = true;
			for(int l=0;l<3;l++)	// do for all vertices in that face
			{
				vvIndex = faceCollisionInfoArray_1[i].getVertexIndex(l);		
				vvInfoIndex = isInVertexCollisionInfo(vvIndex,vertexCollisionInfoList_1);
				if(vvInfoIndex>=0) {
					if((vertexCollisionInfoList_1[vvInfoIndex]->getState()==2)||		
						(vertexCollisionInfoList_1[vvInfoIndex]->getState()==3))
					{
						allvertices_are_noncolliding = false;
						break;
					}
				}
			}
			if(!allvertices_are_noncolliding) continue;

			for(int l=0;l<3;l++)	// do for all vertices in that face
			{
				vvIndex = faceCollisionInfoArray_1[i].getVertexIndex(l);		
				//vvIndex = geom1->face[i].vertex[l]->refid;
				vvInfoIndex = isInVertexCollisionInfo(vvIndex,vertexCollisionInfoList_1);
				
				if (vvInfoIndex < 0)	// vertex is not in the vertex collision info list, create a new one
				{					
					vInfo = new VertexCollisionInfo(vvIndex,geom1->vertex[vvIndex]);
				}
				else					// vertex is in the vertex collision info list, retrive a vInfo
				{
					if((vertexCollisionInfoList_1[vvInfoIndex]->getState()==1)||
					   (vertexCollisionInfoList_1[vvInfoIndex]->getState()==4))
						vInfo = vertexCollisionInfoList_1[vvInfoIndex];								
					else
						break;
				}
	
				set<int> otherFaceIndexList = faceCollisionInfoArray_1[i].getOtherObjFaceIndexList();
				set<int>::iterator ofi;
				for(ofi=otherFaceIndexList.begin();ofi!=otherFaceIndexList.end();++ofi) // do for all face in otherFaceIndexList
				{
					//find the border point of that face (processiong point)
					//printf("ofi = %d\n",*ofi);
					for(int vi=0;vi<3;vi++)
					{
						int vIndex,vInfoIndex;
						vIndex = faceCollisionInfoArray_2[*ofi].getVertexIndex(vi);
						//vIndex = geom2->face[(*ofi)].vertex[vi]->refid;
						vInfoIndex = isInVertexCollisionInfo(vIndex,vertexCollisionInfoList_2);
						if (vInfoIndex >= 0)	// vertex is in the vertex collision info list, do the following
						{			
							//printf("vi=%d has state = %d\n",vi,vertexCollisionInfoList_2[vInfoIndex]->getState());
							if((vertexCollisionInfoList_2[vInfoIndex]->getState()==3)|| // check it is processing point
								(vertexCollisionInfoList_2[vInfoIndex]->getState()==2))
							{
								vInfo_OtherObj = vertexCollisionInfoList_2[vInfoIndex];
								vGeoPt = vInfo_OtherObj->getPoint();
								vGeoN.pos = vInfo_OtherObj->getPDepthDir();
								vInfo->addIntersectionPoint(vGeoPt,vGeoN);
								num_isect1++;
							}							
						}
					} // end for vi
				}
						
				vInfo->setState(4); // set to a_border state								
				//this->setVertexState(0,vvIndex,4);

				if (vvInfoIndex < 0) {
					vertexCollisionInfoList_1.push_back(vInfo);
					vvInfoIndex = isInVertexCollisionInfo(vvIndex,vertexCollisionInfoList_1);
				}

				artificialVertexIndexList_1.insert(vvInfoIndex);
			} // end for each vertex
		} // end if
	} // end for
	
	// 5.c	
	ai = artificialVertexIndexList_1.begin();
	num_ai = artificialVertexIndexList_1.size();
	while(ai != artificialVertexIndexList_1.end())
	{
		vInfo = vertexCollisionInfoList_1[*ai];
		if (vInfo->getState()==4)	// a_border
		{
			vInfo->calculatePDepth(num_ai);
			//vInfo->print();
		}
		++ai;
	}


	// for obj2 /////////////////////////////////////////////////////////////////////////////////

	//5.a
	artificialVertexIndexList_2.clear();	
	//5.b
	num_face = geom2->num_face;
	//printf("--> obj 2\n");
	num_isect2 = 0;
	for(int i=0;i<num_face;i++) // do it all face in faceCollisionInfoArray
	{
		if(faceCollisionInfoArray_2[i].getState()==1)	// if face state == intersect state
		{
			// check all vertices must no border or processing vertices
			allvertices_are_noncolliding = true;
			for(int l=0;l<3;l++)	// do for all vertices in that face
			{
				vvIndex = faceCollisionInfoArray_2[i].getVertexIndex(l);		
				vvInfoIndex = isInVertexCollisionInfo(vvIndex,vertexCollisionInfoList_2);
				if(vvInfoIndex>=0) {
					if((vertexCollisionInfoList_2[vvInfoIndex]->getState()==2)||		
						(vertexCollisionInfoList_2[vvInfoIndex]->getState()==3))
					{
						allvertices_are_noncolliding = false;
						break;
					}
				}
			}
			if(!allvertices_are_noncolliding) continue;

			for(int l=0;l<3;l++)	// do for all vertices in that face
			{
				vvIndex = faceCollisionInfoArray_2[i].getVertexIndex(l);	
				//vvIndex = geom2->face[i].vertex[l]->refid;
				vvInfoIndex = isInVertexCollisionInfo(vvIndex,vertexCollisionInfoList_2);
				
				if (vvInfoIndex < 0)	// vertex is not in the vertex collision info list, create a new one
				{					
					vInfo = new VertexCollisionInfo(vvIndex,geom2->vertex[vvIndex]);
				}
				else					// vertex is in the vertex collision info list, retrive a vInfo
				{
					if((vertexCollisionInfoList_2[vvInfoIndex]->getState()==1)||
					   (vertexCollisionInfoList_2[vvInfoIndex]->getState()==4))
						vInfo = vertexCollisionInfoList_2[vvInfoIndex];								
					else
						break;
				}
				
				set<int> otherFaceIndexList = faceCollisionInfoArray_2[i].getOtherObjFaceIndexList();
				set<int>::iterator ofi;
				for(ofi=otherFaceIndexList.begin();ofi!=otherFaceIndexList.end();++ofi) // do for all face in otherFaceIndexList
				{
					//find the border point of that face (processiong point)					
					for(int vi=0;vi<3;vi++)
					{
						int vIndex,vInfoIndex;
						vIndex = faceCollisionInfoArray_1[*ofi].getVertexIndex(vi);
						//vIndex = geom1->face[(*ofi)].vertex[vi]->refid;
						vInfoIndex = isInVertexCollisionInfo(vIndex,vertexCollisionInfoList_1);
						if (vInfoIndex >= 0)	// vertex is in the vertex collision info list, do the following
						{					
							if((vertexCollisionInfoList_1[vInfoIndex]->getState()==3)|| // check it is processing point
								(vertexCollisionInfoList_1[vInfoIndex]->getState()==2))
							{
								vInfo_OtherObj = vertexCollisionInfoList_1[vInfoIndex];
								vGeoPt = vInfo_OtherObj->getPoint();
								vGeoN.pos = vInfo_OtherObj->getPDepthDir();
								vInfo->addIntersectionPoint(vGeoPt,vGeoN);
								num_isect2++;
							}							
						}
					} // end for vi
				}
						
				vInfo->setState(4); // set to a_border state								
				//this->setVertexState(1,vvIndex,4);

				if (vvInfoIndex < 0) {
					vertexCollisionInfoList_2.push_back(vInfo);
					vvInfoIndex = isInVertexCollisionInfo(vvIndex,vertexCollisionInfoList_2);
				}

				artificialVertexIndexList_2.insert(vvInfoIndex);
			} // end for each vertex
		} // end if
	} // end for

	// 5.c	
	ai = artificialVertexIndexList_2.begin();
	num_ai = artificialVertexIndexList_2.size();
	while(ai != artificialVertexIndexList_2.end())
	{
		vInfo = vertexCollisionInfoList_2[*ai];
		if (vInfo->getState()==4)	// a_border
		{
			vInfo->calculatePDepth(num_ai);
			//vInfo->print();
		}
		++ai;
	}
}

/**
 * setConstant()
 * set constant.
 * @param CC1.
 * @param kk1.
 * @param CC2.
 * @param kk2.
 */
void PDepthCollisionInfo::setConstant(Real CC1, Real kk1, Real CC2, Real kk2)
{
	C1 = CC1;
	k1 = kk1;
	C2 = CC2;
	k2 = kk2;
}

/**
 * calTotalForce()
 * Calculate the totalForce of each overlapping simulation object.
 */
void PDepthCollisionInfo::calTotalForce(void)
{	
	Vector<Real> totalForce_1(3,0.0);
	Vector<Real> totalForce_2(3,0.0);
	Vector<Real> rForce, rForce_1, rForce_2;
	int	size_1, size_2;
	vector<VertexCollisionInfo*>::iterator i;	

	size_1 = 0;
	i = vertexCollisionInfoList_1.begin();
	while(i != vertexCollisionInfoList_1.end())
	{
		if((*i)->getState()>1) {
			totalForce_1 += (*i)->getPDepthForce();
			size_1++;
		}
		++i;
	}
	//printf("total force 1 [%f,%f,%f] = %f \n",totalForce_1[0],totalForce_1[1],totalForce_1[2],totalForce_1.length());

	size_2 = 0;
	i = vertexCollisionInfoList_2.begin();
	while(i != vertexCollisionInfoList_2.end())
	{
		if((*i)->getState()>1){
			totalForce_2 += (*i)->getPDepthForce();
			size_2++;
		}
		i++;
	}
	//printf("total force 2 [%f,%f,%f] = %f \n",totalForce_2[0],totalForce_2[1],totalForce_2[2],totalForce_2.length());

	//Real f1 = totalForce_1.length();
	//Real f2 = totalForce_2.length();
	//rForce = totalForce_1 - totalForce_2;
	//if(f1>f2)
	
	// balance force
	rForce = (totalForce_1 + totalForce_2)*0.5;	
	//printf("rForce [%f,%f,%f] = %f \n",rForce[0],rForce[1],rForce[2],rForce.length());


	
	rForce_1 = rForce;
	if(size_1>0)
		rForce_1 /= -size_1;

	i = vertexCollisionInfoList_1.begin();
	while(i != vertexCollisionInfoList_1.end())
	{
		if((*i)->getState()>1){
			(*i)->addRForce(rForce_1);			
		}
		i++;
	}

	rForce_2 = rForce;
	if(size_2>0)
		rForce_2 /= -size_2;
	i = vertexCollisionInfoList_2.begin();
	while(i != vertexCollisionInfoList_2.end())
	{
		if((*i)->getState()>1){
			(*i)->addRForce(rForce_2);			
		}
		i++;
	}
	
	//printf("rForce 1 [%f,%f,%f] = %f \n",rForce_1[0],rForce_1[1],rForce_1[2],rForce_1.length());
	//printf("rForce 2 [%f,%f,%f] = %f \n",rForce_2[0],rForce_2[1],rForce_2[2],rForce_2.length());
	/*
	totalForce_1 = zero_vector3;
	i = vertexCollisionInfoList_1.begin();
	while(i != vertexCollisionInfoList_1.end())
	{
		if((*i)->getState()>1)
			totalForce_1 += (*i)->getPDepthForce();
		i++;
	}
	printf("total force 1 [%f,%f,%f] = %f, size = %d\n",totalForce_1[0],totalForce_1[1],totalForce_1[2],totalForce_1.length(),size_1);

	totalForce_2 = zero_vector3;
	i = vertexCollisionInfoList_2.begin();
	while(i != vertexCollisionInfoList_2.end())
	{
		if((*i)->getState()>1)
			totalForce_2 += (*i)->getPDepthForce();
		i++;
	}
	printf("total force 2 [%f,%f,%f] = %f, size = %d \n",totalForce_2[0],totalForce_2[1],totalForce_2[2],totalForce_2.length(),size_2);
	*/
}

/** 
 * collision2Bound()
 * Apply the result back to the boundary of simulation object.
 */
void PDepthCollisionInfo::collision2Bound(void)
{

#ifdef _DEBUG_COLLISION
	CollisionTestBoundary *bound;	
	vector<VertexCollisionInfo*>::iterator i;
	int vertexIndex;	
	
	bound = (CollisionTestBoundary *) bObj1;
	i = vertexCollisionInfoList_1.begin();
	while(i != vertexCollisionInfoList_1.end())
	{
		if((*i)->getState()>1) {
			vertexIndex = (*i)->getIndex();
			bound->Set(vertexIndex,0,(*i)->getPDepthForce(),0.0,zero_vector3);
			//bound->SetVF(vertexIndex,(*i)->getPDepthForce());	
		}
		++i;
	}
	
	bound = (CollisionTestBoundary *) bObj2;	
	i = vertexCollisionInfoList_2.begin();
	while(i != vertexCollisionInfoList_2.end())
	{
		if((*i)->getState()>1) {
			vertexIndex = (*i)->getIndex();
			bound->Set(vertexIndex,0,(*i)->getPDepthForce(),0.0,zero_vector3);
			//bound->SetVF(vertexIndex,(*i)->getPDepthForce());	
		}
		++i;
	}
#else	
	CollisionEnabledBoundary *bound;
	vector<VertexCollisionInfo*>::iterator i;
	int vertexIndex;
	Vector<Real> oldpos;
	
	bound = (CollisionEnabledBoundary *) bObj1;		
	i = vertexCollisionInfoList_1.begin();
	while(i != vertexCollisionInfoList_1.end())
	{
		if (!bound->GetResponse()) continue;
		if((*i)->getState()>1) {
			vertexIndex = (*i)->getIndex();
			oldpos = bound->GetPosition(vertexIndex);
			bound->Set(vertexIndex,1,oldpos + (*i)->getPDepthForce(),0.0,zero_vector3);					
		}
		++i;
	}	
	
	bound = (CollisionEnabledBoundary *) bObj2;	
	i = vertexCollisionInfoList_2.begin();
	while(i != vertexCollisionInfoList_2.end())
	{
		if (!bound->GetResponse()) continue;
		if((*i)->getState()>1) {
			vertexIndex = (*i)->getIndex();
			oldpos = bound->GetPosition(vertexIndex);
			bound->Set(vertexIndex,1,oldpos + (*i)->getPDepthForce(),0.0,zero_vector3);													
		}
		++i;
	}
#endif
}

/**
 * updateFaceStateDisplay()
 * Update the color of faceCollisionInfo.
 * @param objIndex object index.
 * @param faceIndex face index.
 * @param faceState face state.
 */
void PDepthCollisionInfo::updateFaceStateDisplay(int objIndex, int faceIndex, int faceState)
{
	Vector<Real> color(4);	

	switch(faceState)
	{
	case 0: // unknow state	: gray	
		color[0] = 0.7529; color[1] = 0.7529; color[2] = 0.7529; color[3] = 0.6;
		break;
	case 1: // intersect : red
		color[0] = 1; color[1] = 0; color[2] = 0; color[3] = 0.6;
		break;
	case 2: // inside : lime_green
		color[0] = 0.1961; color[1] = 0.8000; color[2] = 0.1961; color[3] = 0.6;
		break;
	case 3: // outside : yellow
		color[0] = 0; color[1] = 1; color[2] = 1; color[3] = 0.6;
		break;
	}	

	TriSurface *geom;
	
	if(objIndex==0){	
		geom = (TriSurface *) bObj1;		
	}
	else
	{		
		geom = (TriSurface *) bObj2;		
	}		
	geom->face[faceIndex].vertex[0]->color = color;
	geom->face[faceIndex].vertex[1]->color = color;
	geom->face[faceIndex].vertex[2]->color = color;	
}

/** 
 * updateVertexStateDisplay()
 * Update the color of vertexCollisionInfo.
 * @param objIndex object index.
 * @param vertexIndex vertex index.
 * @param pForce penetration force.
 */
void PDepthCollisionInfo::updateVertexStateDisplay(int objIndex, int vertexIndex, int vertexState)
{
	Vector<Real> color(4);

	switch(vertexState)
	{
	case 0: // unknow state	: gray	
		color[0] = 0.7529; color[1] = 0.7529; color[2] = 0.7529; color[3] = 0.6;
		break;
	case 1: // non-colliding : white
		color[0] = 0.9882; color[1] = 0.9882; color[2] = 0.9882; color[3] = 0.6;
		break;
	case 2: // border : red
		color[0] = 1; color[1] = 0; color[2] = 0; color[3] = 0.6;		
		break;
	case 3: // processing : violet_red
		color[0] = 0.8000; color[1] = 0.1961; color[2] = 0.6000; color[3] = 0.6;		
		break;
	case 4: // a_border : coral
		color[0] = 1; color[1] = 0.498; color[2] = 0; color[3] = 0.6;
		break;
	}	

	TriSurface *geom;
	
	if(objIndex==0){		
		geom = (TriSurface *) bObj1;				
	}
	else
	{		
		geom = (TriSurface *) bObj2;		
	}
	
	geom->vertex[vertexIndex].color = color;		
}

/**
 * updateDisplay()
 * Update the color of CollisionInfo with mode.
 * @param mode [0:face, 1:vertex].
 */
void PDepthCollisionInfo::updateDisplay(int mode)
{
	if(mode==0) 
	{
		int num_face;
		int state;

		num_face = bObj1->num_face;
		for(int i=0;i<num_face;i++) // do it all face in faceCollisionInfoArray
		{
			state = faceCollisionInfoArray_1[i].getState();
			this->updateFaceStateDisplay(0,i,state);
		}

		num_face = bObj2->num_face;
		for(int i=0;i<num_face;i++) // do it all face in faceCollisionInfoArray
		{
			state = faceCollisionInfoArray_2[i].getState();
			this->updateFaceStateDisplay(1,i,state);
		}
	}
	else
	{
		vector<VertexCollisionInfo*>::iterator i;		
		int state;
		int index;
			
		for(i=vertexCollisionInfoList_1.begin();i!=vertexCollisionInfoList_1.end();++i)	
		{
			state = (*i)->getState();
			index = (*i)->getIndex();			
			this->updateVertexStateDisplay(0,index,state);					
		}	

		for(i=vertexCollisionInfoList_2.begin();i!=vertexCollisionInfoList_2.end();++i)
		{
			state = (*i)->getState();
			index = (*i)->getIndex();			
			this->updateVertexStateDisplay(1,index,state);					
		}	
	}
}



/** 
 * response()
 * Resolve the collision for each paire of overlapped objects by
 *	- calculate the peneration depth and direction
 *	- propagating to entire the simulation object
 *	- resolve the artificial border
 *  - calculate the total force
 */
void PDepthCollision::response(void)
{
	int num_cInfo;

	PDepthCollisionInfo *cInfo;
	num_cInfo = collisionInfos.size();

	//printf("number of collision = %d\n",num_cInfo);

#ifdef _DEBUG_COLLISION
	printf("number of collision = %d\n",num_cInfo);
	for(int i=0;i<num_cInfo;i++)
	{		
		cInfo = (PDepthCollisionInfo*)(collisionInfos.get(i));
		cInfo->calculate();					// step 3c
		cInfo->updateDisplay(0);
		cInfo->propagate();					// step 4		
		//cInfo->updateDisplay(0);
		cInfo->resolveArtificialVertex();	// step 5		
		cInfo->calTotalForce();				// step 6
		//cInfo->updateDisplay(1);	
		cInfo->collision2Bound();		
	}
#else	
	for(int i=0;i<num_cInfo;i++)
	{
		cInfo = (PDepthCollisionInfo*)(collisionInfos.get(i));
		cInfo->calculate();					// step 3c
		cInfo->propagate();					// step 4		
		cInfo->resolveArtificialVertex();	// step 5		
		cInfo->calTotalForce();				// step 6
		cInfo->collision2Bound();
	}
#endif
}