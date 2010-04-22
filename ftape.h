/* 
 * Thomas Pantzer: ftape.h, v0.1b  08.05.2002 23:07 
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







#ifndef T_FILETAPE_H
#define T_FILETAPE_H

// filetape -- choose filenames without user interaction
//
// just as with a magnetic tape, but more advanced and ability to record
// a file without fear to destroy previous played files

// New() returns always a filename that doesn't already exist
// Next() returns a filename with "running number" higher than in previous name
// Prev() same but with lower "running number" 

class t_filetape {
private:
	char *_check(char *buf, char cond, int inc, int p);
	int size,pos,ext_cnt;
	char *basepath;
	char *track;
	char **ext;	

public:
	void Init(int size, char *basepath, char *track, char *ext);
	void AddExt(char *ext);
	void Done(void);
	
	char *Next(char *b) { return _check(b,true,1,pos); 			};
	char *Next(char *b,int p) { return _check(b,true,1,p); 			};
	char *Prev(char *b) { return _check(b,true,size-1,pos+size-1); 	};
	char *New(char *b)  { return _check(b,false,1,0); 				};

	char *Next(void) { return _check(0,true,1,pos); 			};
	char *Prev(void) { return _check(0,true,size-1,pos+size-1); };
	char *New(void)  { return _check(0,false,1,0); 				};
	int Pos(void) { return pos; };
};



#endif
