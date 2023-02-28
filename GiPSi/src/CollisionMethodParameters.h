/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Collision Method Parameters Definition (CollisionMethodParameters.h).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao, Suriya Natsupakpong.
*/

////	COLLISIONMETHODPARAMETERS.H v0.0
////
////	Collision Method Parameters
////
////////////////////////////////////////////////////////////////

#ifndef _COLLISION_METHOD_PARAMETERS_H_
#define _COLLISION_METHOD_PARAMETERS_H_

#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

class LoaderUnitTest;

class CollisionMethodParameters
{
public:
	CollisionMethodParameters(XMLNode * collisionMethodNode);
	~CollisionMethodParameters();

	virtual const char	*GetMethodName(void);
	void				SetEnabled(bool enabled);
	bool				GetEnabled(void);

	const char			*Getfilename(void) { return formattedFileName; }
	const char			*GetCollisionRule(void) { return collisionRuleName; }

protected:
	bool				Enabled;
	char				*collisionMethodName;
	char				*formattedFileName;
	char				*collisionRuleName;

	friend LoaderUnitTest;
};

#endif