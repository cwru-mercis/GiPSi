/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit Collision Detection and Response Loader Implementation (ToolkitCollisionDARLoader.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao, Suriya Natsupakpong.
*/

////	TOOLKITCOLLISIONDARLOADER.CPP v0.0
////
////	Toolkit Collision Detection and Response Loader
////
////////////////////////////////////////////////////////////////

#include "CollisionDetectionParameters.h"
#include "CollisionResponseParameters.h"
#include "ToolkitCollisionDARLoader.h"
#include "ToolkitCollisionDARParams.h"
#include "XMLNodeList.h"

/**
 * Load collision detection parameters.
 * 
 * @param colDetectionNode Project file XML 'colDetection' node.
 */
CollisionDetectionParameters * ToolkitCollisionDARLoader::LoadCollisionDetectionParameters(XMLNode * colDetectionNode)
{
	try
	{
		CollisionDetectionParameters * collisionDetectionParameters = NULL;
		const char * CDMethodName = "";
		if (colDetectionNode->GetNumChildren() > 0)
		{
			XMLNodeList * colDetectionChildren = colDetectionNode->GetChildren();
			XMLNode * parametersNode = colDetectionChildren->GetNode((unsigned int)0);
			CDMethodName = parametersNode->GetName();
			if (strcmp(CDMethodName, "TriTri") == 0)
			{
				collisionDetectionParameters = new TriTriCDParameters(parametersNode);
			}
			else if (strcmp(CDMethodName, "TriLine") == 0)
			{
				collisionDetectionParameters = new TriLineCDParameters(parametersNode);
			}
			else if (strcmp(CDMethodName, "AABB") == 0)
			{
				collisionDetectionParameters = new AABBCDParameters(parametersNode);
			}			
			else
			{
				collisionDetectionParameters = new CollisionDetectionParameters();
			}
			delete parametersNode;
			delete colDetectionChildren;
		}
		return collisionDetectionParameters;
	}
	catch (...)
	{
		throw;
		return new CollisionDetectionParameters();
	}
}

/**
 * Load collision reponse parameters.
 * 
 * @param colResponseNode Project file XML 'colResponse' node.
 */
CollisionResponseParameters * ToolkitCollisionDARLoader::LoadCollisionResponseParameters(XMLNode * colResponseNode)
{
	try
	{
		CollisionResponseParameters * collisionResponseParameters = NULL;
		const char * CRMethodName = "";
		if (colResponseNode->GetNumChildren() > 0)
		{
			XMLNodeList * colResponseChildren = colResponseNode->GetChildren();
			XMLNode * parametersNode = colResponseChildren->GetNode((unsigned int)0);

			CRMethodName = parametersNode->GetName();

			if (strcmp(CRMethodName, "Haptic") == 0)
			{
				collisionResponseParameters = new HapticCRParameters(parametersNode);
			}
			else if (strcmp(CRMethodName, "PDepth") == 0)
			{
				collisionResponseParameters = new PDepthCRParameters(parametersNode);
			}
			else
			{
				collisionResponseParameters = new CollisionResponseParameters();
			}

			delete parametersNode;
			delete colResponseChildren;
		}

		return collisionResponseParameters;
	}
	catch (...)
	{
		throw;
		return new CollisionResponseParameters();
	}
}
