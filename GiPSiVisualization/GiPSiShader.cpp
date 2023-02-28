/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Shader Implementation (GiPSiShader.cpp).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen, M. Cenk Cavusoglu, Suriya Natsupakpong.
*/

/*
===============================================================================
	Headers
===============================================================================
*/

#include <stdio.h>
#include <fcntl.h>		// Needed for: open, lseek, close
#ifdef WIN32
#include <io.h>			// Needed for: O_RDONLY
#endif
					// on OpenBSD, O_RDONLY is in fcntl.h
#include <stdlib.h>		// exit()

#ifdef OPENBSD
#include <unistd.h>		// close()  (or should the code use fclose() )
#endif
#ifdef LINUX
#include <unistd.h>		// close()  (or should the code use fclose() )
#endif

#include <string.h>

#include "errors.h"

#include "GiPSiShader.h"

#include "logger.h"
#include "XMLNodeList.h"

/*
===============================================================================
	Shader
===============================================================================
*/

/**
 * Constructor.
 * 
 * @param shaderNode XML project 'shader' node containing initialization information.
 */
Shader::Shader(XMLNode * shaderNode)
{
	XMLNodeList * shaderChildren = shaderNode->GetChildren();

	XMLNode * pathNode = shaderChildren->GetNode("path");
	char * path = pathNode->GetValue();
	delete pathNode;

	XMLNode * passesNode = shaderChildren->GetNode("passes");
	char * passes = passesNode->GetValue();
	int nPasses = atoi(passes);
	delete passes;
	delete passesNode;
	delete shaderChildren;

	//char *path, const int nPasses
	GLchar *vertexShaderSource, *fragmentShaderSource;
	bool success;
	
	this->nPasses		= nPasses;
	this->currentPass	= 0;
	this->program		= new GLint[this->nPasses];

	if (nPasses < 1)
	{
		// Sanity check

		error_exit(1006, "Number of passes must be atleast 1.");
	}
	else if (nPasses == 1)
	{
		// Single pass shader	FORMAT:	myshader.vert
		//								myshader.frag

		success = false;
		ReadSource(path, &vertexShaderSource, &fragmentShaderSource);
		success = Install(vertexShaderSource, fragmentShaderSource, 0);
		if (success == true)
		{
			printf("Single pass shader successfully installed.\n");
		}
	}
	else
	{
		// Multi pass shader FORMAT:	myshader_pass0.vert
		//								myshader_pass0.frag
		//								myshader_pass1.vert
		//								myshader_pass1.frag		etc.

		int nthPass = 0;
		char name[100];
		const char number[30] = {'0', '1', '2', '3', '4', '5'};

		for (int i=0; i<this->nPasses; i++)
		{
			strcpy(name, path);
			strcat(name, "_pass");
			strncat(name, &number[i], 1);

			success = false;
			ReadSource(name, &vertexShaderSource, &fragmentShaderSource);
			success = Install(vertexShaderSource, fragmentShaderSource, i);
			if (success == true)
			{
				nthPass++;
			}
		}
		printf("Multi pass (%i) shader successfully installed.\n", nthPass);
	}
	delete path;
}

/**
 * Indicate if the system has the necessary OpenGL Shading Language extensions.
 */
bool Shader::Compatibility(void)
{
	char *extensions, *vertex, *fragment;
	
	// Ensure we have the necessary OpenGL Shading Language extensions.

	extensions	= (char*)glGetString(GL_EXTENSIONS);
	vertex		= strstr(extensions, "GL_ARB_vertex_shader");
	fragment	= strstr(extensions, "GL_ARB_fragment_shader");

	if (vertex == NULL)		{ printf("No support for vertex shaders\n");  }
	if (fragment == NULL)	{ printf("No support for fragment shaders\n");}

	if (vertex == NULL || fragment == NULL)
	{
		return false;
	}
	else
	{
		return true;
	}

	return false;
}

/**
 * Return the size in bytes of the contents of the indicated shader file.
 * 
 * @param fileName Pointer to the shader file name root.
 * @param shaderType Indicate if we are interested in the vertex or fragment shader.
 */
