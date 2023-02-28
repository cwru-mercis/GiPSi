/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Shader Definition (GiPSiShader.h).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen.
*/

////	GIPSISHADER.H v0.0
////
////	GiPSi Shader
////
////////////////////////////////////////////////////////////////

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

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glext.h>
#include "glext.h"

#include "glprocs.h"

//#include "../OpenGL15/glprocs.h"
//#include "gl.h"
//#include "glprocs.h"

#include "GiPSiTexture.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

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

class Shader
{
public:
	Shader(XMLNode * shaderNode);

	virtual ShaderName	 GetShaderName(void) = 0;
	virtual void		 SetParameters(ShaderParams * params) = 0;
	int					 GetPasses(void);
	void				 Select(const int nthPass);

	static  bool		 Compatibility(void);
	static	void		 UseFixedPipeline(void);

	GLint				 GetCurrentProgram(void);

protected:
	int					 nPasses;
	int					 currentPass;
	GLint				*program;

	int	 GetUniformLoc		(GLint program, const GLchar *name);
	bool ValidateTextureUnit(const int unit);
	bool ValidatePass		(const int nthPass);
	
private:
	int	 FileSize			(char *fileName, ShaderType shaderType);
	bool Install			(const GLchar *vertexShaderSrc, const GLchar *fragmentShaderSrc, const int nthPass);
	void PrintInfoLog		(GLint obj);
	int	 PrintOGLError		(char *file, int line);
	int	 ReadFile			(char *fileName, ShaderType shaderType, char *shaderText, int size);
	bool ReadSource			(char *fileName, GLchar **vertexShaderSrc, GLchar **fragmentShaderSrc);
};

#define PrintOpenGLError() Shader::PrintOGLError(__FILE__, __LINE__)

#endif // #ifndef GIPSISHADER__H
