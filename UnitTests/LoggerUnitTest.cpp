/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Source for GiPSi Logger Unit Test (LoggerUnitTest.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	LOGGERUNITTEST.CPP v0.0
////
////	Source for GiPSi Logger Unit Test
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include "LoggerUnitTest.h"
#include <fstream>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/*
===============================================================================
	LoggerUnitTest class
===============================================================================
*/

LoggerUnitTest::LoggerUnitTest()
{
	myFailedCount = 0;
}

void * loggerOut(void * args)
{
	pthread_t id = pthread_self();
	char locationString[10];
	sprintf_s(locationString, 10, "%d", id);

	for (int i = 0; i < 100; i++)
	{
		logger->Message(locationString, "Critical Section", 0);
	}
	return NULL;
}

void LoggerUnitTest::Run()
{
	printf("Starting Logger unit test\n");
	if (!logger)
		logger = new Logger();
	char fileMode[5] = "File";
	logger->SetVerboseMode(fileMode);

	// Test 1 - File name generation
	printf("1 - Testing file name generation\n");

	// get time information
	time_t systemTime;
	time(&systemTime);
	tm localTime;
	localtime_s(&localTime, &systemTime);

	// format filename using time information
	char fileName[30];
	sprintf_s(fileName, "GiPSi_%02d-%02d-%04d_%02d-%02d-%02d.log", localTime.tm_mday, localTime.tm_mon, localTime.tm_year + 1900, localTime.tm_hour, localTime.tm_min, localTime.tm_sec);
	char * generatedName = logger->GenerateFileName();
	int diff = strcmp(generatedName, fileName);
	if (diff != 0)
	{
		myFailedCount++;
		printf("\tFailed\n");
	}
	else
		printf("\tPassed\n");
	delete generatedName;

	// Test 2 - Message formatting
	printf("2 - Testing message formatting\n");

	// set location and content strings
	char location[9] = "Location";
	char content[8] = "Content";

	// check message formatting
	char message[19] = "Location: Content\n";
	const char * generatedMessage = logger->FormatMessage(location, content);
	if (strcmp(generatedMessage, message) != 0)
	{
		myFailedCount++;
		printf("\tFailed\n");
	}
	else
		printf("\tPassed\n");
	delete generatedMessage;

	// Test 3 - Error formatting
	printf("3 - Testing error formatting\n");

	char error[26] = "Error: Location: Content\n";
	const char * generatedError = logger->FormatError(location, content);
	if (strcmp(generatedError, error) != 0)
	{
		myFailedCount++;
		printf("\tFailed\n");
	}
	else
		printf("\tPassed\n");
	delete generatedError;

	// Test 4 - Verbose filtering
	printf("4 - Testing verbose filtering (Manual results)\n");
	printf("(You should see 1 error and 1 message following, which state \"You should see this message\":\n");

	// create should and should not see case strings
	char shouldSee[] = "You should see this message";
	char shouldNotSee[] = "You should not see this message";

	// test boundary conditions
	// message level > verbose level
	logger->Message(location, shouldNotSee, 1);
	// message level <= verbose level
	logger->SetVerboseLevel(1);
	logger->Message(location, shouldSee, 1);
	logger->Error(location, shouldSee);

	// Test 5 - Concurrent usage
	printf("5 - Testing Concurrent Usage (Manual results)\n");
	pthread_t threadHandles[50];
	for (int i = 0; i < 50; i++)
	{
		int ret = pthread_create(&(threadHandles[i]), NULL, loggerOut, NULL);
		if (ret < 0)
			perror("pthread_create");
	}

	// Need to wait for the threads to finish
	for (int i = 0; i < 50; i++)
	{
		int ret = pthread_join(threadHandles[i],(void **)NULL);
		if (ret < 0)
			perror("pthread_join");
	}

	// Cleanup
	if (logger)
	{
		delete logger;
		logger = NULL;
	}
}