/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Geometry Definitions (Part of Core GiPSi API) (GiPSiGeometry.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	GiPSiGeometry.h v0.1.0
////
////    Part of Core GiPSi API
////
////	1)	Defines various base classes:
////			Point		-	Base class for vertices, nodes etc.
////			Element		-	Base class for geomtry primitives, FEM elements etc.
////			Geometry	-	Base class for surface and volume geometries
////
////	2)	Defines simple geometry primitives
////			Vertex		-	Simple Vertex
////			Triangle	-	Simple Triangle
////			Tetrahedra	-	Simple Tetrahedra
////
////	3)	Defines simple triangular and tetrahedral geometries
////			TriSurface	-	Triangular surface geometry
////			TetVolume	-	Tetrahedral volume geometry
////
////	4)	Defines base classes for boudary and domain
////
////////////////////////////////////////////////////////////////

#ifndef _GiPSiGEOMETRY_H
#define _GiPSiGEOMETRY_H

#include "algebra.h"
#include "errors.h"
#include "load_mesh.h"




/****************************************************************
 *						BASE GEOMETRY							*  
 ****************************************************************/

// Base geometry class
class Geometry {
public:
	Vector<Real>			color;			// Global color
	Real					minx, maxx, miny, maxy, minz, maxz;

	// Constructors
	Geometry() : color(4, "1.0 0.0 0.0 1.0"){}

	// Display manager for the geometry
	virtual void		SetupDisplay(void) {}
    virtual void		Display(void) {}
	// Loader
	virtual void		Load(char*) {}
	virtual void		Load(LoadData*) {}
	// Basic transformations
	virtual void		Translate(float tx, float ty, float tz) {}
    virtual void		Rotate(Real angle, Real ax, Real ay, Real az) {}
	virtual void		Scale(float sx, float sy, float sz) {}

protected:
	
};



/****************************************************************
 *					BASE PRIMITIVE CLASSES						*  
 ****************************************************************/
class	Point {
public:
    unsigned int			refid;		// Reference id
    Vector<Real>			pos;		// Position

	Point() : pos(3) {};
};


// Base Element Class
class	Element {
public:
	unsigned int			refid;		// Reference id

	Element() {}
};


/****************************************************************
 *					SIMPLE GEOMETRY PRIMITIVES					*  
 ****************************************************************/
// Simple Vertex Class	
class Vertex : public Point {
public:
    Vector<Real>			n;			// Normal
    int						valence;	// Degree
	Vector<Real>			color;		// Color
	Vector<Real>			texcoord;	// Texture coordinates

	// Constructors
	Vertex()	:	valence(0), 
					n(3, "1.0 0.0 0.0"), color(4, "1.0 0.0 0.0 1.0"), texcoord(2, "0.0 0.0") {}
	
	void	init(	unsigned int	refid,
					Real			*pos,
					Real			*n,
					Real			*color,
					Real			*texcoord,
					Real			*tangent) {
		this->refid = refid;
		this->pos	= pos;
		// The Vertex class is initialized by value using the copy constructor of the Vector<> class
		// The color is used to store the tangent in lack of a better options
		if(n != NULL)			this->n			= n;
		if(color != NULL)		this->color		= color;
		if(texcoord != NULL)	this->texcoord	= texcoord;
		if(tangent != NULL)
		{
			// Clamp to 0..1 range
			this->color[0] = tangent[0] * 0.5 + 0.5;
			this->color[1] = tangent[1] * 0.5 + 0.5;
			this->color[2] = tangent[2] * 0.5 + 0.5;
		}
	}

};


// Simple Triangle Class
class Triangle : public Element {
public:
	Vector<Real>			n;
    Vertex*					vertex[3];

	// Constructors
	Triangle(): n(3) {}

	void	init(	unsigned int	refid,
					Real			*n,
					Vertex			**vertex) {

		this->refid	= refid;
		this->vertex[0] = vertex[0];
		this->vertex[1] = vertex[1];
		this->vertex[2] = vertex[2];
		if(n != NULL)		this->n		= n;

	}
};


// Simple Tetrahedra Class
class Tetrahedra : public Element{
public:
    Vertex*					vertex[4];

	// Constructors
	Tetrahedra() {}

	void	init(	unsigned int	refid,
					Vertex			**vertex) {

		this->refid	= refid;
		this->vertex[0] = vertex[0];
		this->vertex[1] = vertex[1];
		this->vertex[2] = vertex[2];
		this->vertex[3] = vertex[3];
	}
};


/****************************************************************
 *						POINT CLOUD GEOMETRY					*  
 ****************************************************************/
class PointCloud : public Geometry {
public:
    Vertex				*vertex;				// Vertex array
	unsigned int		num_vertex;				// Number of vertices

	// Constructors
	PointCloud(float r = 0.5, float g = 0.5, float b = 0.5, float a = 1.0) {
		color[0] = r;	color[1] = g;	color[2] = b;	color[3] = a;
	}

	void			Load(char*){};
	void			Translate(float tx, float ty, float tz);
    void			Rotate(Real angle, Real ax, Real ay, Real az);
	void			Scale(float sx, float sy, float sz);
};


/****************************************************************
 *					TRIANGULAR SURFACE GEOMETRY					*  
 ****************************************************************/

// Base class for the simple polygonal geometry class
class TriSurface : public PointCloud {
public:
    Triangle			*face;					// Face array
	unsigned int		num_face;				// Number of faces

	// Constructors
	TriSurface(float r = 0.5, float g = 0.5, float b = 0.5, float a = 1.0) {
		color[0] = r;	color[1] = g;	color[2] = b;	color[3] = a;
	}


	void init(unsigned int num_vertex, unsigned int num_face)
	{
		// Allocate geometry memory
		vertex = new Vertex[num_vertex];
		if(vertex == NULL) {
			error_exit(-1, "Cannot allocate memory for vertices!\n");
		}

		face = new Triangle[num_face];
		if(face == NULL) {
			error_exit(-1, "Cannot allocate memory for faces!\n");
		}

		this->num_vertex	= num_vertex;
		this->num_face		= num_face;
	}

	void			Load(char*);
	void			Load(LoadData*);
	void			calcNormals(void);			// Updates normals
};



/****************************************************************
 *					TETRAHEDRAL VOLUME GEOMETRY					*  
 ****************************************************************/
class TetVolume : public PointCloud {
public:
    Triangle			*face;					// Face array
	unsigned int		num_face;				// Number of faces
	Tetrahedra			*tet;					// Tetrahedra Array
	unsigned int		num_tet;				// Number of tetrahedra

	// Constructors
	TetVolume(float r = 0.5, float g = 0.5, float b = 0.5, float a = 1.0) {
		color[0] = r;	color[1] = g;	color[2] = b;	color[3] = a;
	}


	void	init(	unsigned int	num_vertex, unsigned int	num_face, unsigned int num_tet) {

		// Allocate geometry memory
		vertex = new Vertex[num_vertex];
		if(vertex == NULL) {
			error_exit(-1, "Cannot allocate memory for vertices!\n");
		}
		face = new Triangle[num_face];
		if(face == NULL) {
			error_exit(-1, "Cannot allocate memory for faces!\n");
		}
		tet = new Tetrahedra[num_tet];
		if(tet == NULL) {
			error_exit(-1, "Cannot allocate memory for tets!\n");
		}

		this->num_vertex	= num_vertex;
		this->num_face		= num_face;
		this->num_tet		= num_tet;  
	}

	void			Load(char*);
	void			Load(LoadData*);
	void			calcNormals(void);		// Updates normals
};


/****************************************************************
 *						BASE BOUNDARY							*  
 ****************************************************************/
class Boundary : public TriSurface {
public:

};

/****************************************************************
 *						BASE DOMAIN								*  
 ****************************************************************/
class Domain : public TetVolume {
public:

};

int				isInElement( Vector<Real>	&x,	Tetrahedra	&e);
Tetrahedra*		findElement(TetVolume &mesh, Vector<Real> &x);

#endif
