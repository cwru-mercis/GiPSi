/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Source for GiPSi XML Unit Test (XMLUnitTest.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	XMLUNITTEST.CPP v0.0
////
////	Source for GiPSi XML Unit Test
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include <stdio.h>
#include <string.h>

#include "Logger.h"

#include "XMLUnitTest.h"
#include "XMLNode.h"
#include "XMLNodeList.h"
#include "XMLDocument.h"
#include "XMLDocumentBuilder.h"

/*
===============================================================================
	XMLUnitTest class
===============================================================================
*/

using namespace GiPSiXMLWrapper;

XMLUnitTest::XMLUnitTest()
{
	myFailedCount = 0;
}

void XMLUnitTest::Run()
{
	if (!logger)
		logger = new Logger();

	// Test 1 - Testing XMLDocumentBuilder.Build
	printf("1 - Testing XMLDocumentBuilder.Build\n");
	XMLDocumentBuilder builder;
	char * fileName = ".\\XMLFiles\\testXML.xml";
	XMLDocument * doc = builder.Build(fileName);
	if (doc == NULL)
	{
		myFailedCount++;
		printf("\tFailed\n");
	}
	else
		printf("\tPassed\n");

	// Test 2 - Testing XMLDocument.GetRootNode
	printf("2 - Testing XMLDocument.GetRootNode\n");
	XMLNode * root = doc->GetRootNode();
	const char * nodeName = root->GetName();
	char * targetName = "RootNode";
	if (strcmp(nodeName, targetName) != 0)
	{
		myFailedCount++;
		printf("\tFailed\n");
	}
	else
		printf("\tPassed\n");
	delete nodeName;

	// Test 3 - Testing XMLNode.GetChildren
	printf("3 - Testing XMLNode.GetChildren\n");
	XMLNodeList * rootList = root->GetChildren();
	if (rootList->GetLength() != 2)
	{
		myFailedCount++;
		printf("\tFailed\n");
	}
	else
		printf("\tPassed\n");

	// Test 4 - Testing XMLNodeList.GetNode(index)
	printf("4 - Testing XMLNodeList.GetNode(index)\n");
	XMLNode * child0 = rootList->GetNode((unsigned int)0);
	XMLNode * child1 = rootList->GetNode((unsigned int)1);
	if (child0 == NULL || child1 == NULL)
	{
		myFailedCount++;
		printf("\tFailed\n");
	}
	else
		printf("\tPassed\n");

	// Test 5 - Testing XMLNode.GetName
	printf("5 - Testing XMLNode.GetName\n");
	char * targetName0 = "RootLevelNode1";
	char * targetName1 = "RootLevelNode2";
	const char * childName0 = child0->GetName();
	const char * childName1 = child1->GetName();
	if (childName0 == NULL ||
		childName1 == NULL ||
		strlen(childName0) == 0 ||
		strlen(childName1) == 0 ||
		strcmp(childName0, targetName0) != 0 ||
		strcmp(childName1, targetName1) != 0)
	{
		myFailedCount++;
		printf("\tFailed\n");
	}
	else
		printf("\tPassed\n");
	delete childName0;
	delete childName1;

	// Test 6 - Testing XMLNodeList.GetNode(name)
	printf("6 - Testing XMLNodeList.GetNode(name)\n");
	child0 = rootList->GetNode(targetName0);
	child1 = rootList->GetNode(targetName1);
	if (child0 == NULL || child1 == NULL)
	{
		myFailedCount++;
		printf("\tFailed\n");
	}
	else
	{
		const char * childName0 = child0->GetName();
		const char * childName1 = child1->GetName();
		if (childName0 == NULL ||
			childName1 == NULL ||
			strlen(childName0) == 0 ||
			strlen(childName1) == 0 ||
			strcmp(childName0, targetName0) != 0 ||
			strcmp(childName1, targetName1) != 0)
		{
			myFailedCount++;
			printf("\tFailed\n");
		}
		else
			printf("\tPassed\n");
	}

	// Test 7 - Testing XMLNode.GetValue
	printf("7 - Testing XMLNode.GetValue\n");
	char * targetValue0 = NULL;
	char * targetValue1 = "Value3";
	try
	{
		const char * childValue0 = child0->GetValue();
		myFailedCount++;
		printf("\tFailed\n");
		delete childValue0;
	}
	catch (...)
	{
		const char * childValue1 = child1->GetValue();
		if (strcmp(childValue1, targetValue1) != 0)
		{
			myFailedCount++;
			printf("\tFailed\n");
		}
		else
			printf("\tPassed\n");
		delete childValue1;
	}

	// Cleanup
	delete child0;
	delete child1;
	delete rootList;
	delete root;
	delete doc;

	if (logger)
	{
		delete logger;
		logger = NULL;
	}
}