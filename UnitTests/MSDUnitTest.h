/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for GiPSi MSD Unit Test (MSDUnitTest.h).

The Initial Developer of the Original Code is John Pao.  
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	MSDUNITTEST.H v0.0
////
////	Header for GiPSi MSD Unit Test
////
////////////////////////////////////////////////////////////////

#ifndef _MSD_UNIT_TEST_H_
#define _MSD_UNIT_TEST_H_

#include "msd.h"

class MSDUnitTest: public MSDObject 
{
public:
	MSDUnitTest();

	void Run();
	int GetFailedCount() { return myFailedCount; }
	void TEST_VERIFY(bool test);

private:
	bool isEqualVector(Vector<Real> vec1, Vector<Real> vec2);
	int myFailedCount;
};

#endif