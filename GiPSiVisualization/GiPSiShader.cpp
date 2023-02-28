/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Shader implementation (GiPSiShader.cpp).

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

#include <fcntl.h>		// Needed for: open, lseek, close
#include <io.h>			// Needed for: O_RDONLY
#include <stdio.h>
#include <string.h>

#include "errors.h"

#include "GiPSiShader.h"

/*
===============================================================================
	Shader
===============================================================================
*/

Shader::Shader(char *path, const int nPasses, ShaderName name)
{
	GLcharARB *vertexShaderSource, *fragmentShaderSource;
	bool success;
	
	this->nPasses		= nPasses;
	this->currentPass	= 0;
	this->name			= name;
	this->program		= new GLhandleARB[this->nPasses];

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
}

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
}

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

ShaderName Shader::GetName(void)
{
	return this->name;
}

int Shader::GetPasses(void)
{
	return this->nPasses;
}

int Shader::GetUniformLoc(GLhandleARB program, const GLcharARB* name)
{
    int loc;

    loc = glGetUniformLocationARB(program, name);

    if (loc == -1)
	{
		error_exit(1007, "No such uniform variable");
	}

    PrintOpenGLError();
    return loc;
}

bool Shader::Install(const GLcharARB *vertexShaderSrc, const GLcharARB *fragmentShaderSrc, const int nthPass)
{
	GLhandleARB vertexShaderPrg, fragmentShaderPrg;
    GLint       vertCompiled, fragCompiled;
    GLint       linked;

    // Create a vertex shader object and a fragment shader object

    vertexShaderPrg	  = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    fragmentShaderPrg = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    // Load source code strings into shaders

    glShaderSourceARB(vertexShaderPrg, 1, &vertexShaderSrc, NULL);
    glShaderSourceARB(fragmentShaderPrg, 1, &fragmentShaderSrc, NULL);

    // Compile the vertex shader, and print out the compiler log file.

    glCompileShaderARB(vertexShaderPrg);
    PrintOpenGLError();
    glGetObjectParameterivARB(vertexShaderPrg, GL_OBJECT_COMPILE_STATUS_ARB, &vertCompiled);
    PrintInfoLog(vertexShaderPrg);

    // Compile the fragment shader, and print out the compiler log file.

    glCompileShaderARB(fragmentShaderPrg);
    PrintOpenGLError();
    glGetObjectParameterivARB(fragmentShaderPrg, GL_OBJECT_COMPILE_STATUS_ARB, &fragCompiled);
    PrintInfoLog(fragmentShaderPrg);

    if (!vertCompiled || !fragCompiled)
	{
        return false;
	}

    // Create a program object and attach the two compiled shaders

    program[nthPass] = glCreateProgramObjectARB();
    glAttachObjectARB(program[nthPass], vertexShaderPrg);
    glAttachObjectARB(program[nthPass], fragmentShaderPrg);

    // Link the program object and print out the info log

    glLinkProgramARB(program[nthPass]);
    PrintOpenGLError();
    glGetObjectParameterivARB(program[nthPass], GL_OBJECT_LINK_STATUS_ARB, &linked);
    PrintInfoLog(program[nthPass]);

    if (!linked)
	{
        return false;
	}

	//GLboolean cenktemp;
	//cenktemp=glIsProgramARB(program[0]);
	//printf("%d\n",cenktemp);

    return true;
}

void Shader::PrintInfoLog(GLhandleARB obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    GLcharARB *infoLog;

    PrintOpenGLError();

    glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);

    PrintOpenGLError();

    if (infologLength > 0)
    {
        infoLog = new GLcharARB[infologLength];
        if (infoLog == NULL)
        {
            printf("ERROR: Could not allocate InfoLog buffer\n");
            exit(1);
        }
        glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
        printf("InfoLog:\n%s\n\n", infoLog);
        delete[] infoLog;
    }
    PrintOpenGLError();
}

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

bool Shader::ReadSource(char *fileName, GLcharARB **vertexShaderSrc, GLcharARB **fragmentShaderSrc)
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

    *vertexShaderSrc	= new GLcharARB[vSize];
    *fragmentShaderSrc  = new GLcharARB[fSize];

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

void Shader::Select(const int nthPass)
{
	bool validPass = this->ValidatePass(nthPass);

	// Install program object as part of current state

	if (validPass == true)
	{
		this->currentPass = nthPass;

		glUseProgramObjectARB(this->program[this->currentPass]);
	}
}

void Shader::UseFixedPipeline(void)
{
	glUseProgramObjectARB(0);
}

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

