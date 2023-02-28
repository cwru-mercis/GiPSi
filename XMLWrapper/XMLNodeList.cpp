/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi XMLNodeList Class Implementation (XMLNodeList.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	XMLNODELIST.CPP v0.0
////
////	XMLNodeList Class Implementation
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include "stdafx.h"

#include "GiPSiException.h"
#include "XMLNodeList.h"
#include "XMLNode.h"
#include "XMLUtils.h"

/*
===============================================================================
	XMLDocumentBuilder class
===============================================================================
*/

namespace GiPSiXMLWrapper
{
	/* Initialize member variables. */
	XMLNodeList::XMLNodeList(DOMNodeList * newNodeList)
	{
		if (newNodeList == NULL)
		{
			throw new GiPSiException("XMLNodeList constructor", "Recieved NULL node list.");
			return;
		}
		myNodeList = newNodeList;
	}
	/* Clean up any allocated memory. */
	XMLNodeList::~XMLNodeList()
	{
		// Don't delete the NodeList here; it's taken care of when the parser is deleted
	}

	/* The GetNode (index) method takes an integer index as
	input and returns an XMLNode containing a Node from the
	stored NodeList at that position.  If node such Node exists
	(ie – the index is out of range), a NULL is returned. */
	XMLNode * XMLNodeList::GetNode(unsigned int index) const
	{
		try
		{
			if (index > GetLength())
			{
				throw new GiPSiException("XMLNodeList.GetNode(index)", "Could not get node: Index is out of range.");
				return NULL;
			}
			// I cannot figure out how to exclude ignorable whitespace using Xerces - it seems to be
			// either very stringent or very bad at how it identifies ignorable whitespace.  Instead,
			// I'm just excluding it myself here and in the GetLength() method.
			for (int i = 0; i < myNodeList->getLength() && index < myNodeList->getLength(); i++)
			{
				if (IsWhitespace(myNodeList->item(i)))
				{
					index++;
					continue;
				}
				if (i == index)
					return new XMLNode(myNodeList->item(i));
			}
			throw new GiPSiException("XMLNodeList.GetNode(index)", "Could not get node: Index is out of range.");
			return NULL;
		}
		catch (const XMLException & ex)
		{
			char error[256];
			sprintf_s(error, 256, "Could not get node (XMLException): %s", ex.getMessage());
			throw new GiPSiException("XMLNodeList.GetNode(index)", error);
			return NULL;
		}
	}

	/* The GetNode (name) method takes a native character string
	as input and returns the first child of the node list whose
	name matches the input string.  If node such Node exists, a
	NULL is returned. */
	XMLNode * XMLNodeList::GetNode(const char * name) const
	{
		try
		{
			unsigned int myLength = myNodeList->getLength();
			for(unsigned int i = 0; i < myLength; i++)
			{
				char * nodeName = XMLChToChar(myNodeList->item(i)->getNodeName());
				if (strcmp(nodeName, name) == 0)
				{
					delete nodeName;
					return new XMLNode(myNodeList->item(i));
				}
				delete nodeName;
			}
			char error[256];
			sprintf_s(error, 256, "Could not get node: Node '%s' does not exist", name);
			throw new GiPSiException("XMLNodeList.GetNode(name)", error);
		}
		catch (const XMLException & ex)
		{
			char error[256];
			sprintf_s(error, 256, "Could not get node (XMLException): %s", ex.getMessage());
			throw new GiPSiException("XMLNodeList.GetNode(name)", error);
			return NULL;
		}
	}

	/* Returns the number of nodes in the list. */
	unsigned int XMLNodeList::GetLength() const
	{
		int whitespace_count = 0;
		for (int i = 0; i < myNodeList->getLength(); i++)
			if (IsWhitespace(myNodeList->item(i)))
				whitespace_count++;
		return myNodeList->getLength() - whitespace_count;
	}
}
