/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Dispilat API Definitions (Part of Core GiPSi API) (GiPSiDisplay.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	GiPSiDisplay.h v1.0
////
////    Part of Core GiPSi API
////
////    1.) Defines data structures used in display managers
////
////	2.) Defines DisplayManager base class
////
////    3.) Defines display managers TriSurfaceDisplayManager and
////         TetVolumeDisplayManager
////
////////////////////////////////////////////////////////////////


#ifndef _GiPSiDISPLAY_H
#define _GiPSiDISPLAY_H

#include <pthread.h>
#include "GiPSiGeometry.h"

/****************************************************************
 *				DATA STRUCTURES FOR DISPLAY MANAGERS			*  
 ****************************************************************/

enum	DisplayObjectType  {	GIPSI_DRAW_POINT, GIPSI_DRAW_LINE, GIPSI_DRAW_TRIANGLE, 
								GIPSI_DRAW_QUAD, GIPSI_DRAW_POLYGON, GIPSI_DRAW_CUSTOM };

enum	DisplayPolygonMode  {	GIPSI_POLYGON_OUTLINE, GIPSI_POLYGON_FILL };

enum	DisplayShadeMode  {	GIPSI_SHADE_FLAT, GIPSI_SHADE_SMOOTH };

struct	DisplayDataType {
	unsigned char	color:2;		// 0: none, 1: RGB, 2: RGBA, 3: not used
	unsigned char	normal:1;		
	unsigned char	texcoord:1;
	unsigned char	:4;
};


#define	IS_RGB_ON(x)		(x & 0x01)
#define IS_RGBA_ON(x)		(x & 0x02)
#define IS_NORMAL_ON(x)		(x & 0x04)
#define IS_TEXCOORD_ON(x)	(x & 0x08)


struct DisplayHeader {
	DisplayObjectType				objType;
	DisplayPolygonMode				polyMode;
	DisplayShadeMode				shadeMode;
	char							dataType;
	float							pointSize;
	float							lineSize;
};


struct DisplayArray {
	DisplayHeader					header;
	int								DISPARRAY_NODESIZE;
	float							*dispArray;		// Display Array
	int								dA_size;		// Display array size
	unsigned int					*indexArray;	// Index Array
	unsigned int					iA_size;		// Index array size
	pthread_mutex_t					displayMutex;
};


/****************************************************************
 *				DISPLAY MANAGER BASE CLASS						*  
 ****************************************************************/

class DisplayManager {
public:
	DisplayManager(const	DisplayHeader &inheader) {
		display.header.objType		= inheader.objType;
		display.header.polyMode		= inheader.polyMode;
		display.header.shadeMode	= inheader.shadeMode;
		display.header.dataType		= inheader.dataType;
		display.header.pointSize	= inheader.pointSize;
		display.header.lineSize		= inheader.lineSize;
	}

	void	SetDisplayHeader(const	DisplayHeader &inheader) {
		display.header.objType		= inheader.objType;
		display.header.polyMode		= inheader.polyMode;
		display.header.shadeMode	= inheader.shadeMode;
		display.header.dataType		= inheader.dataType;
		display.header.pointSize	= inheader.pointSize;
		display.header.lineSize		= inheader.lineSize;
	}

	void	GetDisplayHeader(DisplayHeader &outheader) {
		outheader.objType		= display.header.objType;
		outheader.polyMode		= display.header.polyMode;
		outheader.shadeMode		= display.header.shadeMode;
		outheader.dataType		= display.header.dataType;
		outheader.pointSize		= display.header.pointSize;
		outheader.lineSize		= display.header.lineSize;
	}

	DisplayArray*	GetDisplay(void) { return &display; };

	virtual	void	Display(void) {}

protected:
	DisplayArray	display;

};

/****************************************************************
 *					TRISURFACE DISPLAY MANAGER					*  
 ****************************************************************/

class TriSurfaceDisplayManager : public DisplayManager {
public:
	TriSurfaceDisplayManager(TriSurface *ingeometry, DisplayHeader &inheader) :
	  geometry(ingeometry), DisplayManager(inheader) {
		display.DISPARRAY_NODESIZE = 3;
		
		char	type = display.header.dataType;

		if(IS_RGB_ON(type))
			display.DISPARRAY_NODESIZE += 3;
		else if(IS_RGBA_ON(type))
			display.DISPARRAY_NODESIZE += 4;

		if(IS_NORMAL_ON(type))
			display.DISPARRAY_NODESIZE += 3;

		if(IS_TEXCOORD_ON(type))
			display.DISPARRAY_NODESIZE += 2;
		
		display.dA_size		= geometry->num_vertex * display.DISPARRAY_NODESIZE;
		display.dispArray	= new float[display.dA_size];
		display.iA_size		= geometry->num_face * 3;
		display.indexArray	= new unsigned int[display.iA_size];
	}

	void	Display(void);

protected:
	TriSurface		*geometry;
};


/****************************************************************
 *					TETVOLUME DISPLAY MANAGER					*  
 ****************************************************************/

class TetVolumeDisplayManager : public DisplayManager {
public:
	TetVolumeDisplayManager(TetVolume *ingeometry, DisplayHeader &inheader) :
	  geometry(ingeometry), DisplayManager(inheader) {
		display.DISPARRAY_NODESIZE = 3;

		char	type = display.header.dataType;

		if(IS_RGB_ON(type))
			display.DISPARRAY_NODESIZE += 3;
		else if(IS_RGBA_ON(type))
			display.DISPARRAY_NODESIZE += 4;

		if(IS_NORMAL_ON(type))
			display.DISPARRAY_NODESIZE += 3;

		if(IS_TEXCOORD_ON(type))
			display.DISPARRAY_NODESIZE += 2;
	  
		display.dA_size		= geometry->num_vertex * display.DISPARRAY_NODESIZE;
		display.dispArray	= new float[display.dA_size];
		if (display.header.objType==GIPSI_DRAW_TRIANGLE)
			display.iA_size		= geometry->num_face * 3;
		else if (display.header.objType==GIPSI_DRAW_LINE)
			display.iA_size		= geometry->num_tet * 12;
		else 
			error_exit(1,"Unknown object type for TetVolume Display Manager");
		display.indexArray	= new unsigned int[display.iA_size];
	}

	void	Display(void);

protected:
	TetVolume		*geometry;

};

#endif