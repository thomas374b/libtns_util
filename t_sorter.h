/* 
 * Thomas Pantzer: t_sorter.h, v0.1b  24.11.98 10:32 
 * 
 * 
 * Copyright 1998 by Thomas Pantzer
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
 * pantec@aix520.informatik.uni-leipzig.de   (Thomas Pantzer) 
 * 
 */

#ifndef T_SORTER_H
#define T_SORTER_H


#ifdef T_SORTER_G
# define vextern
#else
# define vextern extern
#endif


typedef double (*t_realfunc)(void *);

class t_sorter {
public:
  double max, min;
  bool ascending;	

  void Init(void **, t_realfunc, int nElements, bool asc);
  void Done(void);
  
  double Average(void);
  void Run(void);
  void Run(int start);
  void *MaxElem(void);
  double Max(void);
  
  void *MinElem(void);
  double Min(void);


private:
  void **A;
  void **T0,**T1;
  int size;
  t_realfunc valfunc;

  double Average(int, int);
  double value(int);
  char higher(int, double);
  void sort(int, int);
};


#undef vextern

#endif /*T_SORTER_H*/


