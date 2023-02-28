/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Collision Detection with Triangle-Triangle Method Implementation (BasicCollision.h).

The Initial Developers of the Original Code are Suriya Natsupakpong.  
Portions created by Suriya Natsupakpong are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	COLLISIONBASIC.cpp	v0.0
////
////	Implementation of:
////		Collision detection with triangle-triangle method
////
////////////////////////////////////////////////////////////////

#include "collision.h"
#include "collisiontest.h"


/**
 *	checkCollisionTL()
 *	Triangle-line intersection test.
 */
int BasicCollision::checkCollisionTL(TriSurface *geom1,        /**< First surface */ 
											  TriSurface *geom2,        /**< Second surface*/ 
											  vector<Point> *contacts0  /**< Contact points between the two surfaces */)
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
    assert(geom1 != NULL);
    assert(geom2 != NULL);
    assert(contacts0 != NULL);

    /* Initializations */
	m1 = geom1->num_face;
	m2 = geom2->num_face;
	
	numOfContact = 0;
	contacts0->clear();

	for(int m=0; m < m1; m++) {
          /* Find the vertices of the mth face of the first surface */
          vector_copy(v0, geom1->face[m].vertex[0]->pos);
          vector_copy(v1, geom1->face[m].vertex[1]->pos);
          vector_copy(v2, geom1->face[m].vertex[2]->pos);


	  for(int n=0; n < m2; n++) {
            /* Find the nth line of the second surface */
            vector_copy(u0, geom2->face[n].vertex[0]->pos);
            vector_copy(u1, geom2->face[n].vertex[1]->pos);
            vector_subtract(u1, u0);
				
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
                /* Add the point of contact to the list */
				contacts0->push_back(pt);
			}
		}
	}
	return numOfContact;	
}


/**
 *	checkCollisionTL2()
 *	Triangle-line intersection test.
 */
int BasicCollision::checkCollisionTL2(TriSurface *geom1,        /**< First surface */ 
											   TriSurface *geom2,        /**< Second surface*/ 
										       vector<Point> *contacts0  /**< Contact points between the two surfaces */)
{
    /* Local variables */
	int collided = 0;        /* Boolean to test if the surfaces collide or not */
	int m1,                  /* Number of faces in geom1 */
        m2;	                 /* Number of faces in geom2 */
	float v0[3],v1[3],v2[3], /* Vertices of a triangle */
          u0[3],u1[3];       /* Endpoints of a line */
	float IPt[3];            /* Intersection Point */
	int coPlanar;            /* Boolean to denote if the triangles are coplanar */
	int numOfContact;        /* Number of contact points */
	Point pt;
    /* Check arguments */
    assert(geom1 != NULL);
    assert(geom2 != NULL);
    assert(contacts0 != NULL);

    /* Initializations */
	m1 = geom1->num_face;
	m2 = geom2->num_face;
	
	numOfContact = 0;
	contacts0->clear();

	for(int m=0; m < m1; m++) {
          /* Find the vertices of the mth face of the first surface */
          vector_copy(v0, geom1->face[m].vertex[0]->pos);
          vector_copy(v1, geom1->face[m].vertex[1]->pos);
          vector_copy(v2, geom1->face[m].vertex[2]->pos);


	  for(int n=0; n < m2; n++) {
            /* Find the nth line of the second surface */
            vector_copy(u0, geom2->face[n].vertex[0]->pos);
            vector_copy(u1, geom2->face[n].vertex[1]->pos);
            vector_subtract(u1, u0);
				
			/* Check if the two surfaces collide */
            collided = intersect_LineTriangle(u0, u1 , v0, v1, v2, IPt);
			//-1 = triangle is degenerate (a segment or point)
			// 0 = disjoint (no intersect)
            // 1 = intersect in unique point I1
            // 2 = are in the same plane

			if (collided) {	
                /* Oh, no! They are colliding! */
				numOfContact++;				
				/* Find the point of contact */
                Point pt;
				//vector_copy(&(pt.pos),IPt);
				pt.pos[0] = IPt[0];
				pt.pos[1] = IPt[1];
				pt.pos[2] = IPt[2];
                /* Add the point of contact to the list */
				contacts0->push_back(pt);
			}
		}
	}
	return numOfContact;	
}

/**
 *	checkCollisionTT()
 *	triangle-triangle intersection test.
 */
