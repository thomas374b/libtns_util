/* 
 * Thomas Pantzer: t_Set.h, v0.1b  16.1.2001 3:27 
 * 
 * 
 * Copyright 2001 by Thomas Pantzer
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
 * thomas@pantzer.net   (Thomas Pantzer) 
 * 
 */


#ifndef _T_SET_H
#define _T_SET_H

//other includes go here
class t_Set {
private:
    unsigned int size;
    int elements;

public:
    unsigned char *Flags;

	void Init(unsigned int n);
	virtual ~t_Set() {
		size = 0;
		elements = 0;
		delete Flags;
	};
      	 
	void Increase(unsigned int p);

	void Add(unsigned int p);
	void Del(unsigned int p);
	void Toggle(unsigned int p) {
		if (Contains(p)) {
			Del(p);
		} else {
			Add(p);
		}
	};
	void Invert(void);
	void And(t_Set *s);
	void Or(t_Set *s);

    bool Contains(unsigned int p);
//    int _Index(unsigned int p) {		// who is using this	
//			return Value(p); };			
	
    int Position(unsigned int p);	// position of element p	
	int Value(unsigned int k);		// value of n-th element in set		
	int Members(void); 	 
      
	void Print(void);
	char *Print(char *st);
	void Clear(void);
	void Fill(void);
};



#ifdef _T_SET_G
#define vextern
#else
#define vextern extern
#endif

//static variables go here


#undef vextern
#endif
