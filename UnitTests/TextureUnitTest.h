/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for GiPSi Texture Unit Test (TextureUnitTest.h).

The Initial Developer of the Original Code is John Pao.  
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	TEXTUREUNITTEST.H v0.0
////
////	Header for GiPSi Texture Unit Test
////
////////////////////////////////////////////////////////////////

#ifndef _TEXTURE_UNIT_TEST_H_
#define _TEXTURE_UNIT_TEST_H_

class TextureUnitTest
{
public:
	TextureUnitTest();
	void Run();
	int GetFailedCount() { return myFailedCount; }
	void TEST_VERIFY(bool test);
private:
	void Test_GIPSI_2D_STATIC_CLIENT();
	void Test_GIPSI_3D_STATIC_CLIENT();
	void Test_GIPSI_2D_STATIC_SERVER();
	void Test_GIPSI_2D_DYNAMIC_SERVER();

	int myFailedCount;
};

#endif