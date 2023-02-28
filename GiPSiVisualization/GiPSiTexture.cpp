/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Texture Implementation (GiPSiTexture.cpp).

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

#ifdef WIN32
#include <windows.h>
#endif

//#include <windows.h>
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

/**
 * Constructor.
 * 
 * @param newbuffer DisplayBuffer containing new texture data.
 */
Texture::Texture(DisplayBuffer * newBuffer) :
	buffer(newBuffer), name(NULL)
{
	this->id	= 0;
	int nameLength = strlen(buffer->GetObjectName()) + 1;
	this->name	= new char[nameLength];
	strcpy_s(this->name, nameLength, buffer->GetObjectName());
}

/**
 * Returns texture name.
 */
const char * Texture::GetObjectName(void)
{
	return this->name;
}

/*
===============================================================================
	2D texture
===============================================================================
*/

/**
 * Constructor.
 * 
 * @param newbuffer DisplayBuffer containing new texture data.
 */
Texture2D::Texture2D(DisplayBuffer * newBuffer) : Texture(newBuffer)
{
	this->textureType = buffer->GetTextureType();
	DisplayArray * display = buffer->GetReadArray();

	if (this->textureType == GIPSI_2D_STATIC_SERVER ||
		this->textureType == GIPSI_2D_DYNAMIC_SERVER)
	{
		// Use array to calculate dimensions of the image as the texture dimensions
		// If the array has not yet been initialized, we assume that it will
		// be initialized during a future pass, and move on for now
		if (display->DISPARRAY_NODESIZE > 0 &&
			display->iA_size > 0)
		{
			this->width		= display->DISPARRAY_NODESIZE;
			this->height	= display->iA_size / display->DISPARRAY_NODESIZE;

			// The parameters for actual textures are changed
			glGenTextures(1, &this->id);
			glBindTexture(GL_TEXTURE_2D, this->id);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

			// Finaly build the mipmaps
			gluBuild2DMipmaps(GL_TEXTURE_2D, 4, this->width, this->height, GL_RGBA, GL_UNSIGNED_BYTE, display->indexArray);
		}
		else
		{
			this->width		= 0;
			this->height	= 0;
		}
	}
	else if (this->textureType == GIPSI_2D_STATIC_CLIENT)
	{
		// Use array to identify texture file to load
		// If the array has not yet been initialized, we assume that it will
		// be initialized during a future pass, and move on for now
		if (display->iA_size > 0)
		{
			char * fileName = new char[display->iA_size + 1];
			for (int i = 0; i < display->iA_size; i++)
			{
				fileName[i] = (char)display->indexArray[i];
			}
			fileName[display->iA_size] = '\0';

			// This loader only works for tga files
			const char *extension = strrchr(fileName, '.');

			if (strcmp(extension, ".tga"))
			{
				error_exit(1005, "Texture2D can only open .tga files");
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
		else
		{
			this->width		= 0;
			this->height	= 0;
		}
	}
}

/**
 * Update text with newest display data.
 */
void Texture2D::UpdateTexture()
{
	// This only works on a valid texture type
	if (this->textureType == GIPSI_NO_TEXTURE)
		return;

	// Do not load a static texture if we have already loaded it
	if ((this->textureType == GIPSI_2D_STATIC_SERVER ||
		this->textureType == GIPSI_2D_STATIC_CLIENT) &&
		this->width > 0 && this->height > 0)
		return;

	// Comment this out to enable dynamic textures (performance-intensive!)
	/*
	if (this->textureType == GIPSI_2D_DYNAMIC_SERVER &&
		this->width > 0 && this->height > 0)
		return;
	*/

	// Now, check if we can update the texture
	buffer->ConditionalDequeue();
	DisplayArray * display = buffer->GetReadArray();
	if (display->iA_size <= 0)
	{
		return;
	}

	// Update the texture
	if (this->textureType == GIPSI_2D_STATIC_SERVER ||
		this->textureType == GIPSI_2D_DYNAMIC_SERVER)
	{
		// If the texture was not yet loaded, perform the initialization now
		if (this->width == 0 && this->height == 0)
		{
			this->width		= display->DISPARRAY_NODESIZE;
			this->height	= display->iA_size / display->DISPARRAY_NODESIZE;
			glGenTextures(1, &this->id);
		}

		// The parameters for actual textures are changed

		glBindTexture(GL_TEXTURE_2D, this->id);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	    
		// Finaly build the mipmaps

		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, this->width, this->height, GL_RGBA, GL_UNSIGNED_BYTE, display->indexArray);
	}
	else if (this->textureType == GIPSI_2D_STATIC_CLIENT)
	{
		// Use array to identify texture file to load
		char * fileName = new char[display->iA_size + 1];
		for (int i = 0; i < display->iA_size; i++)
		{
			fileName[i] = (char)display->indexArray[i];
		}
		fileName[display->iA_size] = '\0';

		// This loader only works for tga files
		const char *extension = strrchr(fileName, '.');

		if (strcmp(extension, ".tga"))
		{
			error_exit(1005, "Texture2D can only open .tga files");
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
}

/**
 * Placeholder text.
 */
void Texture2D::OverrideWithFrameBuffer(void)
{
	// NOTE: It is possible to use glCopyTexSubImage2D as well, I'm not sure
	// which one is faster.

	glBindTexture(GL_TEXTURE_2D, this->id);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, this->width, this->height, 0);
	//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, this->width, this->height);
}

/**
 * Placeholder text.
 */
void Texture2D::RenderTarget(void)
{
	// NOTE: Use glCopyTexImage2D here

	glBindTexture(GL_TEXTURE_2D, this->id);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, this->width, this->height, 0);
}

/**
 * Bind this texture.
 */
void Texture2D::Select(void)
{
	// We can't bind this if it hasn't been initialized
	if (this->width > 0 && this->height > 0)
		glBindTexture(GL_TEXTURE_2D, this->id);
}

/**
 * Set viewport size to texture size.
 */
void Texture2D::SetViewPortToSizeOf(void)
{
	glViewport(0, 0, this->width, this->height);
}

/**
 * Swap green and alpha channels.
 */
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

