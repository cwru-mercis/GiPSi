/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Collision Method Parameters Implementation (CollisionMethodParameters.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao, Suriya Natsupakpong.
*/

////	COLLISIONMETHODPARAMETERS.CPP v0.0
////
////	Collision Method Parameters
////
////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include "CollisionMethodParameters.h"
#include "GiPSiException.h"
#include "XMLNodeList.h"

/**
 * Constructor.
 * 
 * @param collisionMethodNode 'collisionMethod' XML project file node.
 * @param CDARLoader Pointer to the CollisionDARLoader to use.
 */
CollisionMethodParameters::CollisionMethodParameters(XMLNode * collisionMethodNode)
{
	try
	{
		XMLNodeList * collisionMethodChildren = collisionMethodNode->GetChildren();
		
		// Collision method name
		XMLNode * methodNode = collisionMethodChildren->GetNode("method");		
		const char * methodName = methodNode->GetValue();		
		delete methodNode;
		collisionMethodName = new char[strlen(methodName) + 1];
		sprintf_s(collisionMethodName, strlen(methodName) + 1, "%s", methodName);

		// Collision rule name
		XMLNode * ruleNode = collisionMethodChildren->GetNode("enabled");		
		const char * ruleName = ruleNode->GetValue();		
		delete ruleNode;
		collisionRuleName = new char[strlen(ruleName) + 1];
		sprintf_s(collisionRuleName, strlen(ruleName) + 1, "%s", ruleName);
		if (strcmp(collisionRuleName, "FOR_NONE") == 0)
		{
			Enabled = false;
		}
		else 
		{
			Enabled = true;
		}
		delete ruleName;
		
		// Format file name for all case except "FOR_NONE" and "FOR_ALL"
		if ((strcmp(collisionRuleName, "FOR_NONE") != 0) && (strcmp(collisionRuleName, "FOR_ALL") != 0))
		{
			XMLNode * pathNode = collisionMethodChildren->GetNode("path");
			const char * path = pathNode->GetValue();
			XMLNode * fileNameNode = collisionMethodChildren->GetNode("fileName");
			const char * fileName = fileNameNode->GetValue();
			delete fileNameNode;
			delete pathNode;

			formattedFileName = new char[strlen(path) + strlen(fileName) + 4];
			sprintf_s(formattedFileName, strlen(path) + strlen(fileName) + 4, ".\\%s\\%s", path, fileName);
			delete path;
			delete fileName;
		}
		
		delete collisionMethodChildren;
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
CollisionMethodParameters::~CollisionMethodParameters()
{

	if (formattedFileName) {
		delete formattedFileName;
		formattedFileName = NULL;
	}
	if (collisionRuleName) {
		delete collisionRuleName;
		collisionRuleName = NULL;
	}
	if (collisionMethodName) {
		delete collisionMethodName;
		collisionMethodName = NULL;
	}
}

/**
 * Get the name of the collision method method.
 */
const char * CollisionMethodParameters::GetMethodName(void)
{
	return collisionMethodName;
}

/**
 * The SetEnabled method takes a bool value and sets or resets the enabled
 * flag based on its value.
 */
void CollisionMethodParameters::SetEnabled(bool enabled)
{
	Enabled = enabled;
}

/*
 * The GetEnabled method returns the state of the enabled flag.
 */
bool CollisionMethodParameters::GetEnabled(void)
{
	return Enabled;
}
