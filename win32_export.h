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



// #ifndef _WIN32_EXPORT_H
// #define _WIN32_EXPORT_H


#ifdef _WIN32
	#ifdef MODULE_G
//		#ifdef TNS_UTIL_EXPORTS
//			#define DLL_EXPORT	extern
//			#define DLL_CLASS	class
//		#else
		#define DLL_EXPORT __declspec(dllexport)
		#define DLL_CLASS	class DLL_EXPORT
//		#define DLL_NAMESPACE	namespace DLL_EXPORT
//		#endif
	#else
		#ifdef TNS_UTIL_EXPORTS
			#define DLL_EXPORT	extern
			#define DLL_CLASS	class
//			#define DLL_NAMESPACE namespace
		#else
			#define DLL_EXPORT __declspec(dllimport)
			#define DLL_CLASS	class DLL_EXPORT
//			#define DLL_NAMESPACE	namespace DLL_EXPORT
		#endif
	#endif
#else
	#define DLL_EXPORT	extern
	#define DLL_CLASS	class
//	#define DLL_NAMESPACE namespace
#endif

// #endif 
