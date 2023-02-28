/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit Shaders Implementation (ToolkitShaders.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	TOOLKITSHADERS.CPP v0.0
////
////	Toolkit Connector Loader
////
////////////////////////////////////////////////////////////////

#include "logger.h"
#include "ToolkitShaders.h"

/*
===============================================================================
	PhongShader
===============================================================================
*/

/**
 * Constructor.
 * 
 * @param shaderNode XML project file 'shader' node.
 */
PhongShader::PhongShader(XMLNode * shaderNode) : Shader(shaderNode)
{
	
}

/**
 * Set shader parameters.
 * 
 * @param params Pointer to a shader parameters object to load phong shader parameters from.
 */
void PhongShader::SetParameters(ShaderParams * params)
{
	PhongShaderParams * phongParams = dynamic_cast<PhongShaderParams *>(params);
	if (!phongParams)
	{
		logger->Error("PhongShader.SetParameters", "Recieved incorrect shader parameters.");
		return;
	}

	// Setup light

	int sizeOfLightVector = 4;

	GLfloat *lightPos = new GLfloat[sizeOfLightVector];
	glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glUniform4fv(GetUniformLoc( this->program[this->currentPass], "LightPosition" ), 1, lightPos);

	// Setup textures
	
	bool validTextureUnit = this->ValidateTextureUnit(phongParams->texUnitBase);

	if (validTextureUnit == true)
	{
		int samplerUniformLoc;
		samplerUniformLoc = GetUniformLoc(this->program[this->currentPass], "Base" );
		glUniform1i(samplerUniformLoc, phongParams->texUnitBase);
	}

	// Setup variables

	int variableUniformLoc;
	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "HalfWayApprox" );
	glUniform1i(variableUniformLoc, phongParams->halfWayApprox);
}

/*
===============================================================================
	BumpShader
===============================================================================
*/

/**
 * Constructor.
 * 
 * @param shaderNode XML project file 'shader' node.
 */
BumpShader::BumpShader(XMLNode * shaderNode) : Shader(shaderNode)
{
	
}

/**
 * Set shader parameters.
 * 
 * @param params Pointer to a shader parameters object to load bump shader parameters from.
 */
void BumpShader::SetParameters(ShaderParams * params)
{
	BumpShaderParams * bumpParams = dynamic_cast<BumpShaderParams *>(params);
	if (!bumpParams)
	{
		logger->Error("BumpShader.SetParameters", "Recieved incorrect shader parameters.");
		return;
	}

	// Setup light
	//int sizeOfLightVector = 4;
	int sizeOfLightVector = 3;

	GLfloat *lightPos = new GLfloat[sizeOfLightVector];
	glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	//glUniform4fv(GetUniformLoc( this->program[this->currentPass], "LightPosition" ), 1, lightPos);
	glUniform3fv(GetUniformLoc( this->program[this->currentPass], "LightPosition" ), 1, lightPos);

	// Setup textures
	
	bool validTextureUnits = this->ValidateTextureUnit(bumpParams->texUnitBase) &&
							 this->ValidateTextureUnit(bumpParams->texUnitHeight);

	if (validTextureUnits == true)
	{
		int samplerUniformLoc;
		samplerUniformLoc = GetUniformLoc(this->program[this->currentPass], "base" );
		glUniform1i(samplerUniformLoc, bumpParams->texUnitBase);

		samplerUniformLoc = GetUniformLoc(this->program[this->currentPass], "height" );
		glUniform1i(samplerUniformLoc, bumpParams->texUnitHeight);
	}
}

/*
===============================================================================
	TissueShader
===============================================================================
*/

/**
 * Constructor.
 * 
 * @param shaderNode XML project file 'shader' node.
 */
TissueShader::TissueShader(XMLNode * shaderNode) : Shader(shaderNode)
{
	
}

/**
 * Set shader parameters.
 * 
 * @param params Pointer to a shader parameters object to load tissue shader parameters from.
 */
void TissueShader::SetParameters(ShaderParams * params)
{

	TissueShaderParams * tissueParams = dynamic_cast<TissueShaderParams *>(params);
	if (!tissueParams)
	{
		logger->Error("TissueShader.SetParameters", "Recieved incorrect shader parameters.");
		return;
	}

	// Setup light

	int sizeOfLightVector = 4;

	GLfloat *lightPos = new GLfloat[sizeOfLightVector];
	glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glUniform4fv(GetUniformLoc(this->program[this->currentPass], "LightPosition" ), 1, lightPos);

	// Setup textures

	
	bool validTextureUnits = this->ValidateTextureUnit(tissueParams->texUnitBase) &&
							 this->ValidateTextureUnit(tissueParams->texUnitHeight);

	if (validTextureUnits == true)
	{
		int  samplerUniformLoc;
		samplerUniformLoc = GetUniformLoc(this->program[this->currentPass], "Base" );
		glUniform1i(samplerUniformLoc, tissueParams->texUnitBase);

		samplerUniformLoc = GetUniformLoc(this->program[this->currentPass], "Height" );
		glUniform1i(samplerUniformLoc, tissueParams->texUnitHeight);
	}

	// Setup variables

	int variableUniformLoc;
	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "AmbientContribution" );
	glUniform1f(variableUniformLoc, tissueParams->ambientContribution);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "DiffuseContribution" );
	glUniform1f(variableUniformLoc, tissueParams->diffuseContribution);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "SpecularContribution" );
	glUniform1f(variableUniformLoc, tissueParams->specularContribution);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "Glossiness" );
	glUniform1f(variableUniformLoc, tissueParams->glossiness);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "StepSize" );
	glUniform1f(variableUniformLoc, tissueParams->stepSize);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "Bumpiness" );
	glUniform1f(variableUniformLoc, tissueParams->bumpiness);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "Opacity" );
	glUniform1f(variableUniformLoc, tissueParams->opacity);

	variableUniformLoc = GetUniformLoc(this->program[this->currentPass], "Displacement" );
	glUniform1f(variableUniformLoc, tissueParams->displacement);
}
