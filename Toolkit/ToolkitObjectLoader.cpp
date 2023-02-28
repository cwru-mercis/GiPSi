/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit Object Loader Implementation (ToolkitObjectLoader.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao, Suriya Natsupakpong.
*/

////	TOOLKITOBJECTLOADER.CPP v0.0
////
////	Toolkit Object Loader
////
////////////////////////////////////////////////////////////////

#include "bioe.h"
#include "collisiontest.h"
#include "fem.h"
#include "GiPSiException.h"
#include "lumpedfluid.h"
#include "probe.h"
#include "msd.h"
#include "simple.h"
#include "qsds.h"
#include "catheter.h"
#include "balloon.h"
#include "ToolkitObjectLoader.h"
#include "XMLNodeList.h"

/**
 * Load simulation object.
 * 
 * @param simObjectNode Project file XML 'simObj' node.
 */
SIMObject * ToolkitObjectLoader::LoadObject(XMLNode * simObjectNode, Real g)
{
	try
	{
		XMLNodeList * simObjectChildren = simObjectNode->GetChildren();
		XMLNode * typeNode = simObjectChildren->GetNode("type");
		const char * type = typeNode->GetValue();

		SIMObject * newSimObject = NULL;
		if (strcmp(type, "TEST") == 0)
		{
			newSimObject = new SimpleTestObject(simObjectNode);
		}
		else if (strcmp(type, "FEM") == 0)
		{
			newSimObject = new FEM_3LMObject(simObjectNode, g);
		}
		else if (strcmp(type, "MSD") == 0)
		{
			newSimObject = new MSDObject(simObjectNode, g);
		}
		else if (strcmp(type, "CBE") == 0)
		{
			newSimObject = new CardiacBioEObject(simObjectNode);
		}
		else if (strcmp(type, "LF") == 0)
		{
			newSimObject = new LumpedFluidObject(simObjectNode);
		}
		else if (strcmp(type, "PHIO") == 0)
		{
			newSimObject = new RigidProbeHIO(simObjectNode);
		}
		else if (strcmp(type, "CT") == 0)		// Collision Test
		{
			newSimObject = new CollisionTestObject(simObjectNode);
		}
		else if (strcmp(type, "QSDS") == 0)	
		{
			newSimObject = new QSDSObject(simObjectNode);
		}
		else if (strcmp(type, "MHIO") == 0)		// Mouse Haptic Interface Object
		{
			newSimObject = new CatheterHIO(simObjectNode);
		}
		else if (strcmp(type, "BALLOON") == 0)	// Balloon Object
		{
			newSimObject = new BalloonObject(simObjectNode);
		}
		else
		{
			char error[256]("");
			sprintf_s(error, 256, "Unrecognized object type %s", type);
			throw new GiPSiException("ObjectLoader", error);
			return NULL;
		}
		delete type;
		delete typeNode;

		// We load the shader params afterwards, since they are independent of the object type
		XMLNode * visualizationNode = simObjectChildren->GetNode("visualization");
		XMLNodeList * visualizationChildren = visualizationNode->GetChildren();
		XMLNode * shaderNode = visualizationChildren->GetNode("shader");
		ShaderParams * shaderParams = shaderParamLoader->LoadShaderParams(shaderNode);
		newSimObject->displayMngr->SetShaderParams(shaderParams);
		delete shaderNode;
		delete visualizationChildren;
		delete visualizationNode;
		delete simObjectChildren;

		return newSimObject;
	}
	catch(...)
	{
		throw;
		return NULL;
	}
}