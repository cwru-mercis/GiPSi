/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Source for GiPSi GiPSi Exception (GiPSiException.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	GIPSIEXCEPTION.CPP v0.0
////
////	Source for GiPSi Exception
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include <stdlib.h>

#include "GiPSiException.h"
#include "Logger.h"

/*
===============================================================================
	GiPSiException class
===============================================================================
*/

/**
 * Constructor.
 * Logs an error using the input location and content strings.
 * 
 * @param location Character string stating the where the exception originates from.
 * @param content Character string containing the content of the exception.
 */
GiPSiException::GiPSiException(char * location, char * content)
{
	myLocation = new char[strlen(location) + 1];
	strcpy_s(myLocation, strlen(location) + 1, location);
	myContent = new char[strlen(content) + 1];
	strcpy_s(myContent, strlen(content) + 1, content);
	if (logger)
		logger->Error(location, content);
}

/**
 * Destructor.
 */
GiPSiException::~GiPSiException()
{
	if (myLocation)
	{
		delete myLocation;
		myLocation = NULL;
	}
	if (myLocation)
	{
		delete myContent;
		myContent = NULL;
	}
}

/**
 * Returns the location string for the exception.
 */
const char * GiPSiException::GetLocation() const
{
	return myLocation;
}

/**
 * Returns the content string for the exception.
 */
const char * GiPSiException::GetContent() const
{
	return myContent;
}