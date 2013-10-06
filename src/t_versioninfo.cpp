/* 
 * Thomas Pantzer: t_versioninfo.cpp, Version: v0.1a  09.11.2000 20:50 
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




#define IMPLEMENT_T_VERSION_INFO_H


#include "tns_util/t_versioninfo.h"
#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"


void t_version_info::Init(int m, int i, int p, int b) 
{ 
	assigned = true; 

	major = m; 
	minor = i; 
	patch = p; 
	build = b;
	
	sprintf(String,"%d.%d.%d.%d",major, minor, patch, build);
};	


bool t_version_info::Init(char *st)
{
	if (st == NULL)
		return false;
		
	int m,i,p,b;
	
	if (sscanf(st,"%d.%d.%d.%d",&m,&i,&p,&b) == 4) {
		Init(m,i,p,b);

		return true;
	}	
	
	return false;
}

char *t_version_info::GetString()
{																  
		return &String[0];
}																  
																  

bool t_version_info::isHigherOrEqual(t_version_info *vi)
{
	if (assigned == false)		// I'm not correct initialized
		return false;
				
	if (vi == NULL)
		return true;			// I'm always higher than null objects
		
	if (vi->assigned == false)
		return true;			// I'm always higher than uninitialized objects
		
	if (major > vi->major)
		return true;

	if (major < vi->major)
		return false;

	// majors are equal			
	if (minor > vi->minor)
		return true;

	if (minor < vi->minor)
		return false;
	
	// minors are equal		
	if (patch > vi->patch)
		return true;

	if (patch < vi->patch)
		return false;
	
	// patchlevels are equal			
	if (build > vi->build)
		return true;

	if (build < vi->build)
		return false;

	// buildnumbers are equal
	return true;			
}	

