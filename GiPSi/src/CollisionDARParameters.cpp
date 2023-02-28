/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Collision Detection and Response Parameters Implementation (CollisionDARParameters.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	COLLISIONDARPARAMETERS.CPP v0.0
////
////	Collision Detection and Response Parameters
////
////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include "CollisionDARParameters.h"
#include "GiPSiException.h"
#include "XMLNodeList.h"

/**
 * Constructor.
 * 
 * @param collisionDARNode 'collisionDAR' XML project file node.
 * @param CDARLoader Pointer to the CollisionDARLoader to use.
 */
CollisionDARParameters::CollisionDARParameters(XMLNode * collisionDARNode, CollisionDARLoader * CDARLoader)
{
	try
	{
		XMLNodeList * collisionDARChildren = collisionDARNode->GetChildren();		

		// Collision Method 
		XMLNode * collisionMethodNode = collisionDARChildren->GetNode("collisionMethod");
		collisionMethodParameters = new CollisionMethodParameters(collisionMethodNode);
		delete collisionMethodNode;

		// Haptic Collision Method 
		XMLNode * hapticCollisionMethodNode = collisionDARChildren->GetNode("hapticCollisionMethod");
		hapticCollisionMethodParameters = new CollisionMethodParameters(hapticCollisionMethodNode);
		delete hapticCollisionMethodNode;

		delete collisionDARChildren;
	}
	catch (...)
	{
		throw;
		return;
	}
}

/**
 * Destructor.
 */
CollisionDARParameters::~CollisionDARParameters()
{
	if (collisionMethodParameters) {
		delete collisionMethodParameters;
		collisionMethodParameters = NULL;
	}
	if (hapticCollisionMethodParameters) {
		delete hapticCollisionMethodParameters;
		hapticCollisionMethodParameters = NULL;
	}
}