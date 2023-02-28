/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Object Loader Definition (ObjectLoader.h).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	OBJECTLOADER.H v0.0
////
////	Object Loader
////
////////////////////////////////////////////////////////////////

#ifndef _OBJECT_LOADER_H_
#define _OBJECT_LOADER_H_

#include "GiPSiAPI.h"
#include "ShaderParamLoader.h"

#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

class ObjectLoader
{
public:
	/**
	 * Constructor.
	 * 
	 * @param newShaderParamLoader Shader loader to use.
	 */
	ObjectLoader(ShaderParamLoader * newShaderParamLoader) : shaderParamLoader(newShaderParamLoader) {}
	/**
	 * Destructor.
	 */
	~ObjectLoader() { if (shaderParamLoader) delete shaderParamLoader; }

	/**
	 * Load simulation object.
	 * 
	 * @param simObjectNode Project file XML 'simObj' node.
	 */
	virtual SIMObject * LoadObject(XMLNode * simObjectNode, Real g=0.0) = 0;

protected:
	ShaderParamLoader * shaderParamLoader;
};

#endif
