/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Error Manager Implementation (errors.h).

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


/**
 * Displays the error message.
 * 
 * @param status Error status.
 * @param text Error text.
 */
void error_display(int status, char *text)
{
	if( status != -1) {
		fprintf(stderr, "%s Error:\t%s at \"%s\":%d: %s\n", PROG_NAME, text, __FILE__, 
		__LINE__, strerror(status));
	} else {
		fprintf(stderr, "%s Error:\t%s at \"%s\":%d: %s\n", PROG_NAME, text, __FILE__, 
		__LINE__);
	}

	fflush(stderr);
}


/**
 * Displays the error message and exits the program.
 * 
 * @param status Error status.
 * @param text Error text.
 */
void error_exit(int status, char *text)
{
   error_display(status, text);

   exit(0); 
}
