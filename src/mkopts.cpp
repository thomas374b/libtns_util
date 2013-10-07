/* 
 * Thomas Pantzer: mkopts.cc, Version: v0.1a  16.02.2000 18:02 
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




#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "tns_util/porting.h"
#include "tns_util/readln.h"
#include "tns_util/mkopts.h"
#include "tns_util/utils.h"
#include "tns_util/netutils.h"

#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"

#ifndef ETC_PREFIX
	#define	ETC_PREFIX ""
#endif

#ifndef PREFIX
	#define PREFIX	"/opt"
#endif


bool verbose_mode;

t_unknown_arg_handler unknown_arg_handler = NULL;


void empty_func(int , double, char *)
{
}


void demo_unknown_arg_handler(int i, char *v)
{
}


bool flag_used(char F, t_opts opts[])
{
	int j = 0;
	while (opts[j].kind != e_unknown)	
	  {
	   if (opts[j].flag == F)
	      return true; 	 	

	   j++;	
	  }

	return false;
}

bool short_of(char F, char *argvi, t_opts opts[])
{
	if (flag_used(F,opts) == true)
	   return false;

	char test[8] = "-F\0";
	test[1] = F;

	if (strcmp(argvi,test) == 0)
	   return true;

	return false;
}


// 
// char *strip_slash(char *argv0)
// {
// 	char *s = strrchr(argv0,'/');
// 
// 	if (s != NULL)
// 	   return s+1;
// 
// 	return argv0; 
// }
// 
// 


void print_arg_defaults(char *argv0, t_opts opts[])
{
	int k = 0;

	fprintf(stderr,"%s options:\n",argv0);
	while (opts[k].kind != e_unknown)	
	  {
	   fprintf(stderr,"\t-%c, --%s,\t%s\t",opts[k].flag,opts[k].longflag,opts[k].helptext);
	   switch(opts[k].kind)
	     {
	      case e_boolean:
		if (opts[k].b == true)
		   fprintf(stderr,"(true)\n");
		 else
		   fprintf(stderr,"(false)\n");  
		break;

	      case e_integer:
		  case e_port:
		fprintf(stderr,"(%d)\n",opts[k].i);
		break;

	      case e_float:
		fprintf(stderr,"(%g)\n",opts[k].f);
		break;

	      case e_string:
		fprintf(stderr,"(%s)\n",opts[k].s);
		break;

	      default:
		fprintf(stderr,"\n");
		break;        
	     }

	   k++;
	  }


	if (flag_used('s',opts) == true)
	   fprintf(stderr,"\t    ");
	 else
	   fprintf(stderr,"\t-s, ");

	fprintf(stderr,"--saverc,\tsave opts to personal rcfile\t(~/.%src)\n",argv0);

	if (flag_used('v',opts) == true)
	   fprintf(stderr,"\t    ");
	 else
	   fprintf(stderr,"\t-v, ");
                        
	fprintf(stderr,"--verbose,\tbe verbose\t\t\t");

	if (verbose_mode == true)
	   fprintf(stderr,"(true)\n");
	 else
	   fprintf(stderr,"(false)\n");  

	if (flag_used('h',opts) == true)
	   fprintf(stderr,"\t    ");
	 else
	   fprintf(stderr,"\t-h, ");

	fprintf(stderr,"--help,\tprint this help screen and exit\n");
	fprintf(stderr,"\t    -V,   \tprint version and exit\n");
}


char open_opts_file(char *argv0, char *fname)
{
	sprintf(fname,"./.%src",argv0);

	if (fexist(fname) == true)
	    return true;

	char *home;
	home = getenv("HOME");

	if (home != NULL) {	
	    sprintf(fname,"%s/.%src",home,argv0);

		if (fexist(fname) == true) {
			return true;
		}
	    sprintf(fname,"%s/etc/%s.conf",home,argv0);

		if (fexist(fname) == true) {
			return true;
		}

	    sprintf(fname,"%s/share/%s/%src",home,argv0,argv0);

		if (fexist(fname) == true) {
			return true;
		}
	}

	sprintf(fname,ETC_PREFIX"/etc/%s.conf",argv0);
	if (fexist(fname) == true) {
	   return true;	
	}
	sprintf(fname,PREFIX"/share/%s/%src",argv0,argv0);
	if (fexist(fname) == true) {
	   return true;	
	}
	return false;
}



void save_manstyle_opts(char *argv0, t_opts opts[])
{
	char fname[256];
	
	sprintf(fname,"%s.manstyle-options",argv0);
	
	fileHandle fd = openFileMode(fname,O_RDWR|O_TRUNC|O_CREAT,0644);
	if (fd == INVALID_HANDLE_VALUE) {
		return;
	}	   
	   
	char st[1024];
	st[0] = '\0';
 	
	sprintf(st,"\n.\\\"\n.SH OPTIONS\n");
	writeFd(fd,st,strlen(st));   

	int k = 0;   
	while (opts[k].kind != e_unknown)	
	  {
	   switch(opts[k].kind)
	     {
	      case e_boolean:
		sprintf(st,".TP\n.B \\-\\^%c, \\fB-\\-\\^%s\\fR\n%s.\n",
			opts[k].flag,opts[k].longflag,opts[k].helptext);	   
		break;

	      case e_integer:
	      case e_port:
		sprintf(st,".TP\n.B \\fB\\-\\^%c\\fR \\fI<%s_number>\\fR\n%s.\n",
			opts[k].flag,opts[k].longflag,opts[k].helptext);	   
		break;

	      case e_float:
		sprintf(st,".TP\n.B \\fB\\-\\^%c\\fR \\fI<%s_float>\\fR\n%s.\n",
			opts[k].flag,opts[k].longflag,opts[k].helptext);	   
		break;

	      case e_string:
		sprintf(st,".TP\n.B \\fB\\-\\^%c\\fR \\fI<%s_string>\\fR\n%s.\n",
			opts[k].flag,opts[k].longflag,opts[k].helptext);	   
	      default:
		break;	
	     }

 	   writeFd(fd,st,strlen(st));
	   	
	   k++;
	  }

	sprintf(st,"\n.PP\n.TP\n.B \\-\\^s, \\fB\\-\\-\\^saverc\\fR\n"
	"Save opts to personal rcfile \\fI~/.%src\\fR .\n",argv0);
	writeFd(fd,st,strlen(st));  

	sprintf(st,".TP\n.B \\-\\^v, \\fB\\-\\-\\^verbose\\fR\n"
	"Be more verbose.\n");
	writeFd(fd,st,strlen(st));  

	sprintf(st,".TP\n.B \\-\\^h, \\fB\\-\\-\\^help\\fR\n"
	"Print a help screen with defaults and exit.\n");
	writeFd(fd,st,strlen(st));  
	
	sprintf(st,".TP\n.B \\-\\^V\n"
	"Print version and copyright information, then exit. "
	"To get detailed version and copyright information "
	"about used modules do a\n\n.BR\n"
	"strings %s | grep copyright: \\fB[ENTER]\\fR\n\n",argv0);
	writeFd(fd,st,strlen(st));  

	sprintf(st,".SH FILES\n.TP\n.B ~/.%src\n.TP\n.B ~/etc/%s.conf\n"
	           ".TP\n.B ~/share/%s/%src\n.TP\n.B /opt/etc/%s.conf\n"
		   ".TP\n.B /opt/share/%s/%src\n.PP\n",
		   argv0,argv0,argv0,argv0,argv0,argv0,argv0);
	writeFd(fd,st,strlen(st));

	sprintf(st,"\\fB%s\\fR looks at these locations to find its defaults. "
	           "If a file is found further searching is aborted. All "
		   "values can be overwritten by its corresponding commandline "
		   "options.\n.\\\"\n.SH ENVIRONMENT\nThe \\fBHOME\\fR variable "
		   "is used.\n", argv0);
	writeFd(fd,st,strlen(st));		   


	closeFd(fd);
	   	 	
}

void save_opts_file(char *argv0, t_opts opts[])
{
	int k = 0;

	char fname[256];

 	char *home = getenv("HOME");

 	if (home == NULL)
 	   return;

 	sprintf(fname,"%s/.%src",home,argv0);

	if (fexist(fname) == true)
	   sprintf(fname,"./.%src",argv0);

	if (verbose_mode == true)
	   fprintf(stderr,"saving options to %s\n",fname); 

	fileHandle fd = openFileMode(fname, O_RDWR|O_TRUNC|O_CREAT, 0640);

	if (fd <= 0)
	   {
	    perror("save options");	
	    return;
	   }

	while (opts[k].kind != e_unknown)	
	  {
	   char st[256];
	   st[0] = '\0';
 	
	   switch(opts[k].kind)
	     {
	      case e_boolean:
		sprintf(st,"%s\t%d\t\t\t#  %s\n",
			opts[k].longflag,opts[k].b,opts[k].helptext);	   
		break;

	      case e_integer:
		  case e_port:
		sprintf(st,"%s\t%d\t\t\t#  %s\n",
			opts[k].longflag,opts[k].i,opts[k].helptext);	   
		break;

	      case e_float:
		sprintf(st,"%s\t%g\t\t\t#  %s\n",
			opts[k].longflag,opts[k].f,opts[k].helptext);	   
		break;

	      case e_string:
		sprintf(st,"%s\t%s\n\t\t#  %s\n",
			opts[k].longflag,opts[k].s,opts[k].helptext);	   
		break;

	      default:
		break;	
	     }

 	   writeFd(fd,st,strlen(st));
	   	
	   k++;
	  }

	closeFd(fd);
}


int scan_integer(int *result, int argc, char *argv[], int i, char flag, const char *longflag)
{
	if (result == NULL) {
		return 0;
	}

	char c[256];	/* formatString */
	int d;

	/* angeklitscht */
	sprintf(c,"-%c0x%%x",flag);	
	if (sscanf(argv[i],c,&d) == 1) {
		*result = d;
		if (verbose_mode == true) {
			fprintf(stderr,"got argument %s from hex number := %d\n",longflag, *result);
		}
		return 1;
	} else {
		sprintf(c,"-%c%%d",flag);	
		if (sscanf(argv[i],c,&d) == 1) {
			*result = d;	
			if (verbose_mode == true) {
				fprintf(stderr,"got argument %s from int number := %d\n",longflag, *result);			
			}
			return 1;
		} 		     	
	}


	/* zwei argumente */	
	if (argc > i+1) {
		if (sscanf(argv[i+1],"0x%x",&d) == 1) {
			*result = d;
			if (verbose_mode == true) {
				fprintf(stderr,"got argument %s from hex number := %d\n",longflag, *result);
			}
			return 2;	
		} else {
			if (sscanf(argv[i+1],"%d",&d) == 1) {
				*result = d;
				if (verbose_mode == true) {
					fprintf(stderr,"got argument %s from int number := %d\n",longflag, *result);
				}
				return 2;
			}
		}
	} 
