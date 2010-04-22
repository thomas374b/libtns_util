/* 
 * Thomas Pantzer: tns_util.cpp, Version: v0.1a  15.01.2006 12:31 
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

static char SCCS_ID [] = "@(#) tns_util.cpp \\main\\2 @(#)";
/******************************************************************************/
/*                                                                            */
/* FILE NAME :     tns_util.cpp                                               */
/* VERSION :       \main\2                                                    */
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
/* 002|131102| Windows-spezifische Portierung               | pan             */
/*    |      |                                              |                 */
/******************************************************************************/
// tns_util.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <winsock.h>
#include <stdio.h>

#include "tns_util/tns_util.h"
#include "tns_util/utils.h"
#include "tns_util/mkopts.h"



BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err; 
	
	switch (ul_reason_for_call)	{
		case DLL_PROCESS_ATTACH:
			wVersionRequested = MAKEWORD( 2, 0 ); 
			err = WSAStartup( wVersionRequested, &wsaData );
			if ( err != 0 ) {
				/* Tell the user that we could not find a usable */
				/* WinSock DLL.                                  */    
				ErrorMsg("WSAStartup");
				return FALSE;
			}
			//			if (verbose_mode == true)
#ifdef _DEBUG
			fprintf(stderr,"using winsock version %d.%d\n",
			LOBYTE( wsaData.wVersion ),HIBYTE( wsaData.wVersion ));
#endif
			break;
		
		case DLL_THREAD_ATTACH:
			case DLL_THREAD_DETACH:
				break;
			
			case DLL_PROCESS_DETACH:
				WSACleanup();
				break;
	}
	return TRUE;
}
	
	
// This is an example of an exported variable
TNS_UTIL_API int nTns_util=0;
	
// This is an example of an exported function.
TNS_UTIL_API int fnTns_util(void)
{
	return 42;
}
	
// This is the constructor of a class that has been exported.
// see tns_util.h for the class definition
CTns_util::CTns_util()
{ 
	return; 
}
	
