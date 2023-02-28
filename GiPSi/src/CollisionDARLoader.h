/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Collision Detection and Response Parameters Definition (CollisionDARLoader.h).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	COLLISIONDARLOADER.H v0.0
////
////	Collision Detection and Response Loader
////
////////////////////////////////////////////////////////////////

#ifndef _COLLISION_DAR_LOADER_H_
#define _COLLISION_DAR_LOADER_H_

#include "CollisionMethodParameters.h"
#include "CollisionDetectionParameters.h"
#include "CollisionResponseParameters.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

/**
 * The CollisionDARLoader class is an abstract class from which
 * application-specific CollisionDARLoaders should derived.  It
 * is responsible
 */

class CollisionDARLoader
{
public:
	/**
	 * Load collision detection parameters.
	 * 
	 * @param colDetectionNode Project file XML 'colDetection' node.
	 */
	virtual CollisionDetectionParameters * LoadCollisionDetectionParameters(XMLNode * colDetectionNode) = 0;
	/**
	 * Load collision reponse parameters.
	 * 
	 * @param colResponseNode Project file XML 'colResponse' node.
	 */
	virtual CollisionResponseParameters * LoadCollisionResponseParameters(XMLNode * colResponseNode) = 0;
};

#endif