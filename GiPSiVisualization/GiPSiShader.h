/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Shader implementation (GiPSiShader.h).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen.
*/

#ifndef GIPSISHADER__H
#define GIPSISHADER__H

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

#include "gl.h"
#include "glprocs.h"

#include "GiPSiTexture.h"

/*
===============================================================================
	Basic shader class
===============================================================================
*/

enum ShaderType
{
	VertexShader,
    FragmentShader
};

enum ShaderName
{
	ShaderName_Phong,
	ShaderName_Bump,
	ShaderName_Tissue
};

class Shader
{
public:
	Shader(char *path, const int nPasses, ShaderName name);

	ShaderName	GetName(void);
	int			GetPasses(void);
	void		Select(const int nthPass);

	static  bool Compatibility(void);
	static	void UseFixedPipeline(void);

protected:
	int				 nPasses;
	int				 currentPass;
	GLhandleARB		*program;
	ShaderName		 name;

	int	 GetUniformLoc		(GLhandleARB program, const GLcharARB *name);
	bool ValidateTextureUnit(const int unit);
	bool ValidatePass		(const int nthPass);
	
private:
	int	 FileSize			(char *fileName, ShaderType shaderType);
	bool Install			(const GLcharARB *vertexShaderSrc, const GLcharARB *fragmentShaderSrc, const int nthPass);
	void PrintInfoLog		(GLhandleARB obj);
	int	 PrintOGLError		(char *file, int line);
	int	 ReadFile			(char *fileName, ShaderType shaderType, char *shaderText, int size);
	bool ReadSource			(char *fileName, GLcharARB **vertexShaderSrc, GLcharARB **fragmentShaderSrc);
};

#define PrintOpenGLError() Shader::PrintOGLError(__FILE__, __LINE__)

// *******************************************
// *******************************************
//  CODE BELOW HERE 
//   IS FOR SPECIFIC SHADERS PRESENT
// *******************************************
// *******************************************

/*
===============================================================================
	Phong shader
===============================================================================
*/

class PhongShader : public Shader
{
public:
	PhongShader(char *path, const int nPasses, ShaderName name);

	void SetParameters(const bool halfWayApprox, const int texUnitBase);

protected:
	bool		halfwayApprox;

private:

};

/*
===============================================================================
	Bump shader
===============================================================================
*/

class BumpShader : public Shader
{
public:
	BumpShader(char *path, const int nPasses, ShaderName name);

	void SetParameters(const int texUnitBase, const int texUnitHeight);

protected:

private:

};

/*
===============================================================================
	Tissue shader
===============================================================================
*/

class TissueShader : public Shader
{
public:
	TissueShader(char *path, const int nPasses, ShaderName name);

	void SetParameters(	const int	texUnitBase,
						const int	texUnitHeightconst,
						const float ambientContribution,
						const float diffuseContribution,
						const float specularContribution,
						const float glossiness,
						const float stepSize,
						const float bumpiness,
						const float opacity,
						const float displacement );

protected:

private:

};

#endif // #ifndef GIPSISHADER__H