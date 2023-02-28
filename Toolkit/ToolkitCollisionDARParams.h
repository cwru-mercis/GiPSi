/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit Collision DAR Params Definition (ToolkitCollisionDARParams.h).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao, Suriya Natsupakpong.
*/

////	TOOLKITCOLLISIONDARPARAMS.H v0.0
////
////	Toolkit Collision DAR Params
////
////////////////////////////////////////////////////////////////

#ifndef _TOOLKIT_COLLISIONDAR_PARAMS_H_
#define _TOOLKIT_COLLISIONDAR_PARAMS_H_

#include "GiPSiException.h"
#include "GiPSiDisplay.h"
#include "XMLNode.h"
#include "XMLNodeList.h"
#include "algebra.h"
#include "CollisionDetectionParameters.h"
#include "CollisionResponseParameters.h"

using namespace GiPSiXMLWrapper;

enum CollisionDetectionName
{
	CollisionDetectionName_TriTri,
	CollisionDetectionName_TriLine,
	CollisionDetectionName_AABB
};

enum CollisionResponseName
{
	CollisionResponseName_Haptic,
	CollisionResponseName_PDepth	
};

class TriTriCDParameters : public CollisionDetectionParameters
{
public:
	TriTriCDParameters(XMLNode * parametersNode);

	virtual const char * GetMethodName();

protected:
	Real colThreshold;
	friend LoaderUnitTest;
};

class TriLineCDParameters : public CollisionDetectionParameters
{
public:
	TriLineCDParameters(XMLNode * parametersNode);

	virtual const char * GetMethodName();

protected:

	friend LoaderUnitTest;
};

class AABBCDParameters : public CollisionDetectionParameters
{
public:
	AABBCDParameters(XMLNode * parametersNode);

	virtual const char * GetMethodName();

protected:

	friend LoaderUnitTest;
};

class HapticCRParameters : public CollisionResponseParameters
{
public:
	HapticCRParameters(XMLNode * parametersNode);

	virtual const char * GetMethodName();

protected:

	friend LoaderUnitTest;
};

class PDepthCRParameters : public CollisionResponseParameters
{
public:
	PDepthCRParameters(XMLNode * parametersNode);

	virtual const char * GetMethodName();

protected:

	friend LoaderUnitTest;
};

#endif