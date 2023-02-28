/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Mesh Loader Header (load_mesh.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

#ifndef _LOAD_MESH_H
#define _LOAD_MESH_H

typedef enum {TRIANGLE, TETRAHEDRA} MeshType;


typedef struct {
    double		pos[3];
	double		normal[3];
	double		tangent[3];
	double		texcoords[2];
	int			*element;
	int			num_element;
    double		*attrib;
    int			num_attrib;
    int			boundary;
} LDNode;

typedef struct {
    int			node[4];
    int			num_node;
	int			neigh[4];
    double		*attrib;
    int			num_attrib;
    int			boundary;
} LDElement;

typedef struct {
    int			node[3];
    int			boundary;
} LDFace;

typedef struct {
    int			node[2];
    int			boundary;
} LDEdge;

typedef struct {
    MeshType    type;
    LDNode		*node;
    int			num_node;
    LDElement	*element;
    int			num_element;
    LDFace		*face;
    int			num_face;
    LDEdge		*edge;
    int			num_edge;
	double		maxx,
				maxy,
				maxz,
				minx,
				miny,
				minz;
} LoadData;


LoadData* LoadNode(const char *basename);
LoadData* LoadNeutral(const char *filename);
LoadData* LoadObj(const char *filename);

void updateBBox(LoadData *data, double x, double y, double z);

#endif

