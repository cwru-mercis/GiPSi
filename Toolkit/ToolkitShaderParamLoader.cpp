/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit Shader Param Loader Implementation (ToolkitShaderParamLoader.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	TOOLKITSHADERPARAMLOADER.CPP v0.0
////
////	Toolkit Shader Param Loader
////
////////////////////////////////////////////////////////////////

#include "GiPSiException.h"
#include "GiPSiShader.h"
#include "logger.h"
#include "ToolkitShaderParamLoader.h"
#include "ToolkitShaderParams.h"
#include "XMLNodeList.h"

/**
 * Load shader parameters object.
 * 
 * @param shaderNode Project file XML 'shader' node.
 */
ShaderParams * ToolkitShaderParamLoader::LoadShaderParams(XMLNode * shaderNode)
{
	try
	{
		ShaderParams * params = NULL;
		if (shaderNode->GetNumChildren() > 0)
		{
			// translate shader name
			XMLNodeList * shaderChildren = shaderNode->GetChildren();
			XMLNode * shaderParamsNode = shaderChildren->GetNode("params");
			XMLNode * shaderNameNode = shaderChildren->GetNode("name");
			char * name = shaderNameNode->GetValue();

			if (strcmp(name, "phong") == 0)
				params = new PhongShaderParams(shaderParamsNode);
			else if (strcmp(name, "bump") == 0)
				params = new BumpShaderParams(shaderParamsNode);
			else if (strcmp(name, "tissue") == 0)
				params = new TissueShaderParams(shaderParamsNode);
			else
			{
				throw new GiPSiException("ToolkitShaderParamLoader.LoadShaderParams", "Unrecognized shader name found.");
			}

			delete name;
			delete shaderChildren;
			delete shaderParamsNode;
			delete shaderNameNode;
		}

		return params;
	}
	catch (...)
	{
		throw;
		return NULL;
	}
}