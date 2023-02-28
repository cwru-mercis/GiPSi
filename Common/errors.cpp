/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Error Manager Implementation (errors.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	ERRORS.C v0.0
////
////	Error Manager
////
////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "errors.h"


////////////////////////////////////////////////////////////////
//
//	error_display()
//
//		Displays the error message
//
void error_display(int status, char *text)
{
	if( status != -1) {
		fprintf(stderr, "Error:\t%s at \"%s\":%d: %s\n", text, __FILE__, 
		__LINE__, strerror(status));
	} else {
		fprintf(stderr, "%s\n", text);
	}

	fflush(stderr);
}


////////////////////////////////////////////////////////////////
//
//	error_exit()
//
//		Displays error and exits the program
//
void error_exit(int status, char *text)
{

	// Print error message

	fprintf(stderr,"Internal Error %i: ", status);
	fprintf(stderr,text);
	printf("\nTerminating as Result of Internal Error.\nPress Enter to exit.\n");

	// Keep the terminal open

	int anyKey = getchar();

	// Exit program

	exit(status);
}