// *******************************************
// *******************************************
//  CODE BELOW HERE 
//   IS FOR SPECIFIC SHADERS PRESENT
// *******************************************
// *******************************************

/*
===============================================================================
	PhongShader
===============================================================================
*/

PhongShader::PhongShader(char *path, const int nPasses, ShaderName name) : Shader(path, nPasses, name)
{
	
}

void PhongShader::SetParameters(const bool halfWayApprox, const int texUnitBase)
{
	// Setup light

	int sizeOfLightVector = 4;

	GLfloat *lightPos = new GLfloat[sizeOfLightVector];
	glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glUniform4fvARB(GetUniformLoc( this->program[this->currentPass], "LightPosition" ), 1, lightPos);

	// Setup textures
	
	bool validTextureUnit = this->ValidateTextureUnit(texUnitBase);

	if (validTextureUnit == true)
	{
		int samplerUniformLoc;
		samplerUniformLoc = GetUniformLoc(this->program[this->currentPass], "Base" );
		glUniform1iARB(samplerUniformLoc, texUnitBase);
	}

	// Setup variables

	int variableUniformLoc;
	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "HalfWayApprox" );
	glUniform1iARB(variableUniformLoc, halfWayApprox);
}

/*
===============================================================================
	BumpShader
===============================================================================
*/

BumpShader::BumpShader(char *path, const int nPasses, ShaderName name) : Shader(path, nPasses, name)
{
	
}

void BumpShader::SetParameters(const int texUnitBase, const int texUnitHeight)
{
	// Setup light

	int sizeOfLightVector = 4;

	GLfloat *lightPos = new GLfloat[sizeOfLightVector];
	glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glUniform4fvARB(GetUniformLoc( this->program[this->currentPass], "LightPosition" ), 1, lightPos);

	// Setup textures
	
	bool validTextureUnits = this->ValidateTextureUnit(texUnitBase) &&
							 this->ValidateTextureUnit(texUnitHeight);

	if (validTextureUnits == true)
	{
		int samplerUniformLoc;
		samplerUniformLoc = GetUniformLoc(this->program[this->currentPass], "Base" );
		glUniform1iARB(samplerUniformLoc, texUnitBase);

		samplerUniformLoc = GetUniformLoc(this->program[this->currentPass], "Height" );
		glUniform1iARB(samplerUniformLoc, texUnitHeight);
	}
}

/*
===============================================================================
	TissueShader
===============================================================================
*/

TissueShader::TissueShader(char *path, const int nPasses, ShaderName name) : Shader(path, nPasses, name)
{
	
}

void TissueShader::SetParameters(	const int	texUnitBase,
									const int	texUnitHeight,
									const float ambientContribution,
									const float diffuseContribution,
									const float specularContribution,
									const float glossiness,
									const float stepSize,
									const float bumpiness,
									const float opacity,
									const float displacement )
{
	// Setup light

	int sizeOfLightVector = 4;

	GLfloat *lightPos = new GLfloat[sizeOfLightVector];
	glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glUniform4fvARB(GetUniformLoc(this->program[this->currentPass], "LightPosition" ), 1, lightPos);

	// Setup textures

	
	bool validTextureUnits = this->ValidateTextureUnit(texUnitBase) &&
							 this->ValidateTextureUnit(texUnitHeight);

	if (validTextureUnits == true)
	{
		int  samplerUniformLoc;
		samplerUniformLoc = GetUniformLoc(this->program[this->currentPass], "Base" );
		glUniform1iARB(samplerUniformLoc, texUnitBase);

		samplerUniformLoc = GetUniformLoc(this->program[this->currentPass], "Height" );
		glUniform1iARB(samplerUniformLoc, texUnitHeight);
	}

	// Setup variables

	int variableUniformLoc;
	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "AmbientContribution" );
	glUniform1fARB(variableUniformLoc, ambientContribution);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "DiffuseContribution" );
	glUniform1fARB(variableUniformLoc, diffuseContribution);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "SpecularContribution" );
	glUniform1fARB(variableUniformLoc, specularContribution);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "Glossiness" );
	glUniform1fARB(variableUniformLoc, glossiness);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "StepSize" );
	glUniform1fARB(variableUniformLoc, stepSize);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "Bumpiness" );
	glUniform1fARB(variableUniformLoc, bumpiness);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "Opacity" );
	glUniform1fARB(variableUniformLoc, opacity);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "Displacement" );
	glUniform1fARB(variableUniformLoc, displacement);
}