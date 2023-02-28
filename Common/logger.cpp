/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Logger Class Implementation (logger.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	LOGGER.CPP v0.0
////
////	Logger Class Implementation
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include <stdio.h>
#include <time.h>

#include "logger.h"

// This logger reference is exported globally.  Anyone wishing to use the project
// logger should use '#include "Logger.h"'
Logger * logger = NULL;

/*
===============================================================================
	Logger class
===============================================================================
*/

/**
 * Constructor.
 */
Logger::Logger()
{
	// Initialize the semaphore
	int ret = sem_init(&out_semaphore, 0, 1);
	if (ret < 0)
		perror("sem_init");

	fout = NULL;
	fileName = NULL;

	SetVerboseMode();
	SetVerboseLevel();

	return;
}

/**
 * Destructor.
 */
Logger::~Logger()
{
	if (fout)
	{
		fout->close();
		delete fout;
		fout = NULL;
	}
	if (fileName)
	{
		delete fileName;
		fileName = NULL;
	}

	return;
}

/**
 * The Message method takes location and content strings,
 * as well as a verbosity level integer value, as input.
 * The logger compares the given verbosity level against
 * that of the project; if it is greater than or equal to
 * the project level, it formats the strings into a status
 * message and outputs them to the log file and the project
 * logging window.
 *
 * @param location Character string stating the where the message originates from.
 * @param content Character string containing the content of the message.
 * @param verbosity Integer (range 0-10) containing the verbosity level of the message (0 = always display, 10 = rarely display)
 */
const void Logger::Message(const char * location, const char * content, const int verbosity)
{
	if (verbosity > myVerboseLevel)
		return;
	const char * output = FormatMessage(location, content);
	PrintOutput(output, strlen(output));
	delete output;
}

/**
 * The Error method takes location and content strings as
 * input, formats them into an error message, and outputs them
 * to the log file and the project logging window.
 *
 * @param location Character string stating the where the error originates from.
 * @param content Character string containing the content of the error.
 */
const void Logger::Error(const char * location, const char * content)
{
	const char * output = FormatError(location, content);
	PrintOutput(output, strlen(output));
	delete output;
}

/**
 * The SetVerboseMode method takes as input the verboseMode
 * value as an enumerated type.  If the verboseMode value
 * indicates file output, the logger will generate a file name
 * based on the current time and open the file for writing.
 *
 * @param verboseMode Character string containing the verbose mode setting (File or Screen)
 */
void Logger::SetVerboseMode(const char * verboseMode)
{
	int ret = sem_wait(&out_semaphore);	/* start of critical section */
	if (ret < 0)
		perror("sem_wait");

	if (verboseMode != NULL &&
		strcmp(verboseMode, "File") == 0)
	{
		// Setting verbose mode to File
		// If a file is already opened, close it and deallocate the output stream
		if (fout)
		{
			fout->close();
			delete fout;
		}
		// If a fileName has already been created, deallocate it
		if (fileName)
			delete fileName;
		// Generate a new fileName
		fileName = GenerateFileName();
		// Create and open a new output stream
		fout = new std::ofstream(fileName, std::ios_base::out | std::ios_base::app);
	}
	else //if (strcmp(verboseMode, "Screen"))
	{
		// Close and deallocate existing output stream (if one exists)
		if (fout)
		{
			fout->close();
			delete fout;
			fout = NULL;
		}
		// Deallocate existing fileName (if one exists)
		if (fileName)
		{
			delete fileName;
			fileName = NULL;
		}
	}
	ret = sem_post(&out_semaphore);		/* end of critical section */
	if (ret < 0)
		perror("sem_post");
}

/**
 * The SetVerboseLevel method takes as input an integer
 * value representing the level (0-9) of verbosity that the
 * logger will display.  It stores this value in a member
 * variable for reference when processing message commands.
 * 
 * @param verboseLevel Integer value of the new verbose level value (0 = most restrictive, 10 = lest restrictive)
 */
void Logger::SetVerboseLevel(int verboseLevel)
{
	int ret = sem_wait(&out_semaphore);	/* start of critical section */
	if (ret < 0)
		perror("sem_wait failed");
	myVerboseLevel = verboseLevel;
	ret = sem_post(&out_semaphore);		/* end of critical section */
	if (ret < 0)
		perror("sem_post failed");
}

/**
 * The GenerateFileName method formats current system time
 * into local time, uses this to generate and return a log file name.
 */
char * Logger::GenerateFileName()
{
	// get time information
	time_t systemTime;
	time(&systemTime);
	tm localTime;
	localtime_s(&localTime, &systemTime);

	// format filename using time and date information
	size_t bufferSize = 30;
	char * newName = new char[bufferSize];
	sprintf_s(newName, bufferSize, "GiPSi_%02d-%02d-%04d_%02d-%02d-%02d.log", localTime.tm_mday, localTime.tm_mon, localTime.tm_year + 1900, localTime.tm_hour, localTime.tm_min, localTime.tm_sec);

	return newName;
}

/**
 * The FormatMessage method takes a location and content string
 * and formats them into the log message format.
 * 
 * @param location Character string stating the where the message originates from.
 * @param content Character string containing the content of the message.
 */
const char * Logger::FormatMessage(const char * location, const char * content)
{
	size_t locationLength = strlen(location);
	size_t contentLength = strlen(content);
	char * newString = new char[locationLength + contentLength + 4];
	sprintf_s(newString, locationLength + contentLength + 4, "%s: %s\n", location, content);
	return newString;
}

/**
 * The FormatMessage method takes a location and content string
 * and formats them into the log error format.
 * 
 * @param location Character string stating the where the error originates from.
 * @param content Character string containing the content of the error.
 */
const char * Logger::FormatError(const char * location, const char * content)
{
	char errorString[8] = "Error: ";
	size_t errorLength = strlen(errorString);

	const char * messageString = FormatMessage(location, content);
	size_t messageLength = strlen(messageString);

	int newStringSize = int(messageLength + errorLength + 1);
	char * newString = new char[newStringSize];
	strcpy_s(newString, newStringSize, errorString);

	strcat_s(newString, newStringSize, messageString);
	delete messageString;

	return newString;
}

/**
 * The PrintOutput method prints the input character string to
 * the output screen and, if available, log file.
 * 
 * @param outputString Character string containing the formatted message.
 * @param size size_t containing the length of outputString.
 */
const void Logger::PrintOutput(const char * outputString, const size_t size)
{
	// protect shared output resources with semaphore
	int ret = sem_wait(&out_semaphore);	/* start of critical section */
	if (ret < 0)
		printf("sem_wait");
	// Write to output stream if one exists
	if (fout)
		fout->write(outputString, (std::streamsize)size);
	// Print to output window
	printf(outputString);
	ret = sem_post(&out_semaphore);		/* end of critical section */
	if (ret < 0)
		printf("sem_post");
}