/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSiDisplay Implementation (GiPSiDisplay.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	GiPSiDisplay.cpp v1.0
////
////	Implements TriSurfaceDisplayManager and TetVolumeDisplayManager
////
////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "GiPSiDisplay.h"
#include "algebra.h"
#include "errors.h"


////////////////////////////////////////////////////////////////
//
//	TriSurfaceDisplayManager::Display()
//
//		Calculates the face normals of the object
//
void TriSurfaceDisplayManager::Display(void)
{
    unsigned int	i;
	int				offset;
	char			type = display.header.dataType;
    float*			darray = display.dispArray;
    unsigned int*	iarray = display.indexArray;

	geometry->calcNormals();

    for(i=0; i<geometry->num_vertex; i++) {
		offset = 0;

		if(IS_TEXCOORD_ON(type)) {
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].texcoord[0];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].texcoord[1];
		}

		if(IS_RGB_ON(type)) {
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[0];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[1];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[2];
		} else if(IS_RGBA_ON(type)) {
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[0];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[1];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[2];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[3];
		}

		if(IS_NORMAL_ON(type)) {
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].n[0];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].n[1];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].n[2];
		}

		darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[0];
		darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[1];
		darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[2];
    }

    for(i=0; i<geometry->num_face; i++) {
		iarray[(i*3)]	= geometry->face[i].vertex[0]->refid;
		iarray[(i*3)+1] = geometry->face[i].vertex[1]->refid;
		iarray[(i*3)+2] = geometry->face[i].vertex[2]->refid;
    }
}




////////////////////////////////////////////////////////////////
//
//	TetVolumeDisplayManager::Display()
//
//		
//
void TetVolumeDisplayManager::Display(void)
{
    unsigned int	i;
	int				offset;
	char			type = display.header.dataType;
    float*			darray = display.dispArray;
    unsigned int*	iarray = display.indexArray;

    for(i=0; i<geometry->num_vertex; i++) {
		offset = 0;

		if(IS_TEXCOORD_ON(type)) {
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].texcoord[0];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].texcoord[1];
		}

		if(IS_RGB_ON(type)) {
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[0];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[1];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[2];
		} else if(IS_RGBA_ON(type)) {
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[0];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[1];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[2];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[3];
		}

		if(IS_NORMAL_ON(type)) {
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].n[0];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].n[1];
			darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].n[2];
		}

		darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[0];
		darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[1];
		darray[(i*display.DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[2];
    }

	if(display.header.objType==GIPSI_DRAW_TRIANGLE) {
		// if object type is GIPSI_DRAW_TRIANGLE then we will draw the faces
		for(i=0; i<geometry->num_face; i++) {
			iarray[(i*3)]	= geometry->face[i].vertex[0]->refid;
			iarray[(i*3)+1] = geometry->face[i].vertex[1]->refid;
			iarray[(i*3)+2] = geometry->face[i].vertex[2]->refid;
		}
	}
	else if (display.header.objType==GIPSI_DRAW_LINE){
		// if object type is GIPSI_DRAW_LINE then we will draw the tet edges
		for(i=0; i<geometry->num_tet; i++) {
			iarray[(i*6*2)+0]		= geometry->tet[i].vertex[0]->refid;
			iarray[(i*6*2)+1]		= geometry->tet[i].vertex[1]->refid;
			iarray[(i*6*2)+2]		= geometry->tet[i].vertex[0]->refid;
			iarray[(i*6*2)+3]		= geometry->tet[i].vertex[2]->refid;
			iarray[(i*6*2)+4]		= geometry->tet[i].vertex[0]->refid;
			iarray[(i*6*2)+5]		= geometry->tet[i].vertex[3]->refid;
			iarray[(i*6*2)+6]		= geometry->tet[i].vertex[1]->refid;
			iarray[(i*6*2)+7]		= geometry->tet[i].vertex[2]->refid;
			iarray[(i*6*2)+8]		= geometry->tet[i].vertex[1]->refid;
			iarray[(i*6*2)+9]		= geometry->tet[i].vertex[3]->refid;
			iarray[(i*6*2)+10]		= geometry->tet[i].vertex[2]->refid;
			iarray[(i*6*2)+11]		= geometry->tet[i].vertex[3]->refid;
		}
	}
	else {
		// Unknown type
		error_exit(1,"Unknown object type for TetVolume Display Manager");
	}
}