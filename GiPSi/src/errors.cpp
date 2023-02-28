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
		fprintf(stderr, "%s Error:\t%s at \"%s\":%d: %s\n", PROG_NAME, text, __FILE__, 
		__LINE__, strerror(status));
	} else {
		fprintf(stderr, "%s Error:\t%s at \"%s\":%d: %s\n", PROG_NAME, text, __FILE__, 
		__LINE__);
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
   error_display(status, text);

   exit(0); 
}
