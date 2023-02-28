/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Collision Detection for Haptic Collision with Tri-Line method Implementation (collisionTLHaptic.h).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	COLLISIONTLHAPTIC.cpp	v0.0
////
////	Implementation of:
////		Collision detection for haptic collision
////
////////////////////////////////////////////////////////////////

#include "collision.h"
#include "collisiontest.h"
#include "probe.h"
#include "msd_haptics.h"

/**
 * TLHapticCollision::checkCollision()
 * The line-triangle intersection test. Haptic Interface Object is defined by a line in Hgeom which is used by face[0].vertex[0] and face[0].vertex[1].
 * @param Hgeom HapticInterfaceObject pointer.
 * @param geom SimObject pointer.
 * @param contacts contact points.
 * @param faceIndex face index.
 */
int TLHapticCollision::checkCollision(TriSurface *Hgeom, TriSurface *geom, Point *HTip, Point *contacts, int *faceIndex)
{
  /* Local variables */
	int collided = 0;        /* Boolean to test if the surfaces collide or not */
	int m1,                  /* Number of faces in geom1 */
        m2;	                 /* Number of faces in geom2 */
	float v0[3],v1[3],v2[3], /* Vertices of a triangle */
          u0[3],u1[3];       /* Endpoints of a line */
	float t,u,v;             /* Intermediate distance values */
	int numOfContact;        /* Number of contact points */
	
    /* Check arguments */
    assert(Hgeom != NULL);
    assert(geom != NULL);
    assert(contacts != NULL);
	assert(faceIndex != NULL);

    /* Initializations */
	m1 = Hgeom->num_face;
	m2 = geom->num_face;
	
	numOfContact = 0;	

	for(int m=0; m < m1; m++) {
          /* Find the vertices of the mth face of the first surface */
		  vector_copy(u0, Hgeom->face[m].vertex[0]->pos);
          vector_copy(u1, Hgeom->face[m].vertex[1]->pos);          
		  vector_subtract(u1, u0);

	  for(int n=0; n < m2; n++) {
            /* Find the nth line of the second surface */
            vector_copy(v0, geom->face[n].vertex[0]->pos);
            vector_copy(v1, geom->face[n].vertex[1]->pos);
			vector_copy(v2, geom->face[n].vertex[2]->pos);    
				
			/* Check if the two surfaces collide */
            collided = intersect_triangle(u0, u1 , v0, v1, v2, &t, &u, &v);
		
			if (check_collision(collided, t)) {	
                /* Oh, no! They are colliding! */
				numOfContact++;
				/* Find the point of contact */
                Point pt;
                //convex_combination(&(pt.pos), v0, v1, v2, u, v);
				for (int i = 0; i < 3; i++) 
					pt.pos[i] = (1. - u - v) * v0[i] + u * v1[i] + v * v2[i];
                /* Set the return result */
				(*HTip).pos = Hgeom->face[m].vertex[0]->pos;
				(*HTip).refid = Hgeom->face[m].vertex[0]->refid;
				*contacts = pt;
				*faceIndex = n;
			}
		}
	}
	return numOfContact;	
}

/*
 * detection()
 * Find intersect between objects 1 and 2, for all pair of objects.
 */
void TLHapticCollision::detection(void)
{	
	int numOfSIMObject;
	int numOfHIObject;
	int id1, id2;
	Point htip, contact;

	/* clear collisionInfoList everytime */
	collisionInfos.clear();	
	numOfHIObject = hbvList->size();
	numOfSIMObject = bvList->size();	
			
	for(int i=0; i<numOfHIObject; i++) 
		hbvList->get(i)->build();
	for(int i=0; i<numOfSIMObject; i++) 
		bvList->get(i)->build();	

	/* For all HapticInterfaceObject */
	for(int i=0; i<numOfHIObject; i++) {		
		id1 = hbvList->get(i)->getBoundary()->Object->GetID();
		/* For each HapticInterfaceObject, check intersect for all SimObject */
		for(int j=0; j<numOfSIMObject; j++) {						
			id2 = bvList->get(j)->getBoundary()->Object->GetID();
			
			if (id2>=id1) {
				if (collisionRule->isCollisionTest(id1, id2)==0) continue;
			}
			else {
				if (collisionRule->isCollisionTest(id2, id1)==0) continue;
			}

			CollisionEnabledBoundary *CBound = (CollisionEnabledBoundary*)bvList->get(j)->getBoundary();
			// If the cd_enable of boundary is set, then do the collision detection
			if (!CBound->GetDetection()) continue;
								
			AABBBoundingVolumeInfo * bvInfo = new AABBBoundingVolumeInfo();
			AABBBoundingVolume * bv1 = (AABBBoundingVolume *)hbvList->get(i);
			AABBBoundingVolume * bv2 = (AABBBoundingVolume *)bvList->get(j);
					
			int overlap = bv1->overlappingTest(bv2, bvInfo, true);
			
			if (overlap) {								
				float u0[3], u1[3], u2[3];	// vertices' object i
				float v0[3], v1[3], v2[3];	// vertices' object j
				int	vIndex[3], uIndex[3];
				float t, u, v;
				int collided = 0;
					
				udword faceindex0 = bvInfo->getFaceIndex0(0);
				udword faceindex1 = bvInfo->getFaceIndex1(0);
				//printf(" haptic contact at: %d <-> %d\n", faceindex0, faceindex1);	
					
				// retrive the vertex					
				bv1->getTriangle(faceindex0,u0,u1,u2,uIndex);
				vector_subtract(u1, u0);
				bv2->getTriangle(faceindex1,v0,v1,v2,vIndex);
					

				/* Check if the line intersects with triangle */
				collided = intersect_triangle(u0, u1, v0, v1, v2, &t, &u, &v);			
				//printf("collided = %d\n",collided);							

				if (check_collision(collided, t)) {	
					/* Oh, no! They are colliding! */
					/* Find the point of contact */					
					for (int k = 0; k < 3; k++) 
						contact.pos[k] = (1. - u - v) * v0[k] + u * v1[k] + v * v2[k];
					/* Set the return result */
					htip.pos = hbvList->get(i)->getBoundary()->face[faceindex0].vertex[0]->pos;
					htip.refid = hbvList->get(i)->getBoundary()->face[faceindex0].vertex[0]->refid;

					/* Find intersection, set info into collision info */
					HapticCollisionInfo *cInfo = new HapticCollisionInfo();
					cInfo->bObj1 = hbvList->get(i)->getBoundary();
					cInfo->bObj2 = bvList->get(j)->getBoundary();
					cInfo->HTip = htip;
					cInfo->contacts = contact;
					cInfo->faceIndex = faceindex1;
					cInfo->HapticID = i;
					/* Add the collision info into the list */
					collisionInfos.add(cInfo);
				}
			}	
			delete bvInfo;
		}
	}	
}

/**
 *	HapticCollision::response()
 *	collision response between HapticInterfaceObject and SimObject
 */
void TLHapticCollision::response(void)
{	
	int numOfCollision = 0;	
	RigidProbeHIOBoundary* probeBound;	
	CollisionEnabledBoundary* CBound;			
	
	// get number of collision
	numOfCollision = collisionInfos.size();	
	//printf("num of collision = %d\n", numOfCollision);		
	
	// Collision Occurs				
	if(numOfCollision>0)
	{	
		for(int i=0;i<numOfCollision;i++)
		{			
			//printf("Haptic tip: [%.4lf %4lf %4lf]\n At Face:[%d] \n Collision Point:[%.4lf %.4lf %.4lf]\n",
			//	collisionInfoList[i].HTip.pos[0],collisionInfoList[i].HTip.pos[1],collisionInfoList[i].HTip.pos[2], 
			//	collisionInfoList[i].faceIndex, 			
			//	collisionInfoList[i].contacts.pos[0],collisionInfoList[i].contacts.pos[1],collisionInfoList[i].contacts.pos[2]);		
			probeBound = (RigidProbeHIOBoundary *) collisionInfos.get(i)->bObj1;
			CBound = (CollisionEnabledBoundary *) collisionInfos.get(i)->bObj2;
			
			// If the cr_enable of boundary is set, then do the collision response
			if (!CBound->GetResponse()) continue;
			
			// find the nearest node
			unsigned int faceIndex = ((HapticCollisionInfo*)collisionInfos.get(i))->faceIndex;
			unsigned int nearestNode = findNearestVertex(((HapticCollisionInfo*)collisionInfos.get(i))->contacts.pos,
															CBound->face[faceIndex].vertex[0]->pos,
															CBound->face[faceIndex].vertex[1]->pos,
															CBound->face[faceIndex].vertex[2]->pos);
			unsigned int nodeIndex = CBound->face[faceIndex].vertex[nearestNode]->refid;
			//printf("nodeIndex = %d\n",nodeIndex);
			// check the nodeIndex is not a fixed boundary node (type 1 boundary) note:nodeIndex is geometry index
			if(!CBound->isTypeOneBoundary(nodeIndex)) {
				
				// apply the boundary type 1 to that deformable solid boundary node
				CBound->Set(nodeIndex, 1, ((HapticCollisionInfo*)collisionInfos.get(i))->HTip.pos, 0.0, zero_vector3);
				
				// call returnHapticModel function from deformable solid boundary 
				GiPSiLowOrderLinearHapticModel *LModel = new GiPSiLowOrderLinearHapticModel();
				int result = CBound->ReturnHapticModel(nodeIndex, *LModel);
				
				if (result==0) {																			
					// set the HapticModel
					probeBound->SetHapticModel(nodeIndex, *LModel);
					//printf("collision at msd node: %d, haptic node:%d\n",nodeIndex,collisionInfoList[i].HTip.refid);
				}	
				delete LModel;			
			}			
		}		
	}		
}

/**
 * TLHapticCollision::findNearestVertex()
 * utility function to fine the nearest vertex
 * @param v contact position
 * @param v0 vertex0 position
 * @param v1 vertex1 position
 * @param v2 vertex2 position
 * @return int index of the nearest contect point (0,1,2)
 */
unsigned int TLHapticCollision::findNearestVertex(Vector<Real> v, Vector<Real> v0, Vector<Real> v1, Vector<Real> v2)
{
	unsigned int index=0;
	Real dis0 = (v-v0).length();
	Real dis1 = (v-v1).length();
	Real dis2 = (v-v2).length();
	
	if (dis0<dis1)
	{
		if (dis0<dis2) index = 0;
		else index = 2;		
	}
	else 
	{
		if (dis1<dis2) index = 1;
		else index = 2;
	}
	return index;
}

/**
 * TLHapticCollision::print()
 * print collision information.
 */
void TLHapticCollision::print(void)
{	
	int numOfCollision;
	HapticCollisionInfo* cInfo;		

	numOfCollision = collisionInfos.size();
	printf("num of collision = %d\n",numOfCollision);
	printf("-----------------------------------------\n");
	for(int i=0;i<numOfCollision;i++)
	{			
		cInfo = (HapticCollisionInfo*)collisionInfos.get(i);
		printf("face:[%d] pos:[%.4lf %.4lf %.4lf]\n",cInfo->faceIndex, cInfo->contacts.pos[0],cInfo->contacts.pos[1],cInfo->contacts.pos[2]);		
	}
	printf("=========================================\n");	
}