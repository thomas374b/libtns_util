/* 
 * Thomas Pantzer: mkopts.h, Version: v0.1a  16.02.2000 18:02 
 * 
 * 
 * Copyright 2000 by Thomas Pantzer
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
 * pantec@zorro.informatik.uni-leipzig.de   (Thomas Pantzer) 
 * 
 */



#ifndef _MK_OPTS_H
#define _MK_OPTS_H


typedef void (*t_arg_handler)(int i, double f, char *s);
typedef void (*t_unknown_arg_handler)(int i, char *v);

enum t_kind {
				e_unknown,
				e_boolean,
				e_integer,
				e_float, 
				e_string, 
				e_port,		/* portnumber/service argument, is string or number */
				e_hexnum,
			};

typedef struct {
		t_arg_handler handler;		

		t_kind kind;

		char flag;
		const char *longflag;
		const char *helptext;

		bool b;
		int i;
		double f;
		char s[256];

} t_opts;


#include "tns_util/win32_export.h"

TNS_UTIL_API void print_arg_defaults(char *argv0, t_opts opts[]);
TNS_UTIL_API void scan_args(int argc, char *argv[], t_opts opts[]);
TNS_UTIL_API void empty_func(int , double, char *);

TNS_UTIL_API char *str_tolower(char *src, char *buf);
// TNS_UTIL_API unsigned char Hex2Byte(char *S);


TNS_UTIL_API_EXTERN bool verbose_mode;

extern t_unknown_arg_handler unknown_arg_handler;



#endif
