/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi XMLDocument Class Implementation (XMLDocument.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	XMLDOCUMENT.CPP v0.0
////
////	XMLDocument Class Implementation
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include "stdafx.h"

#include "XMLDocument.h"
#include "XMLNode.h"

/*
===============================================================================
	XMLDocumentBuilder class
===============================================================================
*/

namespace GiPSiXMLWrapper
{
	/* Initialize member variables. */
	XMLDocument::XMLDocument(DOMDocument * newDocument)
	{
		if (newDocument == NULL)
		{
			throw new GiPSiException("XMLDocument constructor", "Received NULL document.");
			return;
		}
		myDocument = newDocument;
	}
	/* Clean up any allocated memory. */
	XMLDocument::~XMLDocument()
	{
		// Don't delete the Document here; it's taken care of when the parser is deleted
	}

	/* The GetRootNode method returns the Document’s root node
	as an XMLNode object.  Any errors or exceptions encountered
	will generate an exception that should be handled by clients
	of the XMLDocument object. */
	XMLNode * XMLDocument::GetRootNode() const
	{
		return new XMLNode(myDocument->getDocumentElement());
	}
}