/*	
	fprintf(stderr,"arg [%s]\tformat [%s]\tgot %d\n",argv[i],c,i);
*/	 
	return 0;	
}





void scan_opts_file(char *argv0, t_opts opts[])
{
	char fname[256];

	int fd = open_opts_file(argv0,fname);

	if (fd <= 0)
	   return;

	t_buffer *FBuf = new t_buffer;

	if (FBuf->Init(fname,8192) != true)
	   {
	    delete FBuf;	
	    return;
	   }
	
	if (verbose_mode == true)
	   fprintf(stderr,"using options from %s\n",fname); 

	char *st = FBuf->ReadLn();

	while (st != NULL)
	  { 
 	   int j = 0;

//	   fprintf(stderr,"(%s)\n",st);
 
	   while (opts[j].kind != e_unknown)	
	     {
	      int i;	
//	      char b;
	      char format[256]; 		
	      float f;
		
	      char *s = strchr(st,'#');	
	      if (s != NULL)
		 *s = '\0';
	
	      switch (opts[j].kind)
		{	
		 case e_boolean:
		   sprintf(format,"%s %%d",opts[j].longflag);  	
		   if (sscanf(st,format,&i) == 1)
		       opts[j].b = (i>0);
		   break;			

		 case e_integer:
		 case e_port:
		   sprintf(format,"%s %%d",opts[j].longflag);  	
		   if (sscanf(st,format,&i) == 1)
		      opts[j].i = i;
		   break;			

		 case e_float:
		   sprintf(format,"%s %%f",opts[j].longflag);  	
		   if (sscanf(st,format,&f) == 1)
		      opts[j].f = f;
		   break;			

		 case e_string:
		   {
		    char cmp[256];
		    strncpy(cmp,st,240);	

 		    char *v = strchr(cmp,'\t');
 		    if (v == NULL)
		       break;

 		    *v = '\0';
 		    v++;

		    if (strcmp(opts[j].longflag,cmp) == 0)
			strcpy(opts[j].s,v); 	
		   }
		   break;			

		 default:
		   break;		
		}			     

	      j++;	
	     }

	   st = FBuf->ReadLn();	
	  }

	FBuf->Done();
	delete FBuf;
}

 

