/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSiGeometry Implementation (GiPSiGeometry.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu, Suriya Natsupakpong
*/

////	GiPSiGeometry.cpp v1.0
////
////	Implements TriSurface and TetVolume
////
////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>

#include "GiPSiGeometry.h"
#include "algebra.h"
#include "errors.h"
#include "load_mesh.h"

#define		DELIM 10


/**
 * Test if a point is inside a Triangle
 * 
 * @param x Point coordinates.
 * @param e Triangle boundaries for test.
 */
int	isInElement( Vector<Real>	&x,	Triangle	&e) {
	Vector<Real>	&p0 = e.vertex[0]->pos, 
					&p1 = e.vertex[1]->pos,
					&p2 = e.vertex[2]->pos;

	if(	(TriangleArea(x, p0, p1) >= 0.0) && 
		(TriangleArea(x, p1, p2) >= 0.0) && 
		(TriangleArea(x, p2, p0) >= 0.0)) {
			return 1;
	}

	if(	(TriangleArea(x, p0, p1) < 0.0) && 
		(TriangleArea(x, p1, p2) < 0.0) && 
		(TriangleArea(x, p2, p0) < 0.0)) {
			return 1;
	}

	return 0;
}


/**
 * Test if a point is inside a Tetrahedra
 * 
 * @param x Point coordinates.
 * @param e Tetrahedra boundaries for test.
 */
int	isInElement( Vector<Real>	&x,	Tetrahedra	&e) {
	Vector<Real>	&p0 = e.vertex[0]->pos, 
					&p1 = e.vertex[1]->pos,
					&p2 = e.vertex[2]->pos,
					&p3 = e.vertex[3]->pos;

	if(	(TetrahedraVolume(x, p0, p1, p2) >= 0.0) && 
		(TetrahedraVolume(x, p2, p3, p0) >= 0.0) && 
		(TetrahedraVolume(x, p3, p2, p1) >= 0.0) &&
		(TetrahedraVolume(x, p1, p0, p3) >= 0.0)) {
			return 1;
	}

	return 0;
}


/**
 * Finds the tetrahedra that contains point x
 * 
 * @param mesh Mesh containing list of tetrahedra.
 * @param x Point to test for.
 */
Tetrahedra* findElement(TetVolume &mesh, Vector<Real> &x)
{
	unsigned int	i;
	Tetrahedra*		found = NULL;
	Tetrahedra*		element;

	for(i=0; i<mesh.num_tet; i++) {
		element = &(mesh.tet[i]);

		if(isInElement(x, *element)) {
			found = element;
			break;
		}
	}

	return found;
}



/**
 * Reads in .obj file.
 * 
 * @param basename Name of file to read.
 */
void PointCloud::Load(char *basename)   
{
	char		*ext;
	LoadData	*data;

	ext=strrchr(basename, '.');
	if (ext==NULL)
	{
		error_exit(-1, "Unknown triangle surface file type.\n");
	}

	if (!strcmp(ext, ".obj"))
	{
		// Load obj file format
		data = LoadObj(basename);
	}
	else if (!strcmp(ext, ".3ds"))
	{
		// Load 3ds file format
		// TODO: Add 3ds loader
		error_exit(-1, "3ds loader not implemented\n");
	}
	else 
	{
		error_exit(-1, "Unknown triangle surface file type.\n");
	}

	Load(data);
	
	// Clean up memory

	delete [] data->node;
	delete [] data->face;
	delete data;
	data = NULL;
}



/**
 * Reads in .obj file data.
 * 
 * @param data Verticies of object.
 */
void PointCloud::Load(LoadData *data)
{
	unsigned int	i;

	init(data->num_node);
  
	/****************************************/
	/*	Create vertices						*/
	/****************************************/
	for(i=0; i<num_vertex; i++) {
		vertex[i].init(i, data->node[i].pos, data->node[i].normal, color.begin(), NULL, NULL);		
	}

	// NOTE: The face structure is filled out correctly

	minx = data->minx;
	maxx = data->maxx;
	miny = data->miny;
	maxy = data->maxy;
	minz = data->minz;
	maxz = data->maxz;
}



/**
 * Translate the object by the indicated amount.
 * 
 * @param tx Change in x position.
 * @param ty Change in y position.
 * @param tz Change in z position.
 */
void PointCloud::Translate(float tx, float ty, float tz)
{
	unsigned int	i;
	Vertex			*v;

	for(i = 0; i < num_vertex; i++) {
		v = &(vertex[i]);
		v->pos[0] += tx;
		v->pos[1] += ty;
		v->pos[2] += tz;
	}
}



/**
 * Translate the object by the indicated amount.
 * 
 * @param p displacement vector
 */
void PointCloud::Translate(Vector<Real> p)
{
	ASSERT( (p.dim()==3) );
	unsigned int	i;
	Vertex			*v;

	for(i = 0; i < num_vertex; i++) {
		v = &(vertex[i]);
		v->pos += p;
	}
}



/**
 * Scale the object by the indicated amount.
 * 
 * @param sx X scaling.
 * @param sy Y scaling.
 * @param sz Z scaling.
 */
void PointCloud::Scale(float sx, float sy, float sz)
{
	unsigned int	i;
	Vertex			*v;

	for(i = 0; i < num_vertex; i++) {
		v = &(vertex[i]);
		v->pos[0] *= sx;
		v->pos[1] *= sy;
		v->pos[2] *= sz;
	}
}



/**
 * Scale the object by the indicated amount.
 * 
 * @param s scale vector
 */
void PointCloud::Scale(Vector<Real> s)
{
	ASSERT( (s.dim()==3) );
	unsigned int	i;
	Vertex			*v;

	for(i = 0; i < num_vertex; i++) {
		v = &(vertex[i]);
		v->pos[0] *= s[0];
		v->pos[1] *= s[1];
		v->pos[2] *= s[2];
	}
}

/**
 * Rotate the object around a given axis.
 * 
 * @param angle Angle to rotate by.
 * @param ax Axis x coord.
 * @param ay Axis y coord.
 * @param az Axis z coord.
 */
void PointCloud::Rotate(Real angle, Real ax, Real ay, Real az)
{
	unsigned int	i;
	Vertex			*v;
	Matrix<Real>	R(3, 3);
	Real			s	= (Real) cos(angle/2.0);
	Real			sp	= (Real) sin(angle/2.0);
	Real			a	= ax*sp;
	Real			b	= ay*sp;
	Real			c	= az*sp;
	Real			l	= (Real) sqrt(a*a + b*b + c*c + s*s);

	a /= l;
	b /= l;
	c /= l;
	s /= l;

	R[0][0] = 1-2*b*b-2*c*c;
	R[0][1] = 2*a*b-2*s*c;
	R[0][2] = 2*a*c+2*s*b;
	R[1][0] = 2*a*b+2*s*c;
	R[1][1] = 1-2*a*a-2*c*c;
	R[1][2] = 2*b*c-2*s*a;
	R[2][0] = 2*a*c-2*s*b;
	R[2][1] = 2*b*c+2*s*a;
	R[2][2] = 1-2*a*a-2*b*b;

	for(i = 0; i < num_vertex; i++) {
		v = &(vertex[i]);
		v->pos = R * v->pos;
	}
}

/**
 * Rotate the object using a given rotation matrix.
 * 
 * @param R rotation matrix to be used in the transformation
 *
 */
void PointCloud::Rotate(Matrix<Real> R)
{
	ASSERT( (R.m()==3) && (R.n()==3) );
	unsigned int	i;
	Vertex			*v;
	for(i = 0; i < num_vertex; i++) {
		v = &(vertex[i]);
		v->pos = R * v->pos;
	}
}

/**
 * Reads in .obj file.
 * 
 * @param basename Name of file to read.
 */
void VectorField::Load(char *basename)   
{
	char		*ext;
	LoadData	*data;

	ext=strrchr(basename, '.');
	if (ext==NULL)
	{
		error_exit(-1, "Unknown triangle surface file type.\n");
	}

	if (!strcmp(ext, ".obj"))
	{
		// Load obj file format
		data = LoadObj(basename);
	}
	else if (!strcmp(ext, ".3ds"))
	{
		// Load 3ds file format
		// TODO: Add 3ds loader
		error_exit(-1, "3ds loader not implemented\n");
	}
	else 
	{
		error_exit(-1, "Unknown triangle surface file type.\n");
	}

	Load(data);
	
	// Clean up memory

	delete [] data->node;
	delete [] data->face;
	delete data;
	data = NULL;
}



/**
 * Reads in .obj file data.
 * 
 * @param data Verticies of object.
 */
void VectorField::Load(LoadData *data)
{
	unsigned int	i;
	Real zero_vector[3] = {0.0, 0.0, 0.0};

	init(data->num_node);
  
	/****************************************/
	/*	Create vertices						*/
	/****************************************/
	for(i=0; i<num_vertex; i++) {
		vertex[i].init(i, data->node[i].pos, data->node[i].normal, color.begin(), NULL, NULL);
		field_vector[i].init(i, zero_vector, zero_vector, color.begin(), NULL, NULL);		
	}

	// NOTE: The face structure is filled out correctly

	minx = data->minx;
	maxx = data->maxx;
	miny = data->miny;
	maxy = data->maxy;
	minz = data->minz;
	maxz = data->maxz;
}



/**
 * Translate the object by the indicated amount.
 * 
 * @param tx Change in x position.
 * @param ty Change in y position.
 * @param tz Change in z position.
 */
void VectorField::Translate(float tx, float ty, float tz)
{
	PointCloud::Translate(tx,ty,tz);
}


/**
 * Translate the object by the indicated amount.
 * 
 * @param p displacement vector
 */
void VectorField::Translate(Vector<Real> p)
{
	PointCloud::Translate(p);
}


/**
 * Scale the object by the indicated amount.
 * 
 * @param sx X scaling.
 * @param sy Y scaling.
 * @param sz Z scaling.
 */
void VectorField::Scale(float sx, float sy, float sz)
{
	PointCloud::Scale(sx,sy,sz);
}


/**
 * Scale the object by the indicated amount.
 * 
 * @param s scale vector
 */
void VectorField::Scale(Vector<Real> s)
{
	PointCloud::Scale(s);
}


/**
 * Rotate the object around a given axis.
 * 
 * @param angle Angle to rotate by.
 * @param ax Axis x coord.
 * @param ay Axis y coord.
 * @param az Axis z coord.
 */
void VectorField::Rotate(Real angle, Real ax, Real ay, Real az)
{
	// First rotate the underlying point cloud
	PointCloud::Rotate(angle, ax, ay, az);

	// Then rotate the vectors
	unsigned int	i;
	Vertex			*v;
	Matrix<Real>	R(3, 3);
	Real			s	= (Real) cos(angle/2.0);
	Real			sp	= (Real) sin(angle/2.0);
	Real			a	= ax*sp;
	Real			b	= ay*sp;
	Real			c	= az*sp;
	Real			l	= (Real) sqrt(a*a + b*b + c*c + s*s);

	a /= l;
	b /= l;
	c /= l;
	s /= l;

	R[0][0] = 1-2*b*b-2*c*c;
	R[0][1] = 2*a*b-2*s*c;
	R[0][2] = 2*a*c+2*s*b;
	R[1][0] = 2*a*b+2*s*c;
	R[1][1] = 1-2*a*a-2*c*c;
	R[1][2] = 2*b*c-2*s*a;
	R[2][0] = 2*a*c-2*s*b;
	R[2][1] = 2*b*c+2*s*a;
	R[2][2] = 1-2*a*a-2*b*b;

	for(i = 0; i < num_vertex; i++) {
		v = &(field_vector[i]);
		v->pos = R * v->pos;
	}	
}

/**
 * Rotate the object using a given rotation matrix.
 * 
 * @param R rotation matrix to be used in the transformation
 *
 */
void VectorField::Rotate(Matrix<Real> R)
{
	// First rotate the underlying point cloud
	PointCloud::Rotate(R);

	// Then rotate the vectors
	unsigned int	i;
	Vertex			*v;
	for(i = 0; i < num_vertex; i++) {
		v = &(field_vector[i]);
		v->pos = R * v->pos;
	}	
}





/**
 * Calculates the face and vertex normals of the object
 */
void TriSurface::calcNormals(void)
{
	Vertex					*v0, *v1, *v2;
	static Vector<Real>		a(3), b(3), c(3);	
	Triangle				*f;
	unsigned int			i;

	// Find normals of faces
	for(i = 0; i < num_face; i++) {
		f = &(face[i]);
		v0 = f->vertex[0];
		v1 = f->vertex[1];
		v2 = f->vertex[2];
   
		subVV(a, v0->pos, v1->pos);		// a = v0 - v1
		subVV(b, v1->pos, v2->pos);		// b = v1 - v2
   
		crossVV(c, a, b);				// c = a x b
		c.normalize();
   
		f->n = c;

		// Add each face normal to vertices
		v0->n += f->n;
		v1->n += f->n;
		v2->n += f->n;
	}

	// Normalize the vertex normals
	for(i = 0; i < num_vertex; i++) {
		vertex[i].n.normalize();
	} 
}



/**
 * Reads in .obj file.
 * 
 * @param basename Name of file to read.
 */
void TriSurface::Load(char *basename)   
{
	char		*ext;
	LoadData	*data;

	ext=strrchr(basename, '.');
	if (ext==NULL)
	{
		error_exit(-1, "Unknown triangle surface file type.\n");
	}

	if (!strcmp(ext, ".obj"))
	{
		// Load obj file format
		data = LoadObj(basename);
	}
	else if (!strcmp(ext, ".3ds"))
	{
		// Load 3ds file format
		// TODO: Add 3ds loader
		error_exit(-1, "3ds loader not implemented\n");
	}
	else 
	{
		error_exit(-1, "Unknown triangle surface file type.\n");
	}

	Load(data);
	
	// Clean up memory

	delete [] data->node;
	delete [] data->face;
	delete data;
	data = NULL;
}



/**
 * Reads in .obj file data.
 * 
 * @param data Verticies of object.
 */
void TriSurface::Load(LoadData *data)
{
	unsigned int	i;

	init(data->num_node, data->num_face);
  
	/****************************************/
	/*	Create vertices						*/
	/****************************************/
	for(i=0; i<num_vertex; i++) {
		vertex[i].init(i, data->node[i].pos, data->node[i].normal, color.begin(), data->node[i].texcoords, data->node[i].tangent);		
	}

	// NOTE: The vertex structure is filled out correctly
  
	/****************************************/
	/*	Create faces						*/
	/****************************************/
	Vertex	*vertices[3];
	for(i=0; i<num_face; i++) {
		vertices[0] = &(vertex[data->face[i].node[0]]);
		vertices[1] = &(vertex[data->face[i].node[1]]);
		vertices[2] = &(vertex[data->face[i].node[2]]);

		face[i].init(i, NULL, vertices);
	}

	// NOTE: The face structure is filled out correctly

	minx = data->minx;
	maxx = data->maxx;
	miny = data->miny;
	maxy = data->maxy;
	minz = data->minz;
	maxz = data->maxz;
}


/**
 * Calculates the face and vertex normals of the object
 */
void TetVolume::calcNormals(void)
{
	Vertex					*v0, *v1, *v2;
	static Vector<Real>		a(3), b(3), c(3);	
	Triangle				*f;
	unsigned int			i;

	// Find normals of faces
	for(i = 0; i < num_face; i++) {
		f = &(face[i]);
		v0 = f->vertex[0];
		v1 = f->vertex[1];
		v2 = f->vertex[2];
   
		subVV(a, v0->pos, v1->pos);		// a = v0 - v1
		subVV(b, v1->pos, v2->pos);		// b = v1 - v2
   
		crossVV(c, a, b);				// c = a x b
		c.normalize();
   
		f->n = c;

		// Add each face normal to vertices
		v0->n += f->n;
		v1->n += f->n;
		v2->n += f->n;
	}

	// Normalize the vertex normals
	for(i = 0; i < num_vertex; i++) {
		vertex[i].n.normalize();
	} 
}



/**
 * Reads in .obj file.
 * 
 * @param basename Name of file to read.
 */
void TetVolume::Load(char *basename)   
{
	char		*ext, *name;
	LoadData	*data;

	ext=strrchr(basename,'.');
	if (ext==NULL) {
		printf("Unknown tetrahedra geometry file type.\n");
		exit(1);
	}
	if (!strcmp(ext,".node")) {
		// Load Pyramid/Triangle file format
		name = (char *) malloc(sizeof(char)*(strlen(basename)+1));
		sprintf(name,"%s", basename);
		name[ext-basename]=0x00;		
		
		data = LoadNode(name);

		free(name);
	}
	else if (!strcmp(ext,".neu")) {
		// Load NetGen Neutral file format
        data = LoadNeutral(basename);
	}
	else 
	{
		printf("Unknown tetrahedra geometry file type.\n");
		exit(1);
	}

	Load(data);
	
	delete[] data;

}



/**
 * Reads in .obj file data.
 * 
 * @param data Verticies of object.
 */
void TetVolume::Load(LoadData *data)   
{
	unsigned int	i;

	init(data->num_node, data->num_face, data->num_element);
  
	/****************************************/
	/*	Create vertices						*/
	/****************************************/
	for(i=0; i<num_vertex; i++) {
		vertex[i].init(i, data->node[i].pos, NULL, color.begin(), NULL, NULL);
	}
  
	/****************************************/
	/*	Create faces						*/
	/****************************************/
	Vertex	*vertices[4];
	for(i=0; i<num_face; i++) {
		vertices[0] = &(vertex[data->face[i].node[0]]);
		vertices[1] = &(vertex[data->face[i].node[1]]);
		vertices[2] = &(vertex[data->face[i].node[2]]);

		face[i].init(i, NULL, vertices);
	}

	/****************************************/
	/*	Create tets							*/
	/****************************************/
	for(i=0; i<num_tet; i++) {
		vertices[0] = &(vertex[data->element[i].node[0]]);
		vertices[1] = &(vertex[data->element[i].node[1]]);
		vertices[2] = &(vertex[data->element[i].node[2]]);
		vertices[3] = &(vertex[data->element[i].node[3]]);

		vertices[0]->valence++;
		vertices[1]->valence++;
		vertices[2]->valence++;
		vertices[3]->valence++;

		tet[i].init(i, vertices);
	}

	minx = data->minx;
	maxx = data->maxx;
	miny = data->miny;
	maxy = data->maxy;
	minz = data->minz;
	maxz = data->maxz;
}


/**
 * Loads the input .TGA file data and dimensions into the TextureGeometry’s data members.
 * 
 * @param fileName Name of file to read.
 */
void TextureGeometry::Load(char * fileName)
{
	// Load image from tga file

	TGA * TGAImage	= new TGA(fileName);

	// Use to dimensions of the image as the texture dimensions

	this->width		= TGAImage->GetWidth();
	this->height	= TGAImage->GetHeigth();
	int dataSize	= this->width * this->height * (TGAImage->GetPixelDepth()/8);
	this->data		= new byte[dataSize];
	// Copy texture pixel data into our data member
	for (int i = 0; i < dataSize; i++)
	{
		this->data[i] = TGAImage->GetPixels()[i];
	}

	delete TGAImage;
}