int Shader::FileSize(char *fileName, ShaderType shaderType)
{
    int fd;
    char name[100];
    int count = -1;

    strcpy(name, fileName);

    switch (shaderType)
    {
        case VertexShader:
            strcat(name, ".vert");
            break;
        case FragmentShader:
            strcat(name, ".frag");
            break;
        default:
            printf("ERROR: unknown shader file type\n");
            exit(1);
            break;
    }

    // Open the file, seek to the end to find its length

	fd = open(name, O_RDONLY);

    if (fd != -1)
    {
        count = lseek(fd, 0, SEEK_END) + 1;
        close(fd);
    }

    return count;
}

/**
 * Return the number of passes this shader uses.
 */
int Shader::GetPasses(void)
{
	return this->nPasses;
}

/**
 * Get the loc of the indicated uniform variable.
 * 
 * @param program Shader program ID.
 * @param name Uniform variable name.
 */
int Shader::GetUniformLoc(GLint program, const GLchar* name)
{
    int loc;

    loc = glGetUniformLocation(program, name);

    if (loc == GL_INVALID_VALUE)
	{
		error_exit(1007, "No such uniform variable");
	}

    //PrintOpenGLError();
    return loc;
}

/**
 * Install this shader in the GPU.
 * 
 * @param vertexShaderSrc Vertex shader file source.
 * @param fragmentShaderSrc Fragment shader file source.
 * @param nthPass Current pass.
 */
bool Shader::Install(const GLchar *vertexShaderSrc, const GLchar *fragmentShaderSrc, const int nthPass)
{
	GLint		vertexShaderPrg, fragmentShaderPrg;
    GLint       vertCompiled, fragCompiled;
    GLint       linked;
	GLsizei		length = 0;

    // Create a vertex shader object and a fragment shader object

    vertexShaderPrg	  = glCreateShader(GL_VERTEX_SHADER);
    fragmentShaderPrg = glCreateShader(GL_FRAGMENT_SHADER);

    // Load source code strings into shaders

    glShaderSource(vertexShaderPrg, 1, &vertexShaderSrc, NULL);
    glShaderSource(fragmentShaderPrg, 1, &fragmentShaderSrc, NULL);

    // Compile the vertex shader, and print out the compiler log file.

    glCompileShader(vertexShaderPrg);
    PrintOpenGLError();
    glGetShaderiv(vertexShaderPrg, GL_COMPILE_STATUS, &vertCompiled);
    PrintInfoLog(vertexShaderPrg);

    // Compile the fragment shader, and print out the compiler log file.

    glCompileShader(fragmentShaderPrg);
    PrintOpenGLError();
    glGetShaderiv(fragmentShaderPrg, GL_COMPILE_STATUS, &fragCompiled);
    PrintInfoLog(fragmentShaderPrg);

    if (!vertCompiled || !fragCompiled)
	{
        return false;
	}

    // Create a program object and attach the two compiled shaders

    program[nthPass] = glCreateProgram();
    glAttachShader(program[nthPass], vertexShaderPrg);
    glAttachShader(program[nthPass], fragmentShaderPrg);

    // Link the program object and print out the info log

    glLinkProgram(program[nthPass]);
    PrintOpenGLError();
    glGetProgramiv(program[nthPass], GL_LINK_STATUS, &linked);
    PrintInfoLog(program[nthPass]);

    if (!linked)
	{
        return false;
	}

    return true;
}

/**
 * Print infolog of indicated object.
 * 
 * @param obj Handle of object for which to print infolog.
 */
void Shader::PrintInfoLog(GLint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    GLchar *infoLog;

    PrintOpenGLError();
		
	if(glIsShader(obj))
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	else
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    PrintOpenGLError();

    if (infologLength > 0)
    {
        infoLog = new GLchar[infologLength];
        if (infoLog == NULL)
        {
            printf("ERROR: Could not allocate InfoLog buffer\n");
            exit(1);
        }

		if (glIsShader(obj))
			glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		else
			glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);

		if (charsWritten > 0)
	        printf("InfoLog:\n%s\n\n", infoLog);
        delete[] infoLog;
    }
    PrintOpenGLError();
}

/**
 * Print last registered OpenGL error.
 * 
 * @param file Source file where error occurred.
 * @param line Line of source file where error occurred.
 */
