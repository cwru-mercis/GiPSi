/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Texture implementation (GiPSiTexture.cpp).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen.
*/

/*
===============================================================================
	Headers
===============================================================================
*/

/*
http://www.opengl.org/resources/faq
OpenGL FAQ Getting started issue 2.070:
Also, note that you'll need to put an #include <windows.h> statement before the
#include<GL/gl.h>. Microsoft requires system DLLs to use a specific calling convention
that isn't the default calling convention for most Win32 C compilers, so they've
annotated the OpenGL calls in gl.h with some macros that expand to nonstandard C
syntax. This causes Microsoft's C compilers to use the system calling convention.
One of the include files included by windows.h defines the macros.
*/

#include <windows.h>
#include <GL/glu.h>
#include <stdio.h>

#include "errors.h"
#include "gl.h"
#include "glprocs.h"
#include "read_tga.h"

#include "GiPSiTexture.h"

/*
===============================================================================
	Texture
===============================================================================
*/

Texture::Texture(TextureName name)
{
	this->id	= 0;
	this->name	= name;
}

TextureName Texture::GetName(void)
{
	return this->name;
}

/*
===============================================================================
	2D texture
===============================================================================
*/

Texture2D::Texture2D(const TextureName name, const char *fileName) : Texture(name)
{
	// This constructor only works for tga files

	char *extension = strrchr(fileName, '.');

	if (strcmp(extension, ".tga"))
	{
		error_exit(1005, "Texture2D constructor can only open .tga files");
	}

	// Load image from tga file

	TGA *TGAImage	= new TGA(fileName);

	// Use to dimensions of the image as the texture dimensions

	this->width		= TGAImage->GetWidth();
	this->height	= TGAImage->GetHeigth();

	// The parameters for actual textures are changed

	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_2D, this->id);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	// Finaly build the mipmaps

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, this->width, this->height, GL_RGBA, GL_UNSIGNED_BYTE, TGAImage->GetPixels());

    // Release allocated memory

    delete TGAImage;
}

void Texture2D::LoadFromTGAFile(const char *fileName)
{
	// Load image from tga file

	TGA *TGAImage = new TGA(fileName);

	// The parameters for actual textures are changed

	glBindTexture(GL_TEXTURE_2D, this->id);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    
	// Finaly build the mipmaps

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, this->width, this->height, GL_RGBA, GL_UNSIGNED_BYTE, TGAImage->GetPixels());

    // Release allocated memory

    delete TGAImage;
}

void Texture2D::OverrideWithFrameBuffer(void)
{
	// NOTE: It is possible to use glCopyTexSubImage2D as well, I'm not sure
	// which one is faster.

	glBindTexture(GL_TEXTURE_2D, this->id);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, this->width, this->height, 0);
	//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, this->width, this->height);
}

void Texture2D::RenderTarget(void)
{
	// NOTE: Use glCopyTexImage2D here

	glBindTexture(GL_TEXTURE_2D, this->id);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, this->width, this->height, 0);
}

void Texture2D::Select(void)
{
	glBindTexture(GL_TEXTURE_2D, this->id);
}

void Texture2D::SetViewPortToSizeOf(void)
{
	glViewport(0, 0, this->width, this->height);
}

void Texture2D::SwapRGBA(const int nBlocks, char *data)
{
	char swap;

	for (int i=0; i<nBlocks; i++)
	{
		// Swap green and alpha
		swap = data[4*i+1];			
		data[4*i+1] = data[4*i+3];		
		data[4*i+3] = swap;
	}
}