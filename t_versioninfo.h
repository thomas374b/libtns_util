/* 
 * Thomas Pantzer: t_versioninfo.h, Version: v0.1a  09.11.2000 20:50 
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
 * Thomas.Pantzer@apoll.de   (Thomas Pantzer,,,) 
 * 
 */



#ifndef T_VERSION_INFO_H
#define T_VERSION_INFO_H



#include <stdio.h>

class t_version_info {
	bool assigned;
	int major,minor,patch,build;
	char String[128];
	
public:
	t_version_info() {assigned = false; major = minor = patch = build = 0;};
	~t_version_info() {assigned = false;};
	
	void Init(int m, int i, int p, int b);
	bool Init(char *st);
	bool isHigherOrEqual(t_version_info *vi);	
	char *GetString(void);
};



#endif


