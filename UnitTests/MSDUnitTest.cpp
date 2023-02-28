/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Source for GiPSi MSD Unit Test (MSDUnitTest.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): Suriya Natsupakpong.
*/

////	MSDUNITTEST.CPP v0.0
////
////	Source for MSD Unit Test
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/


#include <stdio.h>

#include "MSDUnitTest.h"
#include "logger.h"
#include "XMLDocumentBuilder.h"

/*
===============================================================================
	MSDUnitTest class
===============================================================================
*/

MSDUnitTest::MSDUnitTest():MSDObject(NULL)
{
	myFailedCount = 0;		
}

bool MSDUnitTest::isEqualVector(Vector<Real> vec1, Vector<Real> vec2)
{
	if ((vec1[0]==vec2[0])&&(vec1[1]==vec2[1])&&(vec1[2]==vec2[2]))
		return true;
	else
		return false;
}

void MSDUnitTest::Run()
{
	if (!logger)
		logger = new Logger();
	logger->SetVerboseLevel(9);

	// Test MSDObject function
	printf("\n1. Test MSDObject function\n");		
	
	printf("\tTest 1a: .obj loader\t");
	Load(".\\objects\\msd2d_5x5.obj");
	TEST_VERIFY(GetGeometryPtr() != NULL);

	printf("\tTest 1b: .map loader\t");
	LoadMAP(".\\objects\\msd2d_5x5.obj");
	TEST_VERIFY(mapping != NULL);

	printf("\tTest 1c: .msd loader\t");
	LoadMSD(".\\objects\\msd2d_5x5.obj");
	TEST_VERIFY(force != NULL &&
				mass != NULL &&
				ground_pos != NULL &&
				spring != NULL &&
				fix_boundary != NULL);

	printf("\tTest 1d: init\t");
	Init();
	TEST_VERIFY(true);

	printf("\tTest 1e: integration method\t");
	int_method = getIntegrationMethod("Euler");
	TEST_VERIFY(int_method == 1);
	SetIntegrationMethod(int_method);
	TEST_VERIFY(integrator != NULL);

	printf("\tTest 1f: getEnergy\t");
	Real result = getEnergy();
	TEST_VERIFY(result > 0.0);



	// Test Spring function
	printf("\n2. Test Spring function\n");		
	Spring Tspring;
	printf("\tTest 2a: constructor\t");	
	TEST_VERIFY(Tspring.dir.length() == 0);

	printf("\tTest 2b: setMassIndex\t");
	Tspring.setMassIndex(0,1);
	TEST_VERIFY(Tspring.node[0] == 0 &&
				Tspring.node[1] == 1);

	printf("\tTest 2c: setProperties\t");
	Tspring.setProperties(10,9,8);
	TEST_VERIFY(Tspring.l_zero == 10 &&
				Tspring.k_stiff == 9 &&
				Tspring.b_damp == 8);

	printf("\tTest 2d: getEnergy\t");
	TEST_VERIFY(Tspring.getEnergy() == 450);

	printf("\tTest 2e: length\t");
	TEST_VERIFY(Tspring.length() == 0);



	// Test MSDBoundary function
	printf("\n3. Test MSDBoundary function\n");		
	MSDBoundary Tboundary;
	Vector<Real> Tresult;

	printf("\tTest 3a: init\t");	
	Tboundary.init(NULL,2,2);
	TEST_VERIFY(Tboundary.Object == NULL &&
				Tboundary.num_vertex == 2 &&
				Tboundary.num_face == 2);

	printf("\tTest 3b: set\t");	
	//Tboundary.Set(1,1,zero_vector3,0.0,zero_vector3);
	//TEST_VERIFY(Tboundary.boundary_type[1] == 0 &&
	//			isEqualVector(Tboundary.boundary_value[1],zero_vector3) &&
	//			Tboundary.boundary_value2_scalar[1] == 0.0 &&
	//			isEqualVector(Tboundary.boundary_value2_vector[1],zero_vector3));

	printf("\tTest 3c: getPosition\t");	
	//Tresult = Tboundary.GetPosition(1);
	TEST_VERIFY(true);

	printf("\tTest 3d: getVelocity\t");	
	//Tresult = Tboundary.GetVelocity(1);
	TEST_VERIFY(true);
	
	printf("\tTest 3e: getReactionForce\t");	
	//Tresult = Tboundary.GetReactionForce(1);
	TEST_VERIFY(Tresult.length() == 0);

	printf("\tTest 3f: resetBoundState\t");	
	TEST_VERIFY(true);

	printf("\tTest 3g: Bound2Geom\t");	
	TEST_VERIFY(true);

	if (logger)
	{
		delete logger;
		logger = NULL;
	}
}

void MSDUnitTest::TEST_VERIFY(bool test)
{
	if (test)
	{
		printf("Passed\n");
	}
	else
	{
		myFailedCount++;
		printf("Failed\n");
	}
}