int BasicCollision::checkCollisionTT(TriSurface *geom1,        /**< First surface */ 
											  TriSurface *geom2,        /**< Second surface*/ 
											  vector<Point> *contacts0, /**< Contact points 0 between the two surfaces */
											  vector<Point> *contacts1  /**< Contact points 1 between the two surfaces */)									 
{
	int collided = 0;
	int m1,m2;	
	float v0[3],v1[3],v2[3],u0[3],u1[3],u2[3];
	float pt1[3],pt2[3];
	int coPlanar;
	float t,u,v;
	int numOfContact;
	
	/* Check arguments */
    assert(geom1 != NULL);
    assert(geom2 != NULL);
    assert(contacts0 != NULL);
	
	/* Initializations */
	m1 = geom1->num_face;
	m2 = geom2->num_face;
	
	numOfContact = 0;
	contacts0->clear();
	contacts1->clear();

	for(int m=0;m<m1;m++) {		
		/* Find the vertices of the mth face of the first surface */
        vector_copy(v0, geom1->face[m].vertex[0]->pos);
        vector_copy(v1, geom1->face[m].vertex[1]->pos);
        vector_copy(v2, geom1->face[m].vertex[2]->pos);
		
		for(int n=0;n<m2;n++) {			
			/* Find the nth face of the second surface */
            vector_copy(u0, geom2->face[n].vertex[0]->pos);
            vector_copy(u1, geom2->face[n].vertex[1]->pos);
			vector_copy(u2, geom2->face[n].vertex[2]->pos);
						
			 /* Check if the two surfaces collide */
			collided = tri_tri_intersect_with_isectline(v0,v1,v2,u0,u1,u2,&coPlanar,pt1,pt2);
			
			if (collided==1) { 
				/* Oh, no! They are colliding! */
				if(coPlanar==1) {
					/* If it is co planar */
					u1[0] -= u0[0];
					u1[1] -= u0[1];
					u1[2] -= u0[2];
					intersect_triangle(u0, u1 , v0, v1, v2, &t, &u, &v);
					if ((t>=0) && (t<=1)) { 	
						Point pt;
						numOfContact++;
						/* Find the point of contact */
						//convex_combination(&(pt.pos), v0, v1, v2, u, v);
						for (int i = 0; i < 3; i++) 
							pt.pos[i] = (1. - u - v) * v0[i] + u * v1[i] + v * v2[i];
						/* Add the point of contact to the list */
						contacts0->push_back(pt);
					}				
				}
				else {
					/* If it is not co planar */
					Point pt;
					numOfContact++;
					/* Find the point of contact */
					//vector_copy(pt1,pt.pos);
					pt.pos[0] = pt1[0];
					pt.pos[1] = pt1[1];
					pt.pos[2] = pt1[2];
					/* Add the point of contact to the list */
					contacts0->push_back(pt);
					/* Find the point of contact */
					//vector_copy(pt2,pt.pos);
					pt.pos[0] = pt2[0];
					pt.pos[1] = pt2[1];
					pt.pos[2] = pt2[2];
					/* Add the point of contact to the list */
					contacts1->push_back(pt);
				}
			}
		}
	}
	return numOfContact;	
}

/**
 * detection()
 * Find intersect between objects 1 and 2, for all pair of objects.
 */
void BasicCollision::detection(void)
{
	TriSurface *geom1, *geom2;
	int result = 0;	
	int numOfSIMObject;
	int id1, id2;

	/* Clear collisionInfoList everytime */
	collisionInfos.clear();
	numOfSIMObject = bvList->size();

	/* For all SimObject List in Collision class */
	for(int i=0;i<numOfSIMObject;i++) {
		for(int j=i+1;j<numOfSIMObject;j++) {	

			id1 = bvList->get(i)->getBoundary()->Object->GetID();
			id2 = bvList->get(j)->getBoundary()->Object->GetID();
			if (collisionRule->isCollisionTest(id1, id2)==0) continue;

			result = 0;
			vector<Point> contacts0;
			vector<Point> contacts1;

			/* Initializations */
			geom1 = (TriSurface *) (bvList->get(i)->getBoundary());
			geom2 = (TriSurface *) (bvList->get(j)->getBoundary());

			/* A tri-tri intersection test */
			result = checkCollisionTT(geom1, geom2, &contacts0, &contacts1);			
			//result = checkCollisionTL(geom1, geom2, &contacts0);			
			//result = checkCollisionTL(geom1, geom2, &contacts0);			
			if (result > 0) {
				/* Find intersection, set info into collision info */
				BasicCollisionInfo *cInfo = new BasicCollisionInfo();
				cInfo->bObj1 = bvList->get(i)->getBoundary();
				cInfo->bObj2 = bvList->get(j)->getBoundary();
				cInfo->numOfContact = result;
				cInfo->contacts0 = contacts0;
				cInfo->contacts1 = contacts1;
				/* Add the collision info into the list */
				collisionInfos.add(cInfo);
			} 	
		}
	}	
}

/**
 * response() 
 */
void BasicCollision::response(void)
{
}

/**
 * print()
 * Print the collision result.
 */
void BasicCollision::print(void)
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
			BasicCollisionInfo *cInfo = (BasicCollisionInfo*)collisionInfos.get(i);
			printf(" %d : at %.4lf %.4lf %.4lf\n",j,cInfo->contacts0[j].pos[0],cInfo->contacts0[j].pos[1],cInfo->contacts0[j].pos[2]);
		}
		printf("-----------------------------------------\n");
	}
	printf("=========================================\n");		
}

