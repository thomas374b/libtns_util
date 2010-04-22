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


typedef void (*t_arg_handler)(int i, float f, char *s);
typedef void (*t_unknown_arg_handler)(int i, char *v);


enum t_kind {	e_unknown, 
				e_boolean, 
				e_integer, 
				e_float, 
				e_string, 
				e_port		/* portnumber/service argument, is string or number */ 
			};

typedef struct {
		t_arg_handler handler;		

		t_kind kind;

		char flag;
		char *longflag;
		char *helptext;

		char b;
		int i;
		float f;
		char s[256];

	       } t_opts;


extern void print_arg_defaults(char *argv0, t_opts opts[]);
extern void scan_args(int argc, char *argv[], t_opts opts[]);
extern void empty_func(int , float, char *);
extern char verbose_mode;


extern t_unknown_arg_handler unknown_arg_handler;



#endif
