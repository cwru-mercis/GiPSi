/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Source for GiPSi Texture Unit Test (TextureUnitTest.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	TEXTUREUNITTEST.CPP v0.0
////
////	Source for GiPSi Texture Unit Test
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include <stdio.h>
#include <string.h>

#include "GiPSiDisplay.h"
#include "GiPSiException.h"
#include "GiPSiTexture.h"
#include "logger.h"
#include "read_tga.h"
#include "TextureUnitTest.h"

/*
===============================================================================
	TextureUnitTest class
===============================================================================
*/

TextureUnitTest::TextureUnitTest()
{
	myFailedCount = 0;
}

void TextureUnitTest::Run()
{
	if (!logger)
		logger = new Logger();

	// Test 1 - GIPSI_2D_STATIC_CLIENT
	printf("1 - Testing GIPSI_2D_STATIC_CLIENT\n");
	Test_GIPSI_2D_STATIC_CLIENT();

	// Test 2 - GIPSI_3D_STATIC_CLIENT
	printf("2 - Testing GIPSI_3D_STATIC_CLIENT\n");
	Test_GIPSI_3D_STATIC_CLIENT();

	// Test 3 - GIPSI_2D_STATIC_SERVER
	printf("3 - Testing GIPSI_2D_STATIC_SERVER\n");
	Test_GIPSI_2D_STATIC_SERVER();

	// Test 4 - GIPSI_2D_DYNAMIC_SERVER
	printf("4 - Testing GIPSI_2D_DYNAMIC_SERVER\n");
	Test_GIPSI_2D_DYNAMIC_SERVER();

	if (logger)
	{
		delete logger;
		logger = NULL;
	}
}


void TextureUnitTest::Test_GIPSI_2D_STATIC_CLIENT()
{
	// Test TextureDisplayManager initialization (SK)
	printf("Testing TextureDisplayManager initialization (SK):\t");
	char * newFileName1 = new char[21];
	sprintf_s(newFileName1, 21, ".\\TGAFiles\\TestS.tga");
	TextureDisplayManager * manager1 = new TextureDisplayManager(GIPSI_2D_STATIC_CLIENT, newFileName1);
	char textureName1[6] = "TestS";
	manager1->SetObjectName(textureName1);
	DisplayBuffer * buffer1 = &(manager1->displayBuffer);
	char addr1[11] = "";
	itoa((int)manager1, addr1, 16);

	TEST_VERIFY(buffer1 != NULL &&
				buffer1->GetTextureType() == GIPSI_2D_STATIC_CLIENT &&
				strcmp(manager1->textureFile, ".\\TGAFiles\\TestS.tga") == 0 &&
				strcmp(buffer1->GetObjectName(), textureName1) == 0 &&
				strcmp(buffer1->GetName(), addr1) == 0 &&
				buffer1->nArrays == 1);

	// Test Texture2D initialization (pre-Display)
	printf("Testing Texture2D initialization (pre-Display):\t\t");
	Texture2D * texture1 = new Texture2D(buffer1);

	TEST_VERIFY(texture1->width == 0 &&
				texture1->height == 0 &&
				texture1->textureType == GIPSI_2D_STATIC_CLIENT &&
				strcmp(texture1->GetObjectName(), textureName1) == 0);

	// Test Display method
	printf("Testing display:\n");
	manager1->Display();

	DisplayArray * array1 = buffer1->GetReadArray();
	char * fileName1 = new char[array1->iA_size + 1];
	for (int i = 0; i < array1->iA_size; i++)
	{
		fileName1[i] = (char)array1->indexArray[i];
	}
	fileName1[array1->iA_size] = '\0';

	printf("\t\t\t\t\t\t\t");
	TEST_VERIFY(strcmp(fileName1, manager1->textureFile) == 0);

	// Test Texture2D update (post-Display)
	printf("Testing Texture2D update (post-Display):\n");
	texture1->UpdateTexture();
	TGA *TGAImage	= new TGA(".\\TGAFiles\\TestS.tga");

	printf("\t\t\t\t\t\t\t");
	TEST_VERIFY(texture1->width == TGAImage->GetWidth() &&
				texture1->height == TGAImage->GetHeigth() &&
				texture1->textureType == GIPSI_2D_STATIC_CLIENT &&
				strcmp(texture1->GetObjectName(), textureName1) == 0);

	delete TGAImage;
	delete manager1;
	delete fileName1;
}


void TextureUnitTest::Test_GIPSI_3D_STATIC_CLIENT()
{
	// Test TextureDisplayManager initialization (SK)
	printf("Testing TextureDisplayManager initialization (SK):\t");
	char * newFileName1 = new char[21];
	sprintf_s(newFileName1, 21, ".\\TGAFiles\\TestS.tga");
	TextureDisplayManager * manager1 = new TextureDisplayManager(GIPSI_3D_STATIC_CLIENT, newFileName1);
	char textureName1[6] = "TestS";
	manager1->SetObjectName(textureName1);
	DisplayBuffer * buffer1 = &(manager1->displayBuffer);
	char addr1[11] = "";
	itoa((int)manager1, addr1, 16);

	TEST_VERIFY(buffer1 != NULL &&
				buffer1->GetTextureType() == GIPSI_3D_STATIC_CLIENT &&
				strcmp(buffer1->GetObjectName(), textureName1) == 0 &&
				strcmp(manager1->textureFile, ".\\TGAFiles\\TestS.tga") == 0 &&
				strcmp(buffer1->GetName(), addr1) == 0 &&
				buffer1->nArrays == 1);

	// Test Texture3D initialization (pre-Display)
/*	printf("Testing Texture3D initialization (pre-Display):\t");
	Texture3D * texture1 = new Texture3D(buffer1);

	TEST_VERIFY(texture1->width == 0 &&
				texture1->height == 0 &&
				texture1->depth == 0 &&
				texture1->textureType == GIPSI_3D_STATIC_CLIENT &&
				strcmp(texture1->GetObjectName(), textureName1) == 0);
*/
	// Test Display method
	printf("Testing display:\n");
	manager1->Display();

	DisplayArray * array1 = buffer1->GetReadArray();
	char * fileName1 = new char[array1->iA_size + 1];
	for (int i = 0; i < array1->iA_size; i++)
	{
		fileName1[i] = (char)array1->indexArray[i];
	}
	fileName1[array1->iA_size] = '\0';

	printf("\t\t\t\t\t\t\t");
	TEST_VERIFY(strcmp(fileName1, manager1->textureFile) == 0);

	// Test Texture3D update (post-Display)
/*	printf("Testing Texture3D update (post-Display):\n");
	texture1->UpdateTexture();
	TGA *TGAImage	= new TGA(".\\TGAFiles\\TestS.tga");

	printf("\t\t\t\t\t\t\t");
	TEST_VERIFY(texture1->width == TGAImage->GetWidth() &&
				texture1->height == TGAImage->GetHeigth() &&
				texture1->depth == TGAImage->GetDepth() &&
				texture1->textureType == GIPSI_2D_STATIC_CLIENT &&
				strcmp(texture1->GetObjectName(), textureName1) == 0);

	delete TGAImage; */
	delete manager1;
	delete fileName1;
}


void TextureUnitTest::Test_GIPSI_2D_STATIC_SERVER()
{
	// Test TextureDisplayManager initialization (SK)
	printf("Testing TextureDisplayManager initialization (SK):\t");
	char * newFileName1 = new char[21];
	sprintf_s(newFileName1, 21, ".\\TGAFiles\\TestS.tga");
	TextureDisplayManager * manager1 = new TextureDisplayManager(GIPSI_2D_STATIC_SERVER, newFileName1);
	char textureName1[6] = "TestS";
	manager1->SetObjectName(textureName1);
	DisplayBuffer * buffer1 = &(manager1->displayBuffer);
	char addr1[11] = "";
	itoa((int)manager1, addr1, 16);

	TEST_VERIFY(buffer1 != NULL &&
				buffer1->GetTextureType() == GIPSI_2D_STATIC_SERVER &&
				strcmp(manager1->textureFile, ".\\TGAFiles\\TestS.tga") == 0 &&
				strcmp(buffer1->GetObjectName(), textureName1) == 0 &&
				strcmp(buffer1->GetName(), addr1) == 0 &&
				buffer1->nArrays == 1);

	// Test Texture2D initialization (pre-Display)
	printf("Testing Texture2D initialization (pre-Display):\t\t");
	Texture2D * texture1 = new Texture2D(buffer1);

	TEST_VERIFY(texture1->width == 0 &&
				texture1->height == 0 &&
				texture1->textureType == GIPSI_2D_STATIC_SERVER &&
				strcmp(texture1->GetObjectName(), textureName1) == 0);

	// Test Display method
	printf("Testing display:\t\t\t\t\t");
	TGA *TGAImage	= new TGA(".\\TGAFiles\\TestS.tga");

	try {
		manager1->Display();

		DisplayArray * array1 = buffer1->GetReadArray();
		TEST_VERIFY(array1->DISPARRAY_NODESIZE == TGAImage->GetWidth() &&
					array1->iA_size / array1->DISPARRAY_NODESIZE == TGAImage->GetHeigth() &&
					array1->indexArray != NULL);
	}
	catch(GiPSiException e) {
		TEST_VERIFY(false);
	}

	// Test Texture2D update (post-Display)
	printf("Testing Texture2D update (post-Display):\n");
	texture1->UpdateTexture();

	printf("\t\t\t\t\t\t\t");
	TEST_VERIFY(texture1->width == TGAImage->GetWidth() &&
				texture1->height == TGAImage->GetHeigth() &&
				texture1->textureType == GIPSI_2D_STATIC_SERVER &&
				strcmp(texture1->GetObjectName(), textureName1) == 0);

	delete TGAImage;
	delete manager1;
}


void TextureUnitTest::Test_GIPSI_2D_DYNAMIC_SERVER()
{
	// Test TextureDisplayManager initialization (SK)
	printf("Testing TextureDisplayManager initialization (SK):\t");
	char * newFileName1 = new char[21];
	sprintf_s(newFileName1, 21, ".\\TGAFiles\\TestS.tga");
	TextureDisplayManager * manager1 = new TextureDisplayManager(GIPSI_2D_DYNAMIC_SERVER, newFileName1);
	char textureName1[6] = "TestS";
	manager1->SetObjectName(textureName1);
	DisplayBuffer * buffer1 = &(manager1->displayBuffer);
	char addr1[11] = "";
	itoa((int)manager1, addr1, 16);

	TEST_VERIFY(buffer1 != NULL &&
				buffer1->GetTextureType() == GIPSI_2D_DYNAMIC_SERVER &&
				strcmp(manager1->textureFile, ".\\TGAFiles\\TestS.tga") == 0 &&
				strcmp(buffer1->GetObjectName(), textureName1) == 0 &&
				strcmp(buffer1->GetName(), addr1) == 0 &&
				buffer1->nArrays == 3);

	// Test Texture2D initialization (pre-Display)
	printf("Testing Texture2D initialization (pre-Display):\t\t");
	Texture2D * texture1 = new Texture2D(buffer1);

	TEST_VERIFY(texture1->width == 0 &&
				texture1->height == 0 &&
				texture1->textureType == GIPSI_2D_DYNAMIC_SERVER &&
				strcmp(texture1->GetObjectName(), textureName1) == 0);

	// Test Display method
	printf("Testing display:\t\t\t\t\t");
	manager1->Display();

	TGA *TGAImage	= new TGA(".\\TGAFiles\\TestS.tga");

	buffer1->Dequeue();
	DisplayArray * array1 = buffer1->GetReadArray();
	TEST_VERIFY(array1->DISPARRAY_NODESIZE == TGAImage->GetWidth() &&
				array1->iA_size / array1->DISPARRAY_NODESIZE == TGAImage->GetHeigth() &&
				array1->indexArray != NULL);

	// Test Texture2D update (post-Display)
	printf("Testing Texture2D update (post-Display):\n");
	texture1->UpdateTexture();

	printf("\t\t\t\t\t\t\t");
	TEST_VERIFY(texture1->width == TGAImage->GetWidth() &&
				texture1->height == TGAImage->GetHeigth() &&
				texture1->textureType == GIPSI_2D_DYNAMIC_SERVER &&
				strcmp(texture1->GetObjectName(), textureName1) == 0);

	delete TGAImage;
	delete manager1;
}


void TextureUnitTest::TEST_VERIFY(bool test)
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
