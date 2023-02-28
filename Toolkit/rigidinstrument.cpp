/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Rigid Probe HIO Implementation (probe.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	rigidinstrument.cpp v1.0
////
////	Implements the rigid instrument Haptic Interface Object
////
////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "rigidinstrument.h"
#include "algebra.h"
#include "errors.h"
#include "GiPSiException.h"
#include "XMLNodeList.h"

RigidInstrument::RigidInstrument(XMLNode * simObjectNode) :
	HapticInterfaceObject(simObjectNode)
{
	try
	{
		// Extract initialization information
		if (simObjectNode == NULL)
		{
			char location[256]("");
			sprintf_s(location, 256, "%s constructor", GetName());
			throw new GiPSiException(location, "Empty simObject node recieved.");
			return;
		}
		XMLNodeList * simObjectChildren = simObjectNode->GetChildren();

		// Initialize visualization
		InitializeVisualization(simObjectChildren);
	}
	catch (...)
	{
		throw;
		return;
	}
}


//////////////////////////////////////
//
//	RigidInstrument::InitializeVisualization()
//
//		Initialize visualization parameters
//
void RigidInstrument::InitializeVisualization(XMLNodeList * simObjectChildren)
{
	try
	{
		// Extract baseColor
		XMLNode * visualizationNode = simObjectChildren->GetNode("visualization");
		XMLNodeList * visualizationChildren = visualizationNode->GetChildren();

		// Extract baseColor
		XMLNode * baseColorNode = visualizationChildren->GetNode("baseColor");
		XMLNodeList * baseColorChildren = baseColorNode->GetChildren();

		// Extract red
		XMLNode * redNode = baseColorChildren->GetNode("red");
		const char * red = redNode->GetValue();

		// Extract green
		XMLNode * greenNode = baseColorChildren->GetNode("green");
		const char * green = greenNode->GetValue();

		// Extract blue
		XMLNode * blueNode = baseColorChildren->GetNode("blue");
		const char * blue = blueNode->GetValue();

		// Extract opacity
		XMLNode * opacityNode = baseColorChildren->GetNode("opacity");
		const char * opacity = opacityNode->GetValue();

		// Initialize HeartMuscleGeometry using color information
		initialcolor[0] = (float)atof(red);
		initialcolor[1] = (float)atof(green);
		initialcolor[2] = (float)atof(blue);
		initialcolor[3] = (float)atof(opacity);
		delete red;
		delete green;
		delete blue;
		delete opacity;
	}
	catch (...)
	{
		throw;
		return;
	}
}