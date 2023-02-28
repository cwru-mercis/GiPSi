/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi XMLUtils Class Implementation (XMLUtils.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	XMLUTILS.CPP v0.0
////
////	XMLUtils Class Implementation
////
////////////////////////////////////////////////////////////////

#include "XMLUtils.h"

namespace GiPSiXMLWrapper
{
	/* The XMLToChar method takes an XMLCh pointer, and
	allocates and returns a character array containing
	an equivalent string. */
	char * XMLChToChar(const XMLCh * in)
	{
		unsigned int length = XMLString::stringLen(in);
		char * out = new char[length + 1];
		for (unsigned int i = 0; i < length; i++)
		{
			out[i] = (char)in[i];
		}
		out[length] = '\0';
		return out;
	}

	bool IsWhitespace(DOMNode * node)
	{
		char * name = XMLChToChar(node->getNodeName());
		return (strcmp(name, "#text") == 0);
	}
}