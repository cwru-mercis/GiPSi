/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit Collision DAR Params Definition (ToolkitCollisionDARParams.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao, Suriya Natsupakpong.
*/

////	TOOLKITCOLLISIONDARPARAMS.CPP v0.0
////
////	Toolkit Collision DAR Params
////
////////////////////////////////////////////////////////////////


#include "ToolkitCollisionDARParams.h"

/**
 * Constructor.
 */
TriTriCDParameters::TriTriCDParameters(XMLNode * parametersNode)
{
	/*
	XMLNodeList * parameters = parametersNode->GetChildren();
	XMLNode * colThresholdNode = parameters->GetNode("colThreshold");

	colThreshold = (Real)atof(colThresholdNode->GetValue());
	*/
}

/**
 * Set the name of the collision detection method.
 */
const char * TriTriCDParameters::GetMethodName()
{
	return "TriTri";
}

/**
 * Constructor.
 */
TriLineCDParameters::TriLineCDParameters(XMLNode * parametersNode)
{
}

/**
 * Set the name of the collision detection method.
 */
const char * TriLineCDParameters::GetMethodName()
{
	return "TriLine";
}

/**
 * Constructor.
 */
AABBCDParameters::AABBCDParameters(XMLNode * parametersNode)
{
}

/**
 * Set the name of the collision detection method.
 */
const char * AABBCDParameters::GetMethodName()
{
	return "AABB";
}

/**
 * Constructor.
 */
HapticCRParameters::HapticCRParameters(XMLNode * parametersNode)
{
}

/**
 * Set the name of the collision detection method.
 */
const char * HapticCRParameters::GetMethodName()
{
	return "Haptic";
}

/**
 * Constructor.
 */
PDepthCRParameters::PDepthCRParameters(XMLNode * parametersNode)
{
}

/**
 * Set the name of the collision detection method.
 */
const char * PDepthCRParameters::GetMethodName()
{
	return "PDepth";
}