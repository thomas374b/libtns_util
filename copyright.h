/* 
 * Thomas Pantzer: copyright.h, v0.1b  08.05.2002 23:07 
 * 
 * 
 * Copyright 2002 by Thomas Pantzer
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
 * Thomas@Pantzer.net   (Thomas Pantzer,,,) 
 * 
 */





#ifndef _COPYRIGHT_H
#define _COPYRIGHT_H


#ifndef AUTHOR
#define AUTHOR "Thomas Pantzer, <Thomas at/@ Pantzer dot/. net>"
#endif

#ifdef MAIN_G
	/* char version_string[] = "version: \t"VERSION"\n\0";	*/
	/* char revision_date[] = "last revision: \t"REVISION"\n\0"; */

char copyright_info[] =
	TARGETNAME" v"VERSION", rev. "REVISION",   copyright: "AUTHOR"\n\0";

char compiler_info[] =
	"host: "COMPILER_HOST", compiler: "COMPILER_EXE", version: "COMPILER_VERSION"\n\0";

#else
#ifdef SHOW_COMPILER_MOD
	static char compiler_sss[] = 
	"compiler: "COMPILER_EXE" "COMPILER_VERSION" @ "COMPILER_HOST"\n\0";

#else
	extern char compiler_info[];
#endif	// SHOW_COMPILER_MOD
	/* extern char version_string[]; */
	/* extern char revision_date[]; */
extern char copyright_info[];

static char copyright_string[] = 
	MODNAME" v"VERSION", rev. "REVISION", copyright: "AUTHOR"\n\0";


#endif	// MAIN_G

#endif	// _COPYRIGHT_H
