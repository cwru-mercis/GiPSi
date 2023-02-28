/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is obj loader (load_obj.cpp).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen.
*/

////	LOAD_OBJ.CPP v0.1.0
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include <float.h>								// Needed for DBL_MAX
#include <stdlib.h>
#include <stdio.h>
#include <string.h>								// Needed for strstr

#include "algebra.h"
#include "load_mesh.h"

/*
===============================================================================
	LoadObj
===============================================================================
*/

#define N(x) (data->node[(x)])
#define F(x) (data->face[(x)])

struct ModelInfo
{
	int nNormals;
	int nTexcoords;
	int nTangents;
};

void		CalculateNormals(LoadData *data);
void		CalculateTangents(LoadData *data);
void		FirstPass(ModelInfo &info, FILE *file, LoadData *data);
LoadData*	LoadObj	(const char *filename);
void		NormalizeNormals(LoadData *data);
void		ResetTextureCoordinates(LoadData *data);
void		SecondPass(ModelInfo &info, FILE *file, LoadData *data);
void		UpdateBBox(LoadData *data);

void CalculateNormals(LoadData *data)
{
	double v1[3], v2[3], v1xv2[3], fN[3];
	double length;

	// Set all vertex normals to zero

	for (int i=0; i<data->num_node; i++)
	{
		N(i).normal[0] = 0.0;
		N(i).normal[1] = 0.0;
		N(i).normal[2] = 0.0;
	}

	// Compute the normal of each face and add it to the vertices belonging to
	// that face.

	for (int i=0; i<data->num_face; i++)
	{
		int nVerticesPerFace = 3;

		v1[0] = N(F(i).node[1]).pos[0] - N(F(i).node[0]).pos[0];
		v1[1] = N(F(i).node[1]).pos[1] - N(F(i).node[0]).pos[1];
		v1[2] = N(F(i).node[1]).pos[2] - N(F(i).node[0]).pos[2];
		v2[0] = N(F(i).node[2]).pos[0] - N(F(i).node[0]).pos[0];
		v2[1] = N(F(i).node[2]).pos[1] - N(F(i).node[0]).pos[1];
		v2[2] = N(F(i).node[2]).pos[2] - N(F(i).node[0]).pos[2];

		v1xv2[0] = v1[1]*v2[2] - v1[2]*v2[1];
		v1xv2[1] = v1[2]*v2[0] - v1[0]*v2[2];
		v1xv2[2] = v1[0]*v2[1] - v1[1]*v2[0];

		length = sqrt(v1xv2[0]*v1xv2[0] + v1xv2[1]*v1xv2[1] + v1xv2[2]*v1xv2[2]);

		// The face normal is the normalized cross product

		fN[0] = v1xv2[0] / length;
		fN[1] = v1xv2[1] / length;
		fN[2] = v1xv2[2] / length;

		// Add the face normal to all adjacent vertices

		for (int j=0; j<nVerticesPerFace; j++)
		{
			N(F(i).node[j]).normal[0] += fN[0];
			N(F(i).node[j]).normal[1] += fN[1];
			N(F(i).node[j]).normal[2] += fN[2];
		}
	}

	// Normalize the newly obtained vertex normals

	for (int i=0; i<data->num_node; i++)
	{
		length = sqrt(N(i).normal[0]*N(i).normal[0] + N(i).normal[1]*N(i).normal[1] + N(i).normal[2]*N(i).normal[2]);

		N(i).normal[0] = N(i).normal[0] / length;
		N(i).normal[1] = N(i).normal[1] / length;
		N(i).normal[2] = N(i).normal[2] / length;
	}
}

