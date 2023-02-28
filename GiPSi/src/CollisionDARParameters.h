/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Collision Detection and Response Parameters Definition (CollisionDARParameters.h).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	COLLISIONDARPARAMETERS.H v0.0
////
////	Collision Detection and Response Parameters
////
////////////////////////////////////////////////////////////////

#ifndef _COLLISION_DAR_PARAMETERS_H_
#define _COLLISION_DAR_PARAMETERS_H_

#include "CollisionDARLoader.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

class LoaderUnitTest;

class CollisionDARParameters
{
public:
	CollisionDARParameters(XMLNode * collisionDARNode, CollisionDARLoader * CDARLoader);
	~CollisionDARParameters();

	CollisionMethodParameters * collisionMethodParameters;
	CollisionMethodParameters * hapticCollisionMethodParameters;

protected:
	friend LoaderUnitTest;
};

#endif