/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for GiPSi Exception (GiPSiException.h).

The Initial Developer of the Original Code is John Pao.  
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	GIPSIEXCEPTION.H v0.0
////
////	Header for GiPSi Exception
////
////////////////////////////////////////////////////////////////

#ifndef _GIPSI_EXCEPTION_H_
#define _GIPSI_EXCEPTION_H_

class GiPSiException
{
public:
	GiPSiException(char * location, char * content);
	~GiPSiException();

	const char * GetLocation() const;
	const char * GetContent() const;

private:
	char * myLocation, * myContent;
};

#endif