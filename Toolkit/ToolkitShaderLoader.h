/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit Shader Loader Definition (ToolkitShaderLoader.h).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	TOOLKITSHADERLOADER.H v0.0
////
////	Toolkit Shader Loader
////
////////////////////////////////////////////////////////////////

#ifndef _TOOLKIT_SHADER_LOADER_H_
#define _TOOLKIT_SHADER_LOADER_H_

#include "ShaderLoader.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

class ToolkitShaderLoader : public ShaderLoader
{
public:
	virtual Shader * LoadShader(XMLNode * shaderNode);
};

#endif
