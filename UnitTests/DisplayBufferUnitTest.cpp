/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Source for GiPSi Display Buffer Unit Test (DisplayBufferUnitTest.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	DISPLAYBUFFERUNITTEST.CPP v0.0
////
////	Source for GiPSi Display Buffer Unit Test
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/


#include <stdio.h>

#include "DisplayBufferUnitTest.h"
#include "GiPSiDisplay.h"
#include "logger.h"
#include "ToolkitShaderParamLoader.h"
#include "ToolkitShaderParams.h"
#include "XMLDocumentBuilder.h"

/*
===============================================================================
	DisplayBufferUnitTest class
===============================================================================
*/

DisplayBufferUnitTest::DisplayBufferUnitTest()
{
	myFailedCount = 0;
}

void DisplayBufferUnitTest::Run()
{
	if (!logger)
		logger = new Logger();
	logger->SetVerboseLevel(9);

	// Test constructor
	printf("\nTest constructor\n");
	printf("\tTest 1:\t");
	DisplayHeader header1;
	header1.dataType		= 0;
	header1.lineSize		= 1;
	header1.objType			= DisplayObjectType::GIPSI_DRAW_CUSTOM;
	header1.pointSize		= 3;
	header1.polyMode		= DisplayPolygonMode::GIPSI_POLYGON_FILL;
	header1.shadeMode		= DisplayShadeMode::GIPSI_SHADE_FLAT;

	TriSurfaceDisplayManager manager1(NULL, &header1);
	char testObjectName1[6] = "test1";
	manager1.SetObjectName(testObjectName1);
	char testName1[11] = "";
	itoa(int(&manager1), testName1, 16);

	DisplayHeader header2;
	TEST_VERIFY(manager1.displayBuffer.GetDisplayHeader(&header2) == true &&
				manager1.displayBuffer.writeIndex == 0 &&
				manager1.displayBuffer.queueIndex == 1 &&
				manager1.displayBuffer.readIndex == 2 &&
				manager1.displayBuffer.data[0].dA_size == 0 &&
				manager1.displayBuffer.data[0].iA_size == 0 &&
				manager1.displayBuffer.data[0].dispArray == NULL &&
				manager1.displayBuffer.data[0].indexArray == NULL &&
				manager1.displayBuffer.data[1].dA_size == 0 &&
				manager1.displayBuffer.data[1].iA_size == 0 &&
				manager1.displayBuffer.data[1].dispArray == NULL &&
				manager1.displayBuffer.data[1].indexArray == NULL &&
				manager1.displayBuffer.data[2].dA_size == 0 &&
				manager1.displayBuffer.data[2].iA_size == 0 &&
				manager1.displayBuffer.data[2].dispArray == NULL &&
				manager1.displayBuffer.data[2].indexArray == NULL &&
				header2.dataType	== 0 &&
				header2.lineSize	== 1 &&
				header2.objType		== DisplayObjectType::GIPSI_DRAW_CUSTOM &&
				header2.pointSize	== 3 &&
				header2.polyMode	== DisplayPolygonMode::GIPSI_POLYGON_FILL &&
				header2.shadeMode	== DisplayShadeMode::GIPSI_SHADE_FLAT &&
				strcmp(manager1.displayBuffer.GetName(), testName1) == 0 &&
				strcmp(manager1.displayBuffer.GetObjectName(), testObjectName1) == 0);

	/*
	SetShaderParams is tested during the loader unit test.  To test it now would require assuming
	that the loader unit test had passed, but this test is a prerequisite of the loader unit test,
	so it isn't proper to test for it here.
	*/

	// Test SetNext
	printf("\nTest SetNext\n");
	printf("\tTest 1:\t");
	TriSurfaceDisplayManager manager2(NULL, &header1);
	char testObjectName2[6] = "test2";
	manager2.SetObjectName(testObjectName2);
	manager1.GetDisplayBuffer()->SetNext(manager2.GetDisplayBuffer());
	TEST_VERIFY(manager1.GetDisplayBuffer()->GetNext() == manager2.GetDisplayBuffer());

	// Test GetWriteArray and Enqueue
	printf("\nTest GetWriteArray and Enqueue\n");
	printf("\tTest 1:\t");
	TestDisplayManager manager3(&header2);
	char testObjectName3[6] = "test3";
	manager3.SetObjectName(testObjectName3);
	DisplayBuffer * buffer1 = manager3.GetDisplayBuffer();
	DisplayArray * da0 = &(buffer1->data[0]);
	DisplayArray * da1 = &(buffer1->data[1]);
	DisplayArray * da2 = &(buffer1->data[2]);

	DisplayArray * test0 = buffer1->GetWriteArray(true);
	buffer1->Enqueue();
	DisplayArray * test1 = buffer1->GetWriteArray(true);
	buffer1->Enqueue();
	DisplayArray * test2 = buffer1->GetWriteArray(true);

	TEST_VERIFY(test0 == da0 &&
				test1 == da1 &&
				test2 == da0 &&
				da0->DISPARRAY_NODESIZE == 5 &&
				da0->iA_size == 10 &&
				da0->dA_size == 15 &&
				da0->dispArray &&
				da0->indexArray &&
				da1->DISPARRAY_NODESIZE == 5 &&
				da1->iA_size == 10 &&
				da1->dA_size == 15 &&
				da1->dispArray &&
				da1->indexArray);

	// Test GetReadArray and Dequeue
	printf("\nTest GetReadArray and Dequeue\n");
	printf("\tTest 1:\t");
	test0 = buffer1->GetReadArray();
	buffer1->Dequeue();
	test1 = buffer1->GetReadArray();
	buffer1->Dequeue();
	test2 = buffer1->GetReadArray();

	TEST_VERIFY(test0 == da2 &&
				test1 == da1 &&
				test2 == da2);

	// Test GetReadArray and ConditionalDequeue
	printf("\nTest GetReadArray and ConditionalDequeue\n");
	printf("\tTest 1:\t");
	test0 = buffer1->GetReadArray();
	buffer1->data[buffer1->queueIndex].full = true;
	buffer1->ConditionalDequeue();
	test1 = buffer1->GetReadArray();
	buffer1->data[buffer1->queueIndex].full = true;
	buffer1->ConditionalDequeue();
	test2 = buffer1->GetReadArray();
	buffer1->ConditionalDequeue();
	DisplayArray * test3 = buffer1->GetReadArray();

	TEST_VERIFY(test0 == da2 &&
				test1 == da1 &&
				test2 == da2 &&
				test3 == da2);

	// Test removal of arrays
	printf("\nTest removal of arrays\n");
	printf("\tTest 1:\t");
	test0 = buffer1->GetWriteArray(false);
	buffer1->Enqueue();
	test1 = buffer1->GetWriteArray(false);
	buffer1->Dequeue();
	buffer1->Enqueue();
	test2 = buffer1->GetWriteArray(false);
	TEST_VERIFY(test0 == da0 &&
				test1 == da1 &&
				test2 == da2 &&
				da0->dA_size == 0 &&
				da0->iA_size == 0 &&
				da0->dispArray == NULL &&
				da0->indexArray == NULL &&
				da1->dA_size == 0 &&
				da1->iA_size == 0 &&
				da1->dispArray == NULL &&
				da1->indexArray == NULL &&
				da2->dA_size == 0 &&
				da2->iA_size == 0 &&
				da2->dispArray == NULL &&
				da2->indexArray == NULL);

	if (logger)
	{
		delete logger;
		logger = NULL;
	}
}

void DisplayBufferUnitTest::TEST_VERIFY(bool test)
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
