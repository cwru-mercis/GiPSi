/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Texture implementation (GiPSiTexture.h).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen.
*/

#ifndef GIPSITEXTURE__H
#define GIPSITEXTURE__H

/*
===============================================================================
	Headers
===============================================================================
*/

/*
===============================================================================
	Basic texture class
===============================================================================
*/

enum TextureName
{
	TextureName_TestBlood,
	TextureName_AnteriorSeptalVeinBase,
	TextureName_AnteriorSeptalVeinHeightMap,
	TextureName_BasilarArteryBase,
	TextureName_BasilarArteryHeightMap,
	TextureName_ChoroidPlexusBase,
	TextureName_ChoroidPlexusHeightMap,
	TextureName_MammillarryBodiesBase,
	TextureName_MammillarryBodiesHeightMap,
	TextureName_VentricleFloorBase,
	TextureName_VentricleFloorHeightMap,
	TextureName_VentricleSystemBase,
	TextureName_VentricleSystemHeightMap
};

class Texture
{
public:
	Texture(TextureName name);

	TextureName	 GetName(void);

	virtual void LoadFromTGAFile(const char *fileName)	= 0;
	virtual void RenderTarget(void)						= 0;
	virtual void Select(void)							= 0;
	
	virtual void SetViewPortToSizeOf	(void) = 0;
	virtual void OverrideWithFrameBuffer(void) = 0;

protected:
	unsigned int	 id;
	TextureName		 name;

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
	Texture2D(const TextureName name, const char *fileName);

	virtual void Select(void);
	
	virtual void RenderTarget(void);
	virtual void SetViewPortToSizeOf	(void);
	virtual void OverrideWithFrameBuffer(void);

protected:

private:
	int				width;
	int				height;

	virtual void LoadFromTGAFile(const char *fileName);

	void SwapRGBA(const int nBlocks, char *data);

};

/*
===============================================================================
	3D texture
===============================================================================
*/

class Texture3D : virtual public Texture
{
public:

protected:

private:
	int				width;
	int				height;
	int				depth;

};

#endif // #ifndef GIPSITEXTURE__H