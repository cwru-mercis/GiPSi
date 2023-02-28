/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is NetGen Neutral File Format Loader Implementation (load_neutral.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

/*    load_neutral.cpp
 *
 *    v0.1.0
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "load_mesh.h"

#define EMPTY_LINE_CHAR 10
#define DELIM " \t"
#define	MAX_BUFFER_SIZE	1024



////////////////////////////////////////////////////////////////
//
//	updateBBox()
//
//		updates the bounding box values
//
void updateBBox(LoadData *data, double x, double y, double z)
{
	if(x < data->minx)	data->minx = x;
	if(y < data->miny)	data->miny = y;
	if(z < data->minz)	data->minz = z;

	if(x > data->maxx)	data->maxx = x;
	if(y > data->maxy)	data->maxy = y;
	if(z > data->maxz)	data->maxz = z;
}



////////////////////////////////////////////////////////////////
//
//	Load_Neutral()
//
//	    Reads in .neu files
//
LoadData* LoadNeutral(const char *filename)
{
    FILE		*fp;
    int			i, j;
    char		buffer[MAX_BUFFER_SIZE], 
				*token;
    int			dim, 
				node_no, 
				node_offset;
    double		x, y, z;
    LoadData	*data;

    fp = fopen(filename, "r");

    if(fp == NULL) {
		printf("Cannot open %s\n", filename);
		return NULL;
    }

    data = (LoadData *) malloc(sizeof(LoadData));

    data->num_node		= 0;
    data->num_element	= 0;
    data->num_face		= 0;

	node_offset			= 1;

	data->maxx = data->maxy = data->maxz = -DBL_MAX;
	data->minx = data->miny = data->minz =  DBL_MAX;

    /****************************************/
    /*	    Read nodes   					*/
    /****************************************/
    fscanf(fp, "%d\n", &(data->num_node));

    data->node = (LDNode *) malloc(sizeof(LDNode)*data->num_node);
    if(data->node == NULL) {
    	printf("Cannot allocate memory for vertices!\n");
    }

    i = 0;
    while (i < data->num_node) {
		fgets(buffer, MAX_BUFFER_SIZE, fp);

		node_no	= i;
		x	= atof(strtok(buffer, DELIM));
		y	= atof(strtok(NULL, DELIM));
		z	= atof(strtok(NULL, DELIM));

		updateBBox(data, x, y, z);

		data->node[i].pos[0] = x; 
		data->node[i].pos[1] = y; 
		data->node[i].pos[2] = z;

		data->node[i].num_element	= 0;
		data->node[i].element		= NULL;

		i++;	
    }

    /****************************************/
    /*	    Read elements   				*/
    /****************************************/
    fscanf(fp, "%d\n", &(data->num_element));

	data->type		= TETRAHEDRA;
	int	num_node	= 4;

    data->element = (LDElement *) malloc(sizeof(LDElement)*data->num_element);
    if(data->element == NULL) {
    	printf("Cannot allocate memory for elements!\n");
    }

    i = 0;
    while (i < data->num_element) {
		fgets(buffer, MAX_BUFFER_SIZE, fp);

		token = strtok(buffer, DELIM);

		for(j = 0; j < num_node; j++) {
			data->element[i].node[j] = atoi(strtok(NULL, DELIM)) - node_offset;
			data->node[data->element[i].node[j]].num_element++;
		}

		data->element[i].num_node = num_node;

		for(j = 0; j < 4; j++)
			data->element[i].neigh[j] = -2;

		i++;	
    }

	// Allocate element arrays for each node
	for(i=0; i<data->num_node; i++) {
		data->node[i].element = (int *) malloc(sizeof(int)*data->node[i].num_element);
		data->node[i].num_element = 0;
	}
	
	// Fill in element arrays for each node
	for(i=0; i<data->num_element; i++)
		for(j=0; j<data->element[i].num_node; j++) {
			LDNode &n = data->node[data->element[i].node[j]];
			n.element[n.num_element++] = i;
		}
	

    /****************************************/
    /*	    Read faces   					*/
    /****************************************/
    fscanf(fp, "%d\n", &(data->num_face));

    data->face = (LDFace *) malloc(sizeof(LDFace)*data->num_face);
    if(data->face == NULL) {
    	printf("Cannot allocate memory for faces!\n");
    }

    i = 0;
    while (i < data->num_face) {
		fgets(buffer, MAX_BUFFER_SIZE, fp);

		token = strtok(buffer, DELIM);

		for(j = 0; j < 3; j++)
			data->face[i].node[j] = atoi(strtok(NULL, DELIM)) - node_offset;

		// All faces are on the boundary in neutural format
		data->face[i].boundary = 1;
		for(j = 0; j < 3; j++)
			data->node[data->face[i].node[j]].boundary = 1;

		i++;	
    }

    fclose(fp);

	printf("done\n");

    return data;
}

