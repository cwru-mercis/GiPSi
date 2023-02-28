/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Light Definition (GiPSiLight.h).

The Initial Developer of the Original Code is John Pao.  
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	GIPSILIGHT.H v0.0
////
////	GiPSi Light definition
////
////////////////////////////////////////////////////////////////

#ifndef _GIPSI_LIGHT_H_
#define _GIPSI_LIGHT_H_

#include "algebra.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

enum LightTypeEnum
{
	FREE,
	ATTACHED
};

class Camera;

class Light
{
public:
	Light(XMLNode * lightNode, Camera ** camera, int nCamera);

	Vector<Real> pos, dir, color;

	LightTypeEnum type;
	Camera * attached;
};

#endif