void scan_args(int argc, char *argv[], t_opts opts[])
{
	verbose_mode = false;

	int unknown_args = 0;
	bool bIsStr = false;

	scan_opts_file(strip_slash(argv[0]),opts);

	for (int i=1; i<argc; i++) {
		int j = 0;

//	   fprintf(stderr,"%d of %d: %s\n",i,argc-1,argv[i]);
		char c[256];
		bool got = false;	
	   	bool t3,t4;
		bool t1 = short_of('h',argv[i],opts);
		bool t2 = (strcmp(argv[i],"--help") == 0);	
	
		if (t1 || t2) {
			print_arg_defaults(strip_slash(argv[0]),opts); 
//	       continue;	
			exit(1);
		}		      	
#if _WINDOWS|WIN32
#else
		if (strcmp(argv[i],"-V") == 0) {
//	       fprintf(stderr,"%s %s%s",strip_slash(argv[0]),version_string,revision_date);
			fprintf(stderr,"%s\n",copyright_info);	
			exit(0);
		} 
#endif
		t1 = short_of('v',argv[i],opts);
		t2 = (strcmp(argv[i],"--verbose") == 0);   
	
		if (t1 || t2) {
			verbose_mode = true;
			fprintf(stderr,"entered verbose mode\n");
			continue;
		} 
		if (strcmp(argv[i],"--manstyle") == 0) {
			save_manstyle_opts(strip_slash(argv[0]),opts);
			fprintf(stderr,"hidden feature: man-style options section created\n");
			exit(0);
		} 
		t1 = short_of('s',argv[i],opts);
		t2 = (strcmp(argv[i],"--saverc") == 0);

		if (t1 || t2) {
			save_opts_file(strip_slash(argv[0]),opts);	
			continue;	
		}
		while (opts[j].kind != e_unknown) {
			got = false;
	      
			sprintf(c,"-%c",opts[j].flag);
			// t1 = ...
			if (opts[j].kind == e_boolean) {
				t1 = (strcmp(argv[i],c) == 0);	
  			
				// negation of previously set boolean arguments
				sprintf(c,"!-%c",opts[j].flag);
				t3 = (strcmp(argv[i],c) == 0);	
  			
				sprintf(c,"!--%s",opts[j].longflag);
				t4 = (strcmp(argv[i],c) == 0);	
			} else {
				t1 = (strncmp(argv[i],c,strlen(c)) == 0);	
				t4 = t3 = false;
			}
			sprintf(c,"--%s",opts[j].longflag);
			t2 = (strcmp(argv[i],c) == 0);	

			if (t1 || t2 || t3 || t4) {
//				int d;
				float f;
				char s[256];
  	
				if (opts[j].b == true) {	// toggle the default value
					opts[j].b = false;
				} else {
					opts[j].b = true;	  	
				}
//				if (verbose_mode == true) {
//					fprintf(stderr,"%d\t%s\n",j,opts[j].helptext);
//				}
				switch (opts[j].kind) {
					case e_boolean:
						if (t3 || t4) {		// exclamation sign was given
							opts[j].b = false;	// boolean arg will be set to false
 						}
						if (verbose_mode == true) {
							fprintf(stderr,"got argument %s := %s\n",opts[j].longflag,opts[j].b?"true":"false");
						}
						got = true;
						break;			
						
					case e_port:
						{
							bIsStr = false;
							switch (scan_integer(&(opts[j].i), argc, argv, i, opts[j].flag, opts[j].longflag)) {
								case 2:
									i++;
									/* fall thru */
								case 1:
									got = true;
									break;
								default:
								case 0:
									sprintf(c,"-%c%%s",opts[j].flag);	
									if (sscanf(argv[i],c,&s) == 1) {
										strcpy(opts[j].s,s);
//										opts[j].i++;
										got = true;
										bIsStr = true;
									} 		     	
					       			if ((argc > i+1) && (got == false)) {
										if (sscanf(argv[i+1],"%s",&s[0]) == 1) {
											strcpy(opts[j].s,s);
//											opts[j].i++;
											got = true;
											bIsStr = true;
											i++;
										}
									} 
									if (bIsStr == true) {
#if _WINDOWS|WIN32
										opts[j].i = 0;	// TODO	
#else
										opts[j].i = GetTCPPort(s,0);
#endif
										if (opts[j].i == 0) {
											fprintf(stderr,"unknown service description \"%s\" from arg %s\n",opts[j].s, opts[j].longflag);
										} else {				
											if (verbose_mode == true) {
												fprintf(stderr,"got argument %s := %d from service description string \"%s\"\n",opts[j].longflag,opts[j].i,opts[j].s);
											}				
										}	
									} else {
										opts[j].i = 0;
									}
									break;
							}		
							if (opts[j].i == 0) {
								fprintf(stderr,"unspecific port number %d from arg %s=\"%s\"\n",opts[j].i,opts[j].longflag,opts[j].s);
							}
						}
			 			break;
						
					case e_integer:
						switch (scan_integer(&(opts[j].i), argc, argv, i, opts[j].flag, opts[j].longflag)) {
							case 2:
								i++;
								/* fall thru */
							case 1:
								got = true;
								break;
							default:
							case 0:
								break;
						}
/*
						sprintf(c,"-%c%%d",opts[j].flag);	
						if (sscanf(argv[i],c,&d) == 1) {
							opts[j].i = d;
							if (verbose_mode == true)
								fprintf(stderr,"got argument %s := %d\n",opts[j].longflag,opts[j].i);
							got = true;
						}
						if ((argc > i+1) && (got == false)) {
							if (sscanf(argv[i+1],"%d",&d) == 1) {
								opts[j].i = d;
								if (verbose_mode == true)
									fprintf(stderr,"got argument %s := %d\n",opts[j].longflag,opts[j].i);
								got = true;
								i++;	// skip next
							}
						} 		     	
*/
						break;			

					case e_float:
						sprintf(c,"-%c%%f",opts[j].flag);	
						if (sscanf(argv[i],c,&f) == 1) {
							opts[j].f = f;
							if (verbose_mode == true)
								fprintf(stderr,"got argument %s := %f\n",opts[j].longflag,opts[j].f);
							got = true;
						} 		     	
						if ((argc > i+1) && (got == false)) {
							if (sscanf(argv[i+1],"%f",&f) == 1) {
								opts[j].f = f;
								if (verbose_mode == true)
									fprintf(stderr,"got argument %s := %f\n",opts[j].longflag,opts[j].f);
								got = true;
								i++;	
							}
						} 		     	
						break;			

		    		 case e_string:
						sprintf(c,"-%c%%s",opts[j].flag);	

						if (sscanf(argv[i],c,&s) == 1) {
							strcpy(opts[j].s,s);
							opts[j].i++;

							if (verbose_mode == true)
								fprintf(stderr,"got argument %s := %s\n",opts[j].longflag,opts[j].s);
							got = true;
						} 		     	
						if ((argc > i+1) && (got == false)) {
							if (sscanf(argv[i+1],"%s",&s[0]) == 1) {
								strcpy(opts[j].s,s);
								opts[j].i++;
								if (verbose_mode == true)
									fprintf(stderr,"got argument %s := %s\n",opts[j].longflag,opts[j].s);
								got = true;
								i++;
							}
						} 		     	
						break;			

					default:
						break;		
				}			     
			}
			if (got == true) {
				opts[j].handler(opts[j].i, opts[j].f, opts[j].s);				  
//				if (verbose_mode == true)
//					fprintf(stderr,"%d\t%s\n",j,opts[j].helptext);
				break;		    	
			} 
			j++;
		}
		if (opts[j].kind == e_unknown) {
			if (unknown_arg_handler != NULL) {
				if (verbose_mode == true) {
					fprintf(stderr,"user handler called for %dth unkown argument argv[%d]=(%s)\n",
									unknown_args+1,i,argv[i]);
				}	
				unknown_arg_handler(unknown_args,argv[i]);	
			} else {
				print_arg_defaults(strip_slash(argv[0]),opts); 
				return;
			} 
			unknown_args++;	
		}	
	} 
}



