/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Light Implementation (GiPSiLight.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	GIPSILIGHT.CPP v0.0
////
////	GiPSi Light implementation
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include "GiPSiCamera.h"
#include "GiPSiException.h"
#include "GiPSiLight.h"
#include "XMLNodeList.h"

/*
===============================================================================
	Light class
===============================================================================
*/

/**
 * Constructor.
 * 
 * @param lightNode XML project "Light" node for initialization.
 * @param camera Array of all cameras in the visualization engine.
 * @param nCamera The number of cameras in the camera array.
 */
Light::Light(GiPSiXMLWrapper::XMLNode *lightNode, Camera ** camera, int nCamera) :
	attached(NULL)
{
	try
	{
		// Set camera parameters
		XMLNodeList * lightChildren = lightNode->GetChildren();

		// type
		XMLNode * typeNode = lightChildren->GetNode("type");
		char * typeVal = typeNode->GetValue();
		if (strcmp(typeVal, "FREE") == 0)
			type = FREE;
		else if (strcmp(typeVal, "ATTACHED") == 0)
			type = ATTACHED;
		else
		{
			int errlen = strlen(typeVal) + 28;
			char * err = new char[errlen];
			sprintf_s(err, errlen, "Unknown light type found: %s", typeVal);
			throw new GiPSiException("Light constructor", err);
			delete err;
			delete typeVal;
			delete typeNode;
		}
		delete typeVal;
		delete typeNode;

		// position
		XMLNode * positionNode = lightChildren->GetNode("position");
		XMLNodeList * positionChildren = positionNode->GetChildren();
		XMLNode * posxNode = positionChildren->GetNode("x");
		XMLNode * posyNode = positionChildren->GetNode("y");
		XMLNode * poszNode = positionChildren->GetNode("z");
		const char * posx = posxNode->GetValue();
		const char * posy = posyNode->GetValue();
		const char * posz = poszNode->GetValue();
		pos			= Vector<Real>(3);
		pos[0]		= (Real)atof(posx);
		pos[1]		= (Real)atof(posy);
		pos[2]		= (Real)atof(posz);
		delete posxNode;
		delete posyNode;
		delete poszNode;
		delete positionChildren;
		delete positionNode;
		delete posx;
		delete posy;
		delete posz;

		// direction
		XMLNode * directionNode = lightChildren->GetNode("direction");
		XMLNodeList * directionChildren = directionNode->GetChildren();
		XMLNode * dirxNode = directionChildren->GetNode("x");
		XMLNode * diryNode = directionChildren->GetNode("y");
		XMLNode * dirzNode = directionChildren->GetNode("z");
		const char * dirx = dirxNode->GetValue();
		const char * diry = diryNode->GetValue();
		const char * dirz = dirzNode->GetValue();
		dir			= Vector<Real>(3);
		dir[0]		= (Real)atof(dirx);
		dir[1]		= (Real)atof(diry);
		dir[2]		= (Real)atof(dirz);
		delete dirxNode;
		delete diryNode;
		delete dirzNode;
		delete directionChildren;
		delete directionNode;
		delete dirx;
		delete diry;
		delete dirz;

		// color
		XMLNode * colorNode = lightChildren->GetNode("color");
		XMLNodeList * colorChildren = colorNode->GetChildren();
		XMLNode * colorrNode = colorChildren->GetNode("red");
		XMLNode * colorgNode = colorChildren->GetNode("green");
		XMLNode * colorbNode = colorChildren->GetNode("blue");
		XMLNode * coloraNode = colorChildren->GetNode("opacity");
		const char * colorr = colorrNode->GetValue();
		const char * colorg = colorgNode->GetValue();
		const char * colorb = colorbNode->GetValue();
		const char * colora = coloraNode->GetValue();
		color		= Vector<Real>(4);
		color[0]	= (Real)atof(colorr);
		color[1]	= (Real)atof(colorg);
		color[2]	= (Real)atof(colorb);
		color[3]	= (Real)atof(colora);
		delete colorrNode;
		delete colorgNode;
		delete colorbNode;
		delete coloraNode;
		delete colorChildren;
		delete colorNode;
		delete colorr;
		delete colorg;
		delete colorb;
		delete colora;

		// Attach to specified camera, if applicable
		if (type == 1)
		{
			XMLNode * attachedCameraNameNode = lightChildren->GetNode("attachedCameraName");
			char * cameraName = attachedCameraNameNode->GetValue();

			attached = NULL;
			for (int i = 0; i < nCamera; i++)
			{
				if (strcmp(camera[i]->GetName(), cameraName) == 0)
				{
					// Store the attached camera as a member.
					attached = camera[i];
					break;
				}
			}
			if (attached == NULL)
			{
				int errlen = strlen(typeVal) + 28;
				char * err = new char[errlen];
				sprintf_s(err, errlen, "Unknown camera name found: %s", typeVal);
				throw new GiPSiException("Light constructor", err);
				delete err;
				delete cameraName;
				delete attachedCameraNameNode;
				delete lightChildren;
				return;
			}

			delete cameraName;
			delete attachedCameraNameNode;
		}

		// Make this light source visible to only specified cameras
		XMLNode * cameraNamesNode = lightChildren->GetNode("cameraNames");
		XMLNodeList * cameraNamesChildren = cameraNamesNode->GetChildren();
		XMLNode * cameraName1Node = cameraNamesChildren->GetNode((unsigned int)0);
		if (strcmp(cameraName1Node->GetValue(), "ALL") == 0)
		{
			// The ALL keyword makes this light visible to all cameras
			for (int i = 0; i < nCamera; i++)
			{
				camera[i]->AddLight(this);
			}
		}
		else
		{
			// Give knowledge of this light only to the named cameras
			for (unsigned int i = 0; i < cameraNamesChildren->GetLength(); i++)
			{
				XMLNode * cameraNameNode = cameraNamesChildren->GetNode(i);
				char * cameraName = cameraNameNode->GetValue();
				for (int j = 0; j < nCamera; j++)
				{
					if (strcmp(cameraName, camera[j]->GetName()) == 0)
					{
						// Add this light source to the camera with the matching name
						camera[j]->AddLight(this);
						break;
					}
				}
				if (i == nCamera)
				{
					// If the named camera wasn't found, throw an exception
					int errlen = strlen(cameraName) + 28;
					char * err = new char[errlen];
					sprintf_s(err, errlen, "Unknown camera name found: %s", cameraName);
					throw new GiPSiException("Light constructor", err);
					delete err;
					delete cameraName;
					delete cameraNameNode;
					delete cameraNamesNode;
					delete cameraNamesChildren;
					delete cameraName1Node;
					delete lightChildren;
					return;
				}
				delete cameraName;
				delete cameraNameNode;
			}
		}
		delete cameraNamesNode;
		delete cameraNamesChildren;
		delete cameraName1Node;
		delete lightChildren;
	}
	catch (...)
	{
		throw;
		return;
	}
}
