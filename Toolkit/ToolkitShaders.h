/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit Shaders (ToolkitShaders.h).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	TOOLKITSHADERS.H v0.0
////
////	Toolkit Shaders
////
////////////////////////////////////////////////////////////////

#ifndef _TOOLKIT_SHADERS_H_
#define _TOOLKIT_SHADERS_H_

#include "GiPSiShader.h"
#include "ToolkitShaderParams.h"

/*
===============================================================================
	Phong shader
===============================================================================
*/

class PhongShader : public Shader
{
public:
	PhongShader(XMLNode * shaderNode);

	virtual ShaderName	GetShaderName(void) { return ShaderName_Phong; }

	virtual void		SetParameters(ShaderParams * params);

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
	BumpShader(XMLNode * shaderNode);

	virtual ShaderName	GetShaderName(void) { return ShaderName_Bump; }

	virtual void		SetParameters(ShaderParams * params);

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
	TissueShader(XMLNode * shaderNode);

	virtual ShaderName	GetShaderName(void) { return ShaderName_Tissue; }

	virtual void		SetParameters(ShaderParams * params);

protected:

private:

};

#endif