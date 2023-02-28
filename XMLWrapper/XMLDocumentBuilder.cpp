/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi XMLDocumentBuilder Class Implementation (XMLDocumentBuilder.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	XMLDOCUMENTBUILDER.CPP v0.0
////
////	XMLDocumentBuilder Class Implementation
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include "stdafx.h"

#include <stdio.h>
#include <string.h>
#include <xercesc\sax\HandlerBase.hpp>
#include <xercesc\util\PlatformUtils.hpp>
#include <xercesc\util\XMLString.hpp>
#include <xercesc\validators\common\Grammar.hpp>

#include "XMLDocumentBuilder.h"
#include "XMLDocument.h"

/*
===============================================================================
	XMLDocumentBuilder class
===============================================================================
*/

namespace GiPSiXMLWrapper
{
	/* Initialize member variables. */
	XMLDocumentBuilder::XMLDocumentBuilder()
	{
		myBuilder = NULL;
		myErrorHandler = NULL;

		try
		{
			XMLPlatformUtils::Initialize();
        }
        catch (const XMLException& toCatch)
		{
			char * location = "XMLDocumentBuilder";
			char * error = new char[256];
			error = "Could not initialize XML platform: ";
			char * xmlError = XMLString::transcode(toCatch.getMessage());
			strcat_s(error, 256, xmlError);
			XMLString::release(&xmlError);
			throw new GiPSiException(location, error);
			delete error;
			return;
        }

		myBuilder = new XercesDOMParser();
		
		myBuilder->setValidationScheme(XercesDOMParser::Val_Auto);
		myBuilder->setIncludeIgnorableWhitespace(false);

		myErrorHandler = (ErrorHandler*) new HandlerBase();
		myBuilder->setErrorHandler(myErrorHandler);
	}

	/* Clean up any allocated memory, and close any open streams. */
	XMLDocumentBuilder::~XMLDocumentBuilder()
	{
		if (myBuilder)
		{
			myBuilder->resetCachedGrammarPool();
			myBuilder->resetDocumentPool();
			delete myBuilder;
			myBuilder = NULL;
		}
		if (myErrorHandler)
		{
			delete myErrorHandler;
			myErrorHandler = NULL;
		}

		try
		{
			XMLPlatformUtils::Terminate();
		}
		// XML Exceptions
        catch (const XMLException& toCatch)
		{
			char * location = "XMLDocumentBuilder";
			char * error = new char[256];
			error = "Could not terminate XML platform: ";
			char * xmlError = XMLString::transcode(toCatch.getMessage());
			strcat_s(error, 256, xmlError);
			XMLString::release(&xmlError);
			throw new GiPSiException(location, error);
			delete error;
			return;
        }
	}

	/* The Build method generates and returns an XMLDocument object.
	The XML source is read from the file referenced by the filePath
	parameter.  The Build method puts the XML source into whatever
	format is required by the parser’s DocumentBuilder object and
	requests that the document be built. */
	XMLDocument * XMLDocumentBuilder::Build(char * fileName) const
	{
		XMLDocument * doc = NULL;
		try
		{
			myBuilder->parse(fileName);
			doc = new XMLDocument(myBuilder->getDocument());
		}
		// XML exceptions
		catch (const XMLException & ex)
		{
			char * error = new char[256];
			error = "Could not parse project file (XML Exception): ";
			char * xmlError = XMLString::transcode(ex.getMessage());
			strcat_s(error, 256, xmlError);
			XMLString::release(&xmlError);
			throw new GiPSiException("XMLDocumentBuilder", error);
			delete error;
			return NULL;
		}
		// DOM-specific exceptions
		catch (const DOMException & ex)
		{
			char * error = new char[256];
			error = "Could not parse project file (DOM Exception): ";
			char * xmlError = XMLString::transcode(ex.getMessage());
			strcat_s(error, 256, xmlError);
			XMLString::release(&xmlError);
			throw new GiPSiException("XMLDocumentBuilder", error);
			delete error;
			return NULL;
		}
		// GiPSi exceptions
		catch (const GiPSiException & ex)
		{
			throw ex;
			return NULL;
		}
		// Other exceptions
		catch (...)
		{
			throw new GiPSiException("XMLDocumentBuilder", "Could not parse project file (Unexpected Exception).");
			return NULL;
		}
		return doc;
	}
}
