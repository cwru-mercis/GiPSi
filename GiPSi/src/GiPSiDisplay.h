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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <pthread.h>
#include "GiPSiGeometry.h"
#include "XMLNode.h"
#include "XMLNodeList.h"

using namespace GiPSiXMLWrapper;

/****************************************************************
 *				DATA STRUCTURES FOR DISPLAY MANAGERS			*  
 ****************************************************************/

enum	DisplayObjectType  {	GIPSI_DRAW_POINT, GIPSI_DRAW_LINE, GIPSI_DRAW_TRIANGLE, 
								GIPSI_DRAW_QUAD, GIPSI_DRAW_POLYGON, GIPSI_DRAW_CUSTOM };

enum	DisplayPolygonMode  {	GIPSI_POLYGON_OUTLINE, GIPSI_POLYGON_FILL };

enum	DisplayShadeMode  {	GIPSI_SHADE_FLAT, GIPSI_SHADE_SMOOTH };

enum	DisplayTextureType {	GIPSI_NO_TEXTURE, GIPSI_2D_STATIC_CLIENT, GIPSI_2D_STATIC_SERVER,
								GIPSI_2D_DYNAMIC_SERVER, GIPSI_3D_STATIC_CLIENT };


struct	DisplayDataType {
	unsigned char	color:2;		// 0: none, 1: RGB, 2: RGBA, 3: texcoord, 4: tangent, 5: unused
	unsigned char	normal:1;		
	unsigned char	texcoord:1;
	unsigned char	tangent:1;
	unsigned char	:3;
};


#define	IS_RGB_ON(x)		(x & 0x01)
#define IS_RGBA_ON(x)		(x & 0x02)
#define IS_NORMAL_ON(x)		(x & 0x04)
#define IS_TEXCOORD_ON(x)	(x & 0x08)
#define IS_TANGENT_ON(x)	(x & 0x10)
#define HAS_EXTRA_ATTRS(x)	(IS_TANGENT_ON(x))

enum ShaderName;

class ShaderParams {
public:
	virtual ShaderName	GetShaderName() = 0;
	virtual char		GetShaderAttributes() = 0;

protected:
	XMLNode * FindParam(XMLNodeList * params, const char * targetName);
	const char * GetValue(XMLNodeList * params, const char * targetName);
};

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
	int								dA_size;		// Display array size
	unsigned int					iA_size;		// Index array size
	float							*dispArray;		// Display Array
	unsigned int					*indexArray;	// Index Array
	bool							full;
};

class DisplayBufferUnitTest;
class DisplayManager;
class LoaderUnitTest;
class TextureUnitTest;

class DisplayBuffer {
public:
	DisplayBuffer(DisplayManager * manager, const DisplayHeader * inheader, DisplayTextureType textType = GIPSI_NO_TEXTURE);
	~DisplayBuffer();

	DisplayArray * GetReadArray();
	DisplayArray * GetWriteArray(bool visible);
	DisplayArray * GetWriteArray(int DISPARRAY_NODESIZE, int dA_size, int iA_size, bool visible);

	void Enqueue();
	void Dequeue();
	void ConditionalDequeue();

	bool GetDisplayHeader(DisplayHeader * outheader);
	void SetDisplayHeader(const DisplayHeader * inheader);

	ShaderParams * GetShaderParams();
	void SetShaderParams(ShaderParams * newShaderParams);

	const char * GetName(void);
	void SetName(const char * newName);

	const char * GetObjectName();
	void SetObjectName(const char * newObjectName);

	void SetNext(DisplayBuffer * newNext);
	DisplayBuffer * GetNext();

	DisplayTextureType GetTextureType() { return textureType; }
	int GetNumTextures() { return nTextures; }
	const char * GetTexture(int i) { return texture[i]; }

	void AddTexture(char * textName);

protected:
	void DequeueExchange();

	void UpdateDataType();

	// Referenced structs/classes
	DisplayManager		*manager;

	DisplayHeader		*header;

	ShaderParams		*shaderParams;

	DisplayArray		*data;
	int					nArrays;

	// DisplayBuffer name
	char				*name;

	// Object name
	char				*objectName;

	// Texture information
	DisplayTextureType	textureType;
	int					nTextures;
	char				**texture;

	// Mutex
	pthread_mutex_t mutex;

	// Indices
	int					readIndex, writeIndex, queueIndex;

	// Next pointer
	DisplayBuffer		*next;

	friend DisplayBufferUnitTest;
	friend LoaderUnitTest;
	friend TextureUnitTest;
};

/****************************************************************
 *				DISPLAY MANAGER BASE CLASS						*  
 ****************************************************************/

class DisplayManager {
public:
	DisplayManager(const DisplayHeader * inheader, DisplayTextureType textType = GIPSI_NO_TEXTURE)
	:	displayBuffer(this, inheader, textType), visible(true) {
		// We must have a unique name, so we use the local memory address
		// sxn66 change to the name of object
		//char name[11];
		//itoa((int)this, name, 16);
		//displayBuffer.SetName(name);
	}

	void	GetDisplayHeader(DisplayHeader * outheader) {
		displayBuffer.GetDisplayHeader(outheader);
	}

	void	SetDisplayHeader(const DisplayHeader * inheader) {
		displayBuffer.SetDisplayHeader(inheader);
	}

	void	SetShaderParams(ShaderParams * shaderParams) {
		displayBuffer.SetShaderParams(shaderParams);
	}

	DisplayArray*	GetDisplay(void) { return displayBuffer.GetReadArray(); }

	DisplayBuffer*	GetDisplayBuffer(void) { return &displayBuffer; }

	virtual	void	Display(void) {}

	virtual void	SetVisible(bool newVisible) { visible = newVisible; }

	/**
	 * Adds the input texture name to the DisplayBufferData’s list of texture names.
	 */
	virtual void	AddTexture(char * textName) { displayBuffer.AddTexture(textName); }

	/**
	 * Calculate the DISPARRAY_NODESIZE for a DisplayArray.
	 */
	virtual int		GetDISPARRAY_NODESIZE(const DisplayHeader * header) = 0;

	/**
	 * Calculate the iA_size for a DisplayArray.
	 */
	virtual int		GetdA_size(int DISPARRAY_NODESIZE) = 0;

	/**
	 * Calculate the dA_size for a DisplayArray.
	 */
	virtual int		GetiA_size(const DisplayHeader * header) = 0;

	/**
	 * Returns the name of the object that this DisplayManager represents.
	 */
	const char *	GetObjectName() { return displayBuffer.GetObjectName(); }

	/**
	 * Sets the name of the object that this DisplayManager represents.
	 */
	void			SetObjectName(const char * newObjectName) { 
		displayBuffer.SetObjectName(newObjectName); 
		// sxn66 change to the name of object
		displayBuffer.SetName(newObjectName);
	}

protected:
	bool			visible;
	DisplayBuffer	displayBuffer;

	friend DisplayBufferUnitTest;
	friend LoaderUnitTest;
	friend TextureUnitTest;
};

/****************************************************************
 *					POINTCLOUD DISPLAY MANAGER					*  
 ****************************************************************/

class PointCloudDisplayManager : public DisplayManager {
public:
	PointCloudDisplayManager(PointCloud *ingeometry, const DisplayHeader * inheader) :
	  geometry(ingeometry), DisplayManager(inheader) {}

	void			Display(void);

	int				GetDISPARRAY_NODESIZE(const DisplayHeader * header);

	int				GetdA_size(int DISPARRAY_NODESIZE);

	int				GetiA_size(const DisplayHeader * header);

protected:
	PointCloud		*geometry;
};


/****************************************************************
 *					VECTORFIELD DISPLAY MANAGER					*  
 ****************************************************************/

class VectorFieldDisplayManager : public DisplayManager {
public:
	VectorFieldDisplayManager(VectorField *ingeometry, DisplayHeader * inheader) :
	  geometry(ingeometry), DisplayManager(inheader)
	{
		// Change the display header objType to GIPSI_DRAW_LINE
		DisplayHeader hdr;
		GetDisplayBuffer()->GetDisplayHeader(&hdr);
		hdr.objType = GIPSI_DRAW_LINE;
		GetDisplayBuffer()->SetDisplayHeader(&hdr);
	}

	void			Display(void);

	int				GetDISPARRAY_NODESIZE(const DisplayHeader * header);

	int				GetdA_size(int DISPARRAY_NODESIZE);

	int				GetiA_size(const DisplayHeader * header);

protected:
	VectorField		*geometry;
};

/****************************************************************
 *					TRISURFACE DISPLAY MANAGER					*  
 ****************************************************************/

class TriSurfaceDisplayManager : public DisplayManager {
public:
	TriSurfaceDisplayManager(TriSurface *ingeometry, const DisplayHeader * inheader) :
		geometry(ingeometry), DisplayManager(inheader) {}

	void			Display(void);

	int				GetDISPARRAY_NODESIZE(const DisplayHeader * header);

	int				GetdA_size(int DISPARRAY_NODESIZE);

	int				GetiA_size(const DisplayHeader * header);

protected:
	TriSurface		*geometry;
};


/****************************************************************
 *					TETVOLUME DISPLAY MANAGER					*  
 ****************************************************************/

class TetVolumeDisplayManager : public DisplayManager {
public:
	TetVolumeDisplayManager(TetVolume *ingeometry, const DisplayHeader * inheader) :
		geometry(ingeometry), DisplayManager(inheader) {}

	void			Display(void);

	int				GetDISPARRAY_NODESIZE(const DisplayHeader * header);

	int				GetdA_size(int DISPARRAY_NODESIZE);

	int				GetiA_size(const DisplayHeader * header);

protected:
	TetVolume		*geometry;

};


class TextureDisplayManager : public DisplayManager {
public:
	TextureDisplayManager(DisplayTextureType textType, char * formattedFileName);
	~TextureDisplayManager()
	{
		if (textureFile)
		{
			delete textureFile;
			textureFile = NULL;
		}
	}

	void			GetDisplayHeader(DisplayHeader * outheader) {}

	void			SetDisplayHeader(const DisplayHeader * inheader) {}

	void			SetVisible(bool newVisible) {}

	void			Display();

	/**
	 * Overridden method: does nothing.
	 */
	void			AddTexture(char * textName) {}

	int				GetDISPARRAY_NODESIZE(const DisplayHeader * header);

	int				GetdA_size(int DISPARRAY_NODESIZE);

	int				GetiA_size(const DisplayHeader * header);

protected:
	TextureGeometry	*geometry;
	char			*textureFile;
	bool			static_texture_sent;

	friend TextureUnitTest;
};


/****************************************************************
 *					TEST DISPLAY MANAGER					*  
 ****************************************************************/

class TestDisplayManager : public DisplayManager {
public:
	TestDisplayManager(const DisplayHeader * inheader) :
		DisplayManager(inheader) {}

	void			Display(void) {}

	virtual int		GetDISPARRAY_NODESIZE(const DisplayHeader * header) { return 5; }

	virtual int		GetdA_size(int DISPARRAY_NODESIZE) { return 15; }

	virtual int		GetiA_size(const DisplayHeader * header) { return 10; }
};

#endif
