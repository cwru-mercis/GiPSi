/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Source for GiPSi Unit Tests (UnitTests.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao, Suriya Natsupakpong.
*/

////	UNITTESTS.CPP v0.0
////
////	Source for GiPSi Unit Tests
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include "stdafx.h"
#include "DisplayBufferUnitTest.h"
#include "LoaderUnitTest.h"
#include "LoggerUnitTest.h"
#include "TextureUnitTest.h"
#include "XMLUnitTest.h"
#include "IntegratorUnitTest.h"

/*
===============================================================================
	Entry Point for UnitTests.exe
===============================================================================
*/

#define RUN_LOGGER_TESTS		0x00000001
#define RUN_XML_TESTS			0x00000002
#define RUN_DISPBUFF_TESTS		0x00000004
#define RUN_TEXTURE_TESTS		0x00000008
#define RUN_LOADER_TESTS		0x00000010
#define RUN_INTEGRATOR_TESTS	0x00000020

int _tmain(int argc, _TCHAR* argv[])
{
	int x = _ttoi(argv[1]);

	if (x & RUN_LOGGER_TESTS)
	{
		LoggerUnitTest loggerTest;
		loggerTest.Run();
		printf("%d Failed\n", loggerTest.GetFailedCount());
	}

	if (x & RUN_XML_TESTS)
	{
		XMLUnitTest XMLTest;
		XMLTest.Run();
		printf("%d Failed\n", XMLTest.GetFailedCount());
	}

	if (x & RUN_DISPBUFF_TESTS)
	{
		DisplayBufferUnitTest DBTest;
		DBTest.Run();
		printf("%d Failed\n", DBTest.GetFailedCount());
	}

	if (x & RUN_TEXTURE_TESTS)
	{
		TextureUnitTest TextureTest;
		TextureTest.Run();
		printf("%d Failed\n", TextureTest.GetFailedCount());
	}

	if (x & RUN_LOADER_TESTS)
	{
		LoaderUnitTest LoaderTest;
		LoaderTest.Run();
		printf("%d Failed\n", LoaderTest.GetFailedCount());
	}

	if (x & RUN_INTEGRATOR_TESTS)
	{
		IntegratorUnitTest IntegratorTest;
		IntegratorTest.Run();
		printf("%d Failed\n", IntegratorTest.GetFailedCount());
	}

	return 0;
}

