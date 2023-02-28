/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Pyramid/Triangle File Format Loader Implementation (load_node.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

/*    load_node.cpp
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


/*
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
*/


////////////////////////////////////////////////////////////////
//
//	Create_Neigh()
//
//		Creates .neigh files
//
int Create_Neigh(const char *basename, int element_offset, LoadData *data)
{
    FILE    *fp;
    char    *filename;
    int	    i, j, k, m;
    int	    num_neigh;

	printf("\tCreating Neighbours ...\t");

    filename = (char *) malloc(sizeof(char)*(strlen(basename)+7));
    sprintf(filename,"%s.neigh", basename);
    fp = fopen(filename, "w");

    if(fp == NULL) {
		printf("Cannot open %s\n", filename);
		return 0;
    }

	if(data->type == TRIANGLE)		num_neigh = 3;
	if(data->type == TETRAHEDRA)	num_neigh = 4;

    /****************************************/
    /*		Write header information		*/
    /****************************************/
    fprintf(fp, "%d\t%d\n", data->num_element, num_neigh);


	/****************************************/
    /*	    Write neighbours   				*/
    /****************************************/
	i = 0;
    while (i < data->num_element) {
		LDElement *e = &(data->element[i]);
		for(j=0; j<e->num_node; j++) {
			// Get the nodes opposite to node j
			int		p = (j+1) % e->num_node;
			int		q = (j+2) % e->num_node;
			int		r = (j+3) % e->num_node;

			//NOTE: This part only works for 3D
			
			LDNode &pn = data->node[e->node[p]];
			LDNode &qn = data->node[e->node[q]];
			LDNode &rn = data->node[e->node[r]];

			// Go through each element incident to one of the
			// opposite nodes (such pn)
			for(k=0; k<pn.num_element; k++) {
				LDElement *ek = &(data->element[pn.element[k]]);
				
				if(ek == e) continue;

				// Look if kth element incident to pn contains all
				// of pn, qn and rn
				int	hits = 0;
				for(m=0; m<ek->num_node; m++) {
					if(ek->node[m] == e->node[p] || ek->node[m] == e->node[q] || ek->node[m] == e->node[r])
						hits++;
				}
				if(hits == 3) {	// Neighbour found
					e->neigh[j] = pn.element[k];
				}
			}
		}

		fprintf(fp, "%d\t%d\t%d\t%d\t%d\n", i+element_offset,
											e->neigh[0]+element_offset,
											e->neigh[1]+element_offset,
											e->neigh[2]+element_offset,
											e->neigh[3]+element_offset);
		i++;
	}

	fclose(fp);

	printf("done\n");

	return 1;
}



////////////////////////////////////////////////////////////////
//
//	Load_Neigh()
//
//		Reads in .neigh files
//
int Load_Neigh(const char *basename, int element_offset, LoadData *data)
{
    FILE    *fp;
    char    *filename;
    int	    i, j;
    char    buffer[MAX_BUFFER_SIZE], 
			*token;
    int	    num_element,
			element_no,
			num_neigh,
			neigh_no;

	printf("\tLoading Neighbours ...\t\t");

    filename = (char *) malloc(sizeof(char)*(strlen(basename)+7));
    sprintf(filename,"%s.neigh", basename);
    fp = fopen(filename, "r");

    if(fp == NULL) {
		printf("Cannot open %s\n", filename);
		return 0;
    }

    /****************************************/
    /*		Read header information			*/
    /****************************************/
    fscanf(fp, "%d%d\n", &(num_element), &num_neigh);

	if(num_element != data->num_element) {
		printf("Invalid neighbour file!\n");
		fclose(fp);
		return 0;
	}

	/****************************************/
    /*	    Read neighbours   				*/
    /****************************************/
	i = 0;
    while (i < num_element) {
		fgets(buffer, MAX_BUFFER_SIZE, fp);

		token = strtok(buffer, DELIM);

		// Skip comments and blank lines
		if(token[0] == '#' || token[0] == EMPTY_LINE_CHAR) continue;

		element_no = atoi(token);

		for(j=0; j<num_neigh; j++) {
			neigh_no = atoi(strtok(NULL, DELIM));
			if(neigh_no != -1)
				data->element[element_no-element_offset].neigh[j] = neigh_no - element_offset;
			else
				data->element[element_no-element_offset].neigh[j] = neigh_no;
		}
		
		i++;
	}

	fclose(fp);

	printf("done\n");

	return 1;
}



////////////////////////////////////////////////////////////////
//
//	Load_Ele()
//
//		Reads in .ele files
//
int Load_Ele(const char *basename, int node_offset, LoadData *data)
{
    FILE    *fp;
    char    *filename;
    int	    i, j;
    char    buffer[MAX_BUFFER_SIZE], 
			*token;
    int	    element_no, 
			element_offset, 
			num_node, 
			num_attrib;

	printf("\tLoading Elements ...\t\t");

    filename = (char *) malloc(sizeof(char)*(strlen(basename)+5));
    sprintf(filename,"%s.ele", basename);
    fp = fopen(filename, "r");

    if(fp == NULL) {
		printf("Cannot open %s\n", filename);
		return 0;
    }

    /****************************************/
    /*		Read header information			*/
    /****************************************/
    fscanf(fp, "%d%d%d\n", &(data->num_element), &num_node, &num_attrib);

    if(num_node == 3) data->type = TRIANGLE;
    if(num_node == 4) data->type = TETRAHEDRA;

    /****************************************/
    /*	Allocate continuous memory			*/
    /****************************************/
    data->element = (LDElement *) malloc(sizeof(LDElement)*data->num_element);
    if(data->element == NULL) {
    	printf("Cannot allocate memory for elements!\n");
    }

    /****************************************/
    /*	    Read elements   				*/
    /****************************************/
    i = 0;
    while (i < data->num_element) {
		fgets(buffer, MAX_BUFFER_SIZE, fp);

		token = strtok(buffer, DELIM);

		// Skip comments and blank lines
		if(token[0] == '#' || token[0] == EMPTY_LINE_CHAR) continue;

		element_no = atoi(token);
		if(i == 0) element_offset = element_no;

		for(j = 0; j < num_node; j++) {
			data->element[i].node[j] = atoi(strtok(NULL, DELIM)) - node_offset;
			data->node[data->element[i].node[j]].num_element++;
		}

		data->element[i].num_node = num_node;

		for(j = 0; j < 4; j++)
			data->element[i].neigh[j] = -2;

		// Record attributes
		if(num_attrib > 0) {
			data->element[i].attrib = (double *) malloc(sizeof(double)*num_attrib);
			for(j = 0; j < num_attrib; j++) 
    			data->element[i].attrib[j] = atof(strtok(NULL, DELIM));
			data->element[i].num_attrib = num_attrib;
		} else data->element[i].attrib = NULL;

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
	
	fclose(fp);

	printf("done\n");

	if(!Load_Neigh(basename, element_offset, data))
		Create_Neigh(basename, element_offset, data);

    return 1;

}




////////////////////////////////////////////////////////////////
//
//	Load_Edge()
//
//		Reads in .edge files
//
int Load_Edge(const char *basename, int node_offset, LoadData *data)
{
    FILE    *fp;
    char    *filename;
    int	    i, j;
    char    buffer[MAX_BUFFER_SIZE], 
			*token;
    int	    edge_no, 
			edge_offset, 
			num_boundary;

	printf("\tLoading Edges ...\t\t");

    filename = (char *) malloc(sizeof(char)*(strlen(basename)+6));
    sprintf(filename,"%s.edge", basename);
    fp = fopen(filename, "r");

    if(fp == NULL) {
		printf("No edge file present %s\n", filename);
		return 0;
    }

    /****************************************/
    /*		Read header information			*/
    /****************************************/
    fscanf(fp, "%d%d\n", &(data->num_edge), &num_boundary);

    /****************************************/
    /*	Allocate continuous memory			*/
    /****************************************/
    data->edge = (LDEdge *) malloc(sizeof(LDEdge)*data->num_edge);
    if(data->edge == NULL) {
    	printf("Cannot allocate memory for edges!\n");
    }

    /****************************************/
    /*	    Read edges   					*/
    /****************************************/
    i = 0;
    while (i < data->num_edge) {
		fgets(buffer, MAX_BUFFER_SIZE, fp);

		token = strtok(buffer, DELIM);

		// Skip comments and blank lines
		if(token[0] == '#' || token[0] == EMPTY_LINE_CHAR) continue;

		edge_no = atoi(token);
		if(i == 0) edge_offset = edge_no;

		for(j = 0; j < 2; j++)
			data->edge[i].node[j] = atoi(strtok(NULL, DELIM)) - node_offset;

		// Record boundary values
		if(num_boundary == 1) {
			data->edge[i].boundary = atoi(strtok(NULL, DELIM));
		} else {
			data->edge[i].boundary = -1;
		}

		i++;	
    }

    fclose(fp);

	printf("done\n");

    return 1;

}




////////////////////////////////////////////////////////////////
//
//	Load_Face()
//
//		Reads in .face files
//
int Load_Face(const char *basename, int node_offset, LoadData *data)
{
    FILE    *fp;
    char    *filename;
    int	    i, j;
    char    buffer[MAX_BUFFER_SIZE], 
			*token;
    int	    face_no, 
			face_offset, 
			num_boundary;

	printf("\tLoading Faces ...\t\t");

    filename = (char *) malloc(sizeof(char)*(strlen(basename)+6));
    sprintf(filename,"%s.face", basename);
    fp = fopen(filename, "r");

    if(fp == NULL) {
		printf("No face file present %s\n", filename);
		return 0;
    }

    /****************************************/
    /*		Read header information			*/
    /****************************************/
    fscanf(fp, "%d%d\n", &(data->num_face), &num_boundary);

    /****************************************/
    /*	Allocate continuous memory			*/
    /****************************************/
    data->face = (LDFace *) malloc(sizeof(LDFace)*data->num_face);
    if(data->face == NULL) {
    	printf("Cannot allocate memory for faces!\n");
    }

    /****************************************/
    /*	    Read faces   					*/
    /****************************************/
    i = 0;
    while (i < data->num_face) {
		fgets(buffer, MAX_BUFFER_SIZE, fp);

		token = strtok(buffer, DELIM);

		// Skip comments and blank lines
		if(token[0] == '#' || token[0] == EMPTY_LINE_CHAR) continue;

		face_no = atoi(token);
		if(i == 0) face_offset = face_no;

		for(j = 0; j < 3; j++)
			data->face[i].node[j] = atoi(strtok(NULL, DELIM)) - node_offset;

		// Record boundary values
		if(num_boundary == 1) {
			data->face[i].boundary = atoi(strtok(NULL, DELIM));
		} else {
			data->face[i].boundary = -1;
		}

		i++;	
    }

    fclose(fp);

	printf("done\n");

    return 1;

}




////////////////////////////////////////////////////////////////
//
//	Load_Node()
//
//	    Reads in .node files
//
LoadData* LoadNode(const char *basename)
{
    FILE		*fp;
    char		*filename;
    int			i, j;
    char		buffer[MAX_BUFFER_SIZE], 
				*token;
    int			dim, 
				node_no, 
				node_offset, 
				num_attrib, 
				num_boundary;
    double		x, y, z;
    LoadData	*data;

	printf("\n\tLoading Nodes ...\t\t");

    filename = (char *) malloc(sizeof(char)*(strlen(basename)+6));
    sprintf(filename,"%s.node", basename);
    fp = fopen(filename, "r");

    if(fp == NULL) {
		printf("Cannot open %s\n", filename);
		return NULL;
    }

    data = (LoadData *) malloc(sizeof(LoadData));

    data->num_node		= 0;
    data->num_element	= 0;
    data->num_edge		= 0;
    data->num_face		= 0;

	data->maxx = data->maxy = data->maxz = -DBL_MAX;
	data->minx = data->miny = data->minz =  DBL_MAX;

    /****************************************/
    /*		Read header information			*/
    /****************************************/
    fscanf(fp, "%d%d%d%d\n", &(data->num_node), &dim, &num_attrib, &num_boundary);


    /****************************************/
    /*	Allocate continuous memory  	    */
    /****************************************/
    data->node = (LDNode *) malloc(sizeof(LDNode)*data->num_node);
    if(data->node == NULL) {
    	printf("Cannot allocate memory for vertices!\n");
    }

    /****************************************/
    /*	    Read nodes   					*/
    /****************************************/
    i = 0;
    while (i < data->num_node) {
		fgets(buffer, MAX_BUFFER_SIZE, fp);
		token = strtok(buffer, DELIM);

		// Skip comments and blank lines
		if(token[0] == '#' || token[0] == EMPTY_LINE_CHAR) continue;

		node_no	= atoi(token);
		x	= atof(strtok(NULL, DELIM));
		y	= atof(strtok(NULL, DELIM));
		if(dim == 2) z = 0;
		else if (dim == 3) z = atof(strtok(NULL, DELIM));

		updateBBox(data, x, y, z);

		// Record attributes
		if(num_attrib > 0) {
			data->node[i].attrib = (double *) malloc(sizeof(double)*num_attrib);
			for(j = 0; j < num_attrib; j++) 
    			data->node[i].attrib[j] = atof(strtok(NULL, DELIM));
			data->node[i].num_attrib = num_attrib;
		} else data->node[i].attrib = NULL;

		// Record boundary values
		if(num_boundary == 1) {
			data->node[i].boundary = atoi(strtok(NULL, DELIM));
		} else {
			data->node[i].boundary = false;
		}

		if(i == 0) node_offset = node_no;

		data->node[i].pos[0] = x; 
		data->node[i].pos[1] = y; 
		data->node[i].pos[2] = z;

		data->node[i].num_element	= 0;
		data->node[i].element		= NULL;

		i++;	
    }
 
    fclose(fp);

	printf("done\n");

    if (!Load_Ele(basename, node_offset, data)) return NULL;

	// Find boundary nodes if data is missing
	if(num_boundary == 0) {
		for(i=0; i<data->num_element; i++) {
			LDElement	&e = data->element[i];
			for(j=0; j<e.num_node; j++)
				if(e.neigh[j] < 0) {
					for(int k=1; k<e.num_node; k++)
						data->node[e.node[(j+k) % e.num_node]].boundary = true;
				}
		}
	}
    
	if(data->type == TRIANGLE)
		Load_Edge(basename, node_offset, data);

    if (data->type == TETRAHEDRA)
		Load_Face(basename, node_offset, data);

    return data;
}