void CalculateTangents(LoadData *data)
{
	double t1, t2, b1, b2, r, length;
	double v1[3], v2[3], MP[3], fT[3];

	// Set all tangents to zero

	for (int i=0; i<data->num_node; i++)
	{
		N(i).tangent[0] = 0.0;
		N(i).tangent[1] = 0.0;
		N(i).tangent[2] = 0.0;
	}

	// Compute the tangent of each face and add it to the vertices belonging to
	// that face.

	for (int i=0; i<data->num_face; i++)
	{
		int nVerticesPerFace = 3;

		// Tangent vectors are only computed if actual texture coordinates have
		// been assigned to a given face.

		if (N(F(i).node[0]).texcoords[0] == 0.0 && N(F(i).node[0]).texcoords[1] == 0.0 &&
			N(F(i).node[1]).texcoords[0] == 0.0 && N(F(i).node[1]).texcoords[1] == 0.0 &&
			N(F(i).node[2]).texcoords[0] == 0.0 && N(F(i).node[2]).texcoords[1] == 0.0	 )
		{
			// Do nothing
		}
		else
		{
			t1 = N(F(i).node[1]).texcoords[0] - N(F(i).node[0]).texcoords[0];
			t2 = N(F(i).node[2]).texcoords[0] - N(F(i).node[0]).texcoords[0];
			b1 = N(F(i).node[1]).texcoords[1] - N(F(i).node[0]).texcoords[1];
			b2 = N(F(i).node[2]).texcoords[1] - N(F(i).node[0]).texcoords[1];

			// v1: vert2 - vert1 and v2: vert3 - vert1

			v1[0] = N(F(i).node[1]).pos[0] - N(F(i).node[0]).pos[0];
			v1[1] = N(F(i).node[1]).pos[1] - N(F(i).node[0]).pos[1];
			v1[2] = N(F(i).node[1]).pos[2] - N(F(i).node[0]).pos[2];
			v2[0] = N(F(i).node[2]).pos[0] - N(F(i).node[0]).pos[0];
			v2[1] = N(F(i).node[2]).pos[1] - N(F(i).node[0]).pos[1];
			v2[2] = N(F(i).node[2]).pos[2] - N(F(i).node[0]).pos[2];

			r = 1.0 / (t1*b2 - t2*b1);

			// Matrix vector product
			//
			// | T |	=	1/r * |  b2 -b1 | | v1 |
			// | B |			  | -t2  t1 | | v2 |

			MP[0] = (b2*v1[0] - b1*v2[0]) * r;
			MP[1] = (b2*v1[1] - b1*v2[1]) * r;
			MP[2] = (b2*v1[2] - b1*v2[2]) * r;

			length = sqrt(MP[0]*MP[0] + MP[1]*MP[1] + MP[2]*MP[2]);

			// The normalized face tangent

			fT[0] = MP[0] / length;
			fT[1] = MP[1] / length;
			fT[2] = MP[2] / length;
			
			for (int j=0; j<nVerticesPerFace; j++)
			{
				N(F(i).node[j]).tangent[0] += fT[0];
				N(F(i).node[j]).tangent[1] += fT[1];
				N(F(i).node[j]).tangent[2] += fT[2];
			}
		}
	}

	// Normalize the obtained tangent vectors

	for (int i=0; i<data->num_node; i++)
	{
		length = sqrt(N(i).tangent[0]*N(i).tangent[0] + N(i).tangent[1]*N(i).tangent[1] + N(i).tangent[2]*N(i).tangent[2]);

		// Avoid division by zero for vertices with no assigned texture
		// coordinates.

		if (length > 0.0)
		{
			N(i).tangent[0] = N(i).tangent[0] / length;
			N(i).tangent[1] = N(i).tangent[1] / length;
			N(i).tangent[2] = N(i).tangent[2] / length;
		}
	}
}

