/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Texture Definition (GiPSiTexture.h).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen.
*/

////	GIPSITEXTURE.H v0.0
////
////	GiPSi Texture
////
////////////////////////////////////////////////////////////////

#ifndef GIPSITEXTURE__H
#define GIPSITEXTURE__H

/*
===============================================================================
	Headers
===============================================================================
*/

#include "GiPSiDisplay.h"

class TextureUnitTest;

/*
===============================================================================
	Basic texture class
===============================================================================
*/

class Texture
{
public:
	Texture(DisplayBuffer * newBuffer);
	~Texture()
	{
		if (name)
			delete name;
	}

	const char * GetObjectName(void);

	virtual void UpdateTexture()						= 0;
	virtual void RenderTarget(void)						= 0;
	virtual void Select(void)							= 0;
	
	virtual void SetViewPortToSizeOf	(void) = 0;
	virtual void OverrideWithFrameBuffer(void) = 0;

protected:
	unsigned int	id;
	char		   *name;

	DisplayBuffer * buffer;

	friend TextureUnitTest;

private:
};

/*
===============================================================================
	2D texture
===============================================================================
*/

class Texture2D : virtual public Texture
{
public:
	Texture2D(DisplayBuffer * newBuffer);

	virtual void Select(void);
	
	virtual void RenderTarget(void);
	virtual void SetViewPortToSizeOf	(void);
	virtual void OverrideWithFrameBuffer(void);

protected:

	friend TextureUnitTest;

private:
	int					width;
	int					height;
	DisplayTextureType	textureType;

	virtual void UpdateTexture();

	void SwapRGBA(const int nBlocks, char *data);
};

/*
===============================================================================
	3D texture
===============================================================================
*/

/*
class Texture3D : virtual public Texture
{
public:

protected:

	friend TextureUnitTest;

private:
	int				width;
	int				height;
	int				depth;

};
*/

#endif // #ifndef GIPSITEXTURE__H

