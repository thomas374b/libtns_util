/* 
 * Thomas Pantzer: tns_util.h, Version: v0.1a  15.01.2006 12:31 
 * 
 * 
 * Copyright 2006 by Thomas Pantzer
 * 
 * Permission to use, copy, modify, and distribute this software for noncommercial 
 * use and without fee is hereby granted, provided that the above copyright notice 
 * appear in all copies and that both that copyright notice and this permission 
 * notice appear in supporting documentation, and that the name of the Author not be 
 * used in advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission. The Author makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is" 
 * without expressed or implied warranty. 
 * 
 * 
 * pantec@dvorak.private.site   (Thomas Pantzer,,,) 
 * 
 */

//static char SCCS_ID [] = "@(#) tns_util.h \main\1 @(#)";
/******************************************************************************/
/*                                                                            */
/* FILE NAME :     tns_util.h                                                 */
/* VERSION :       \main\1                                                    */
/*                                                                            */
/* FUNCTION :      timer, network, sort, buffer, options Utilities Library    */
/*                 written by Thomas Pantzer                                  */
/*                                                                            */
/* AUTHOR :        Thomas Pantzer                                             */
/* CREATION DATE : 07.11.2002                                                 */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* Copyright (C) Siemens AG 2001                                              */
/*----------------------------------------------------------------------------*/
/* HISTORY OF CHANGE                                                          */
/*----------------------------------------------------------------------------*/
/* VER| DATE | CHANGE                                       | AUTHOR          */
/*----------------------------------------------------------------------------*/
/* +++ \main +++                                            |                 */
/* 001|071102| timer, network, sort, buffer, options        | pan             */
/*    |      | Utilities Library written by Thomas Pantzer  |                 */
/*    |      |                                              |                 */
/******************************************************************************/

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TNS_UTIL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TNS_UTIL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef TNS_UTIL_EXPORTS
#define TNS_UTIL_API __declspec(dllexport)
#else
#define TNS_UTIL_API __declspec(dllimport)
#endif


#define VERSION(major,minor,patchlevel)			((major<<16) | (minor << 8) | patchlevel)
#define UTS_VERSION(major,minor,patchlevel)		_T(#major"."#minor"."#patchlevel)

#define BASECORE_VERSION						VERSION(9,1,0)
#define BASECORE_VERSION_STRING					UTS_VERSION(9,1,0)

#if (BASECORE_VERSION > VERSION(5,1,0))
#pragma message("is higher")
#else
#pragma message("is lower")
#endif


// This class is exported from the tns_util.dll
class TNS_UTIL_API CTns_util {
	public:
	CTns_util(void);
	// TODO: add your methods here.
};

extern TNS_UTIL_API int nTns_util;

TNS_UTIL_API int fnTns_util(void);

