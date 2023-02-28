/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi XMLNode Class Implementation (XMLNode.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	XMLNODE.CPP v0.0
////
////	XMLNode Class Implementation
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include "stdafx.h"

#include "XMLNode.h"
#include "XMLNodeList.h"
#include "XMLUtils.h"

/*
===============================================================================
	XMLDocumentBuilder class
===============================================================================
*/

namespace GiPSiXMLWrapper
{
	/* Initialize member variables. */
	XMLNode::XMLNode(DOMNode * newNode)
	{
		if (newNode == NULL)
		{
			throw new GiPSiException("XMLNode constructor", "Recieved NULL node.");
			return;
		}
		myNode = newNode;
	}
	/* Clean up any allocated memory. */
	XMLNode::~XMLNode()
	{
		// Don't delete the Node here; it's taken care of when the parser is deleted
	}

	/* In all cases, the GetName method returns the Tag Name of
	the enclosed Node. */
	char * XMLNode::GetName() const
	{
		char * name = XMLChToChar(myNode->getNodeName());
		if (name != NULL)
			return name;
		else
			return "";
	}

	/* The GetValue method returns the value of the enclosed Node.
	According to the W3C recommendation, Element nodes do not have
	associated values.
	In our schema, Element nodes have either complex or simple
	element types with consistent formats.
	Elements of the simple type contain a single, restricted-value
	text node as their only child node (having no other markup besides
	this text).  Calling GetValue on a simple Element will return
	the value of this child text node as the value of the simple
	Element.
	Elements of the complex type contain an embedded list of child
	Elements, and have no text directly associated with them.
	Calling GetValue on a complex Element will return NULL. */
	char * XMLNode::GetValue() const
	{
		// If this node has a single child and that child is a text node,
		// return the text of the text node
		DOMNodeList * children = myNode->getChildNodes();
		if (children->getLength() == 1)
		{
			DOMNode * child = children->item(0);
			if (child->getNodeType() == DOMNode::TEXT_NODE)
			{
				char * value = XMLChToChar(child->getTextContent());
				if (value == NULL)
				{
					char location[256] = "";
					sprintf_s(location, 256, "Node %s", GetName());
					throw new GiPSiException(location, "Could not get value: Value is NULL.");
					return NULL;
				}
				return value;
			}
		}
		char location[256] = "";
		sprintf_s(location, 256, "Node %s", GetName());
		throw new GiPSiException(location, "Could not get value: Incorrect node type.");
		return NULL;
	}

	/* The GetChildren method returns an XMLNodeList object
	representing the list of the Element’s child Elements. */
	XMLNodeList * XMLNode::GetChildren() const
	{
		DOMNodeList * children = myNode->getChildNodes();
		if (children)
		{
			if (children->getLength() > 1)
			{
				return new XMLNodeList(children);
			}
			else if (children->getLength() == 1)
			{
				DOMNode * child = children->item(0);
				if (child->getNodeType() != DOMNode::TEXT_NODE)
				{
					return new XMLNodeList(children);
				}
				char location[256] = "";
				sprintf_s(location, 256, "Node %s", GetName());
				throw new GiPSiException(location, "Could not get children: Incorrect node type.");
			}
			else
			{
				char location[256] = "";
				sprintf_s(location, 256, "Node %s", GetName());
				throw new GiPSiException(location, "Could not get children: Incorrect node type.");
			}
		}
		else
		{
			char location[256] = "";
			sprintf_s(location, 256, "Node %s", GetName());
			throw new GiPSiException(location, "Could not get children: Branch is empty.");
		}
		return NULL;
	}

	/* Returns the number of child nodes. */
	unsigned int XMLNode::GetNumChildren() const
	{
		int whitespace_count = 0;
		DOMNodeList * children = myNode->getChildNodes();
		for (int i = 0; i < children->getLength(); i++)
			if (IsWhitespace(children->item(i)))
				whitespace_count++;
		return children->getLength() - whitespace_count;
	}
}