int Shader::PrintOGLError(char *file, int line)
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

/**
 * Load the contents of the indicated file into a character buffer.
 * 
 * @param fileName Root name of shader file to read.
 * @param shaderType Indicates if this is a vertex or fragment shader.
 * @param shaderText Pointer that is set to the filled buffer.
 * @param size Size in bytes of the file to be read.
 */
int Shader::ReadFile(char *fileName, ShaderType shaderType, char *shaderText, int size)
{
    FILE *fh;
    char name[100];
    int count;

    strcpy(name, fileName);

    switch (shaderType) 
    {
        case VertexShader:
            strcat(name, ".vert");
            break;
        case FragmentShader:
            strcat(name, ".frag");
            break;
        default:
            printf("ERROR: unknown shader file type\n");
            exit(1);
            break;
    }

    // Open the file

    fh = fopen(name, "r");
    if (!fh)
        return -1;

    // Get the shader from the file.

    fseek(fh, 0, SEEK_SET);
    count = fread(shaderText, 1, size, fh);
    shaderText[count] = '\0';

    if (ferror(fh))
        count = 0;

    fclose(fh);
    return count;
}

/**
 * Read the source files for the indicated shader.
 * 
 * @param fileName Root name of shader files to read.
 * @param vertexShaderSrc Pointer set to a buffer containing the vertex shader source.
 * @param vertexShaderSrc Pointer set to a buffer containing the fragment shader source.
 */
bool Shader::ReadSource(char *fileName, GLchar **vertexShaderSrc, GLchar **fragmentShaderSrc)
{
	int vSize, fSize;

    // Allocate memory to hold the source of our shaders.
 
    vSize = FileSize(fileName, VertexShader);
    fSize = FileSize(fileName, FragmentShader);

    if ((vSize == -1) || (fSize == -1))
    {
        printf("Cannot determine size of the shader %s\n", fileName);
        return 0;
    }

    *vertexShaderSrc	= new GLchar[vSize];
    *fragmentShaderSrc  = new GLchar[fSize];

    // Read the source code
   
    if (!ReadFile(fileName, VertexShader, *vertexShaderSrc, vSize))
    {
        printf("Cannot read the file %s.vert\n", fileName);
        return false;
    }

    if (!ReadFile(fileName, FragmentShader, *fragmentShaderSrc, fSize))
    {
        printf("Cannot read the file %s.frag\n", fileName);
        return false;
    }

    return true;
}

/**
 * Select this shader for use in current rendering.
 * 
 * @param nthPass Current shader pass.
 */
void Shader::Select(const int nthPass)
{
	bool validPass = this->ValidatePass(nthPass);

	// Install program object as part of current state

	if (validPass == true)
	{
		this->currentPass = nthPass;

		glUseProgram(this->program[this->currentPass]);
	}
}

/**
 * Deselect the currently selected shader.
 */
void Shader::UseFixedPipeline(void)
{
	glUseProgram(0);
}

/**
 * Get the current shader program ID.
 */
GLint Shader::GetCurrentProgram(void)
{
	return this->program[this->currentPass];
}

/**
 * Make sure the chosen texture unit is within the legal range.
 * 
 * @param uint Texture unit to validate.
 */
bool Shader::ValidateTextureUnit(const int unit)
{
	// Make sure the chosen texture unit is within the legal range, this OpenGL
	// implementation has support for 8 active textures. (4 on the lab computer.)

	bool validTextureUnit = false;
	
	int maxTextureUnits;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits);

	if (unit >= 0 && unit < maxTextureUnits)
	{
		validTextureUnit = true;
	}
	else
	{
		error_exit(1008, "Invalid texture unit selected");
	}

	return validTextureUnit;
}

/**
 * Make this the chosen pass is within our range of passes.
 * 
 * @param nthPass Pass to be validated.
 */
bool Shader::ValidatePass(const int nthPass)
{
	// Make sure a legitimate pass has been selected

	bool validPass = false;

	if (nthPass >= 0 && nthPass < this->GetPasses())
	{
		validPass = true;
	}
	else
	{
		error_exit(1009, "Invalid pass selected");
	}

	return validPass;
}

