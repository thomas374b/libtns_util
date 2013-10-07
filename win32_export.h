/* 
 * Thomas Pantzer: win32_export.h, Version: v0.1a  15.01.2006 12:31 
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

//static char SCCS_ID [] = "@(#) win32_export.h \main\1 @(#)";
/******************************************************************************/
/*                                                                            */
/* FILE NAME :     win32_export.h                                             */
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



#ifndef _WIN32_EXPORT_H
#define _WIN32_EXPORT_H

#if _WINDOWS | WIN32
	// Windows hat kein PI
	#define M_PI	3.14159265358979323844
	#define snprintf _snprintf

	#ifdef _MONOLITH
		/* static linking of all object files to one executable (no DLL) */
		#define	TNS_UTIL_API			
		#define TNS_UTIL_API_CLASS		class
	#else
		/* dynamic linking with DLL */
		#ifdef TNS_UTIL_EXPORTS
			/* if included in  compilation of DLL */
			#define TNS_UTIL_API __declspec(dllexport)
		#else
			/* if included in compilation of main.c */
			#define TNS_UTIL_API __declspec(dllimport)
		#endif
	
		#define TNS_UTIL_API_CLASS		class TNS_UTIL_API
	#endif

	#define TNS_UTIL_API_EXTERN		extern TNS_UTIL_API
#else
	/* mimic the windows declaration for externals */
	#define	TNS_UTIL_API			extern
	#define TNS_UTIL_API_EXTERN		extern
	#define TNS_UTIL_API_CLASS		class
#endif



#endif // _WIN32_EXPORT_H
