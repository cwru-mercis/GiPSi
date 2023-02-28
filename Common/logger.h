/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Header for GiPSi Logger API (logger.h).

The Initial Developer of the Original Code is John Pao.  
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	LOGGER.H v0.0
////
////	Header for GiPSi Logger API
////
////////////////////////////////////////////////////////////////

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <fstream>
#include <pthread.h>
#include <semaphore.h>

class LoggerUnitTest;

class Logger
{
public:
	Logger();
	~Logger();

	const void Message(const char * location, const char * content, const int verbosity);
	const void Error(const char * location, const char * content);

	void SetVerboseMode(const char * verboseMode = NULL);
	void SetVerboseLevel(int verboseLevel = 0);

private:
	char * GenerateFileName();
	const char * FormatMessage(const char * location, const char * content);
	const char * FormatError(const char * location, const char * content);

	const void PrintOutput(const char * outputString, const size_t size);

	int myVerboseMode, myVerboseLevel;
	char * fileName;
	std::ofstream * fout;

	sem_t out_semaphore;

	friend LoggerUnitTest;
};

extern Logger * logger;

#endif