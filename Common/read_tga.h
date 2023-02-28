/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is obj loader (read_tga.cpp).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen.
*/

#ifndef READTGA__H
#define READTGA__H

/*
===============================================================================
	Headers
===============================================================================
*/

/*
The TGA file format has some advantages over other commonly used image formats,
namely:

    * Supports 8, 16, 24 and 32 bit color depth and an alpha channel, which is
		what we need to use images as textures in OpenGL.
    * The file format is very easy to read.
    * Supported by most image editing programs (Photoshop etc.)
    * This file format is very well documented.

A good description of the TGA file format can be found at
http://organicbit.com/closecombat/formats/tga.html
*/

#include <string>

/*
===============================================================================
	TGA
===============================================================================
*/

typedef unsigned char byte;
typedef unsigned int  uint;

enum ImageType
{
	ImageType_Undefined,
	ImageType_RGB,
	ImageType_RGBA,
	ImageType_Greyscale
};

class TGA
{
public:
    TGA();
    TGA(const char *fileName);
    virtual ~TGA();
     
	byte *		GetPixels(void);
	uint		GetPixelDepth(void);
	uint		GetAlphaDepth(void);
	uint		GetWidth(void);
	uint		GetHeigth(void);
	ImageType	GetImageType(void);
	
private:
	byte	   *pixels;
	uint		pixelDepth;
	uint		alphaDepth;
	uint		width;
	uint		height;
	ImageType	type;
	bool		loaded;

	bool LoadFromFile(const char *fileName);
	void Clear();
};

#endif READTGA__H