void FirstPass(ModelInfo &info, FILE *file, LoadData *data)
{
	int  nNodes;
	int  nNormals;
	int  nTexcoords;
	int  nFaces;

	unsigned int v, n, t;
	char buf[128];

	// Count the number of nodes, normals, texcoordinates and faces

	nNodes		= 0;
	nNormals	= 0;
	nTexcoords	= 0;
	nFaces		= 0;

	while (fscanf(file, "%s", buf) != EOF)
	{
		switch (buf[0])
		{
		case '#': // Comment
			fgets(buf, sizeof(buf), file);
			break;
		case 'v': // v, vn or vt
			switch(buf[1])
			{
            case '\0': // Node
                fgets(buf, sizeof(buf), file);
                nNodes++;
                break;
            case 'n': // Normal
                fgets(buf, sizeof(buf), file);
				nNormals++;
                break;
            case 't': // Texcoord
                fgets(buf, sizeof(buf), file);
                nTexcoords++;
                break;
            default:
                printf("FirstPass: Unknown token \"%s\".\n", buf);
                exit(1);
                break;
            }
			break;
		case 'g': // Group
			fgets(buf, sizeof(buf), file);
			// TODO: Add support for groups
			break;
		case 'f': // Face
			v = n = t = 0;
            fscanf(file, "%s", buf);

			// The following face formats are supported v, v//n, v/t and v/t/n
			// Corresponding to input string %d, %d//%d, %d/%d, and %d/%d/%d
            
            if (strstr(buf, "//")) // v//n
            {    
                sscanf(buf, "%d//%d", &v, &n);
                fscanf(file, "%d//%d", &v, &n);
                fscanf(file, "%d//%d", &v, &n);
                nFaces++;
                while(fscanf(file, "%d//%d", &v, &n) > 0)
				{
                    nFaces++;
                }
            }
			else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) // v/t/n
			{
                fscanf(file, "%d/%d/%d", &v, &t, &n);
                fscanf(file, "%d/%d/%d", &v, &t, &n);
                nFaces++;
                while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0)
				{
                    nFaces++;
                }
            }
			else if (sscanf(buf, "%d/%d", &v, &t) == 2) // v/t
			{
                fscanf(file, "%d/%d", &v, &t);
                fscanf(file, "%d/%d", &v, &t);
                nFaces++;
                while(fscanf(file, "%d/%d", &v, &t) > 0)
				{
                    nFaces++;
                }
            }
			else // v
			{
                fscanf(file, "%d", &v);
                fscanf(file, "%d", &v);
                nFaces++;
                while(fscanf(file, "%d", &v) > 0)
				{
                    nFaces++;
                }
            }
			break;
		default:
			fgets(buf, sizeof(buf), file);
            break;
		}
	}

	// Save the stats

	data->num_node	= nNodes;
	data->num_face	= nFaces;
	info.nNormals	= nNormals;
	info.nTexcoords	= nTexcoords;
}

LoadData* LoadObj(const char *filename)
{	
	FILE		*file;
	LoadData	*data;
	ModelInfo	 info;

	// Open file

	file = fopen(filename, "r");
	if (file == NULL) { 
		printf("Cannot open %s\n", filename);
		exit(1);
	}

	// Setup the LoadData and ModelInfo structure

	data = new LoadData;
    data->num_node	= 0;
    data->num_face	= 0;

	info.nNormals	= 0;
	info.nTexcoords = 0;
	info.nTangents	= 0;
	
	// Make a first pass through the file to count the number of vertices,
	// normals, texcoordinates and faces.

	FirstPass(info, file, data);

	// Allocate memory

	data->node	= new LDNode[data->num_node];
	data->face	= new LDFace[data->num_face];
	
	// Rewind the file and make a second pass through the file to read the data

	rewind(file);
	SecondPass(info, file, data);

	// Close file

	fclose(file);

	// Calculate normals if necessary. Obj files exported from 3ds max doesn't
	// necessarily have normalized normals, therefore predefined normals are
	// normalized.

	if (info.nNormals == 0)
	{
		CalculateNormals(data);
	}
	else
	{
		NormalizeNormals(data);
	}

	// If texture coordinates are not present assign 0.0, 0.0

	if (info.nTexcoords == 0)
	{
		ResetTextureCoordinates(data);
	}

	// Calculate tangent vectors

	if (info.nTangents == 0)
	{
		CalculateTangents(data);
	}

	// Find max and min coordinates

	UpdateBBox(data);

	return data;
}

void NormalizeNormals(LoadData *data)
{
	double length;

	// Normalize the existing vertex normals

	for (int i=0; i<data->num_node; i++)
	{
		length = sqrt(N(i).normal[0]*N(i).normal[0] + N(i).normal[1]*N(i).normal[1] + N(i).normal[2]*N(i).normal[2]);

		N(i).normal[0] = N(i).normal[0] / length;
		N(i).normal[1] = N(i).normal[1] / length;
		N(i).normal[2] = N(i).normal[2] / length;
	}
}

void ResetTextureCoordinates(LoadData *data)
{
	for (int i=0; i<data->num_node; i++) 
	{
		data->node[i].texcoords[0] = 0.0;
		data->node[i].texcoords[1] = 0.0;
	}
}

void SecondPass(ModelInfo &info, FILE *file, LoadData *data)
{
	int		 curNode;
	int		 curNormal;
	int		 curTexcoord;
	int		 curFace;
	double	*normals;
	double	*texcoords;

	unsigned int	v, n, t;
	char			buf[128];

	// Setup temporary arrays to hold the normals and texcoords. Keep in mind
	// that we are using 0 indexing, the obj files are using 1 indexing.

	normals		= new double[3 * info.nNormals];
	texcoords	= new double[2 * info.nTexcoords];

	// On the second pass read all data into the allocated arrays

	curNode		= 0;
	curNormal	= 0;
	curTexcoord	= 0;
	curFace		= 0;
	
	while (fscanf(file, "%s", buf) != EOF)
	{
		switch (buf[0])
		{
		case '#': // Comment
			fgets(buf, sizeof(buf), file);
			break;
		case 'v': // v, vn or vt
			switch(buf[1])
			{
            case '\0': // Vertex
				fscanf(file, "%lf %lf %lf", &N(curNode).pos[0],
											&N(curNode).pos[1],
											&N(curNode).pos[2]);
				curNode++;
                break;
            case 'n': // Normal
                fscanf(file, "%lf %lf %lf", &normals[3*curNormal + 0],
											&normals[3*curNormal + 1],
											&normals[3*curNormal + 2]);
				curNormal++;
                break;
            case 't': // Texcoord
                fscanf(file, "%lf %lf", &texcoords[2*curTexcoord + 0],
										&texcoords[2*curTexcoord + 1]);
                curTexcoord++;
                break;
            default:
                printf("FirstPass: Unknown token \"%s\".\n", buf);
                exit(1);
                break;
            }
			break;
		case 'g': // Group
			fgets(buf, sizeof(buf), file);
			// TODO: Add support for groups
			break;
		case 'f': // Face
			v = n = t = 0;
            fscanf(file, "%s", buf);

			// The following face formats are supported v, v//n, v/t and v/t/n
			// Corresponding to input strings %d, %d//%d, %d/%d, and %d/%d/%d
            
            if (strstr(buf, "//")) // v//n
            {    
                sscanf(buf, "%d//%d", &v, &n);
				F(curFace).node[0]	= v-1;
				N(v-1).normal[0]	= normals[3 * (n-1) + 0];
				N(v-1).normal[1]	= normals[3 * (n-1) + 1];
				N(v-1).normal[2]	= normals[3 * (n-1) + 2];
				fscanf(file, "%d//%d", &v, &n);
				F(curFace).node[1]	= v-1;
				N(v-1).normal[0]	= normals[3 * (n-1) + 0];
				N(v-1).normal[1]	= normals[3 * (n-1) + 1];
				N(v-1).normal[2]	= normals[3 * (n-1) + 2];
                fscanf(file, "%d//%d", &v, &n);
				F(curFace).node[2]	= v-1;
				N(v-1).normal[0]	= normals[3 * (n-1) + 0];
				N(v-1).normal[1]	= normals[3 * (n-1) + 1];
				N(v-1).normal[2]	= normals[3 * (n-1) + 2];
                curFace++;
                while(fscanf(file, "%d//%d", &v, &n) > 0)
				{
					// TODO: Add support for incremental face definition
                }
            }

			else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) // v/t/n
			{
				F(curFace).node[0]	= v-1;
				N(v-1).texcoords[0]	= texcoords[2 * (t-1) + 0];
				N(v-1).texcoords[1]	= texcoords[2 * (t-1) + 1];
				N(v-1).normal[0]	= normals[3 * (n-1) + 0];
				N(v-1).normal[1]	= normals[3 * (n-1) + 1];
				N(v-1).normal[2]	= normals[3 * (n-1) + 2];
				fscanf(file, "%d/%d/%d", &v, &t, &n);
				F(curFace).node[1]	= v-1;
				N(v-1).texcoords[0]	= texcoords[2 * (t-1) + 0];
				N(v-1).texcoords[1]	= texcoords[2 * (t-1) + 1];
				N(v-1).normal[0]	= normals[3 * (n-1) + 0];
				N(v-1).normal[1]	= normals[3 * (n-1) + 1];
				N(v-1).normal[2]	= normals[3 * (n-1) + 2];
                fscanf(file, "%d/%d/%d", &v, &t, &n);
				F(curFace).node[2]	= v-1;
				N(v-1).texcoords[0]	= texcoords[2 * (t-1) + 0];
				N(v-1).texcoords[1]	= texcoords[2 * (t-1) + 1];
				N(v-1).normal[0]	= normals[3 * (n-1) + 0];
				N(v-1).normal[1]	= normals[3 * (n-1) + 1];
				N(v-1).normal[2]	= normals[3 * (n-1) + 2];
                curFace++;
				while(fscanf(file, "%d/%d/%d", &v, &t, &n) == 3)
				{
					// TODO: Add support for incremental face definition
                }
            }
			else if (sscanf(buf, "%d/%d", &v, &t) == 2) // v/t
			{
				F(curFace).node[0]	= v-1;
				N(v-1).texcoords[0]	= texcoords[2 * (t-1) + 0];
				N(v-1).texcoords[1]	= texcoords[2 * (t-1) + 1];
				fscanf(file, "%d/%d", &v, &t);
				F(curFace).node[1]	= v-1;
				N(v-1).texcoords[0]	= texcoords[2 * (t-1) + 0];
				N(v-1).texcoords[1]	= texcoords[2 * (t-1) + 1];
				fscanf(file, "%d/%d", &v, &t);
				F(curFace).node[2]	= v-1;
				N(v-1).texcoords[0]	= texcoords[2 * (t-1) + 0];
				N(v-1).texcoords[1]	= texcoords[2 * (t-1) + 1];
				curFace++;
				while (fscanf(file, "%d/%d", &v, &t) > 0)
				{
					// TODO: Add support for incremental face definition
				}
            }
			else // v
			{
				sscanf(buf, "%d", &v);
				F(curFace).node[0] = v-1;
                fscanf(file, "%d", &v);
				F(curFace).node[1] = v-1;
                fscanf(file, "%d", &v);
				F(curFace).node[2] = v-1;
                curFace++;
                while(fscanf(file, "%d", &v) > 0)
				{
                    // TODO: Add support for incremental face definition
                }
            }
			break;
		default:
			fgets(buf, sizeof(buf), file);
            break;
		}
	}

	// Clean up memory

	delete [] normals;
	delete [] texcoords;
}

void UpdateBBox(LoadData *data)
{
	// Find max and min coordinates

	data->maxx = data->maxy = data->maxz = -DBL_MAX;
	data->minx = data->miny = data->minz =  DBL_MAX;

	for (int i=0; i<data->num_node; i++)
	{
		if (data->node[i].pos[0] < data->minx) { data->minx = data->node[i].pos[0]; }
		if (data->node[i].pos[0] > data->maxx) { data->maxx = data->node[i].pos[0]; }
		if (data->node[i].pos[1] < data->miny) { data->miny = data->node[i].pos[1]; }
		if (data->node[i].pos[1] > data->maxy) { data->maxy = data->node[i].pos[1]; }
		if (data->node[i].pos[2] < data->minz) { data->minz = data->node[i].pos[2]; }
		if (data->node[i].pos[2] > data->maxz) { data->maxz = data->node[i].pos[2]; }
	}
}