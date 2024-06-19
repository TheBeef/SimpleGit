/*******************************************************************************
 * FILENAME: trycatch.h
 * 
 * PROJECT:
 *    
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (14 Aug 2022)
 *       Created
 *
 *******************************************************************************/
#ifndef __TRYCATCH_H_
#define __TRYCATCH_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/
#define ctry(x) bool __HadError=false;x __ThrowValue;
#define ccatch(x) ExitJmp:;x=__ThrowValue;if(__HadError)
#define cthrow(x) {__ThrowValue=(x);__HadError=true;goto ExitJmp;}

#define cbasictry() bool __HadError=false;
#define cbasiccatch() ExitJmp:;if(__HadError)
#define cbasicthrow() {__HadError=true;goto ExitJmp;}

// DEBUG PAUL: Some day when gcc 8 is standard
//#define a(...) __VA_OPT__(printf("test\n"))

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
