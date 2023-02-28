/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit Shader Params Definition (ToolkitShaderParams.h).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	TOOLKITSHADERPARAMS.H v0.0
////
////	Toolkit Shader Params
////
////////////////////////////////////////////////////////////////

#ifndef _TOOLKIT_SHADER_PARAMS_H_
#define _TOOLKIT_SHADER_PARAMS_H_

#include "GiPSiException.h"
#include "GiPSiDisplay.h"
#include "XMLNode.h"
#include "XMLNodeList.h"

using namespace GiPSiXMLWrapper;

enum ShaderName
{
	ShaderName_Phong,
	ShaderName_Bump,
	ShaderName_Tissue
};

class PhongShaderParams : public ShaderParams {
public:
	PhongShaderParams(XMLNode * phongParamsNode);

	virtual ShaderName GetShaderName() { return ShaderName_Phong; }
	virtual char GetShaderAttributes() { return 0x00; }

	bool halfWayApprox;
	int texUnitBase;
};

class BumpShaderParams : public ShaderParams {
public:
	BumpShaderParams(XMLNode * bumpParamsNode);

	virtual ShaderName GetShaderName() { return ShaderName_Bump; }
	virtual char GetShaderAttributes() { return 0x18; }

	int texUnitBase;
	int texUnitHeight;
};

class TissueShaderParams : public ShaderParams {
public:
	TissueShaderParams(XMLNode * tissueParamsNode);

	virtual ShaderName GetShaderName() { return ShaderName_Tissue; }
	virtual char GetShaderAttributes() { return 0x18; }

	int	texUnitBase;
	int	texUnitHeight;
	float ambientContribution;
	float diffuseContribution;
	float specularContribution;
	float glossiness;
	float stepSize;
	float bumpiness;
	float opacity;
	float displacement;
};

#endif