/* 
 * Thomas Pantzer: getreginfo.cpp, Version: v0.1a  15.01.2006 12:31 
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

static char SCCS_ID [] = "@(#) getreginfo.cpp \\main\\6 @(#)";
/******************************************************************************/
/*                                                                            */
/* FILE NAME :     getreginfo.cpp                                             */
/* VERSION :       \main\6                                                    */
/*                                                                            */
/* FUNCTION :                                                                 */
/*                                                                            */
/* AUTHOR :        Thomas Pantzer                                             */
/* CREATION DATE : 10.12.2001                                                 */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* Copyright (C) Siemens AG 2001                                              */
/*----------------------------------------------------------------------------*/
/* HISTORY OF CHANGE                                                          */
/*----------------------------------------------------------------------------*/
/* VER| DATE | CHANGE                                       | AUTHOR          */
/*----------------------------------------------------------------------------*/
/* +++ \main +++                                            |                 */
/* +++ \main\api27 +++                                      |                 */
/* 001|101201|                                              | pan             */
/* 002|260302|                                              | pan             */
/* 001|131102| simple registry handling                     | pan             */
/* 002|131102|                                              | pan             */
/* 003|181102| remote registry lesen                        | pan             */
/* 004|181102|                                              | pan             */
/* 005|191102| cleanup debugging directive                  | pan             */
/* 006|191102| map ERROR_xxx to proper HRESULTs             | pan             */
/*    |      |                                              |                 */
/******************************************************************************/

#include <afx.h>

#ifndef MODNAME		
	#define MODNAME		__FILE__
#endif
#ifndef VERSION		
	#define VERSION		"\\main\\1"
#endif

#include "tns_utils/copyright.h"
		mk_CopyrightString(MODNAME);


#define MODULE_G
#include "tns_util/getRegInfo.h"

#include <atlbase.h>


HRESULT getRegOption(wchar_t *wcSubKey, wchar_t *wcName, wchar_t *wcValue, wchar_t *wcMachine)
{
	if ((wcSubKey == NULL) || (wcName == NULL) || (wcMachine == NULL) || (wcValue == NULL)) {
		return E_POINTER;
	}
	if ((wcslen(wcSubKey) == 0) || (wcslen(wcName) == 0) || (wcslen(wcMachine) == 0)) {
		return E_INVALIDARG;
	}
	HKEY hKey;
	DWORD dwRet = RegConnectRegistry(wcMachine,HKEY_LOCAL_MACHINE, &hKey);
	switch (dwRet) {			// map ERROR_xxx to proper HRESULTs
		case ERROR_SUCCESS:
			break;

		case ERROR_BAD_NETPATH:	
			return RPC_E_REMOTE_DISABLED;

		default:				
#ifdef _DEBUG
			fwprintf(stderr,L"connect remote registry \"%s\" failed, dwRet: %d\n",wcMachine,dwRet);
#endif
			return E_FAIL;
	}
	CRegKey		CKey;
	dwRet = CKey.Open(hKey,wcSubKey,KEY_QUERY_VALUE);
	if (dwRet != ERROR_SUCCESS) {
#ifdef _DEBUG
		fwprintf(stderr,L"open key \"%s\" failed, dwRet %d\n",wcSubKey,dwRet);
#endif
		return E_FAIL;
	}
	unsigned long rLen = 1536;
	dwRet = CKey.QueryValue(wcValue,wcName,&rLen);
	CKey.Close();
	
	if (dwRet != ERROR_SUCCESS) {
		wcValue[0] = '\0';	// empty the string
	}
	
	HRESULT hr;
	switch(dwRet) {		// map ERROR_xxx to proper HRESULTs
		case ERROR_SUCCESS:
			hr = S_OK;
			break;

		case ERROR_FILE_NOT_FOUND:  // does not exists  
			hr = NTE_NO_KEY;
			break;

		default:
#ifdef _DEBUG
			fwprintf(stderr,L"query key \"%s\", value \"%s\" failed\n",wcSubKey,wcName);
#endif
			hr = E_FAIL;
			break;
	}
	RegCloseKey(hKey);
	return hr;
}


HRESULT getRegOption(wchar_t *wcSubKey, wchar_t *wcName, wchar_t *wcValue)
{
	if ((wcSubKey == NULL) || (wcName == NULL) || (wcValue == NULL)) {
		return E_POINTER;
	}
	if ((wcslen(wcSubKey) == 0) || (wcslen(wcName) == 0)) {
		return E_INVALIDARG;
	}
	CRegKey		CKey;
	DWORD dwRet = CKey.Open(HKEY_LOCAL_MACHINE,wcSubKey,KEY_QUERY_VALUE);
	if (dwRet != ERROR_SUCCESS) {
#ifdef _DEBUG
		fwprintf(stderr,L"open key \"%s\" failed, dwRet %d\n",wcSubKey,dwRet);
#endif
		return E_FAIL;
	}
	unsigned long rLen = 1536;
	dwRet = CKey.QueryValue(wcValue,wcName,&rLen);
	CKey.Close();
	if (dwRet != ERROR_SUCCESS) {
#ifdef _DEBUG
		fwprintf(stderr,L"query key \"%s\", value \"%s\" failed\n",wcSubKey,wcName);
#endif
		wcValue[0] = '\0';			// empty the string
		return E_FAIL;
	}
	return S_OK;
}

bool getRegOption(char *cSubKey, char *cName, char *cValue)
{
	return getRegOption(cSubKey,cName,cValue,HKEY_LOCAL_MACHINE);
}

bool getRegOption(char *cSubKey, char *cName, char *cValue, HKEY hkParent)
{
	if ((cValue == NULL) || (cSubKey == NULL) || (cName == NULL)) {
		return false;
	}
	if ((strlen(cSubKey) == 0) || (strlen(cName) == 0)) {
		return false;
	}
	CRegKey		CKey;
	USES_CONVERSION;
	wchar_t *wcSubKey = A2W(cSubKey);

	if (CKey.Open(hkParent,wcSubKey,KEY_QUERY_VALUE) != ERROR_SUCCESS) {
#ifdef _DEBUG
		fwprintf(stderr,L"open key \"%s\" failed\n",wcSubKey);
#endif
		return false;
	}

	unsigned long rLen = 1536;
	wchar_t wcValue[2048];
	wchar_t *wcName = A2W(cName);
	unsigned long R = CKey.QueryValue(wcValue,wcName,&rLen);
	DWORD dwValue;

    // from the last query one get the length of a possible string, but the only string 
    // with length == 2 is the empty string; all other contents got a Count greater than 2,
    // but this method is neccessary, because empty strings will be accepted as numbers
    if (rLen == 2) {
	    // we've got the registry value; it's an empty string
		strcpy(cValue,"\0");
	} else {
		// here it is possible that the key value can be a DWORD or an String
		R = CKey.QueryValue(dwValue, wcName);
		if (R == ERROR_SUCCESS) {
			// yes, it's a number
			sprintf(cValue,"%d",dwValue);
		} else {
			// lets try for a string
			rLen = 1536;
			R = CKey.QueryValue(wcValue, wcName, &rLen);
			if (R == ERROR_SUCCESS) {
				// we've got the registry value
				strcpy(cValue,W2A(wcValue));
			} else {
				// thats an error
			}
		}
	}

	CKey.Close();

	if (R != ERROR_SUCCESS) {
#ifdef _DEBUG
		fwprintf(stderr,L"query key \"%s\", value \"%s\" failed\n",wcSubKey,wcName);
#endif
		cValue[0] = '\0';		// empty the string
		return false;
	}
	return true;
}	




class MyCRegKey : public CRegKey {
public:
	LONG CreateOrOpen(HKEY hkParent, char *cSubKey, char *cName) {
		USES_CONVERSION;
		wchar_t *wcSubKey = A2W(cSubKey);
		wchar_t *wcName = A2W(cName);
		
		if ((cSubKey == NULL) || (cName == NULL)) {
#ifdef _DEBUG
			fwprintf(stderr,L"arguments are NULL\n");
#endif
			return ERROR_BAD_FORMAT;
		}
		if ((strlen(cSubKey) == 0) || (strlen(cName) == 0)) {
#ifdef _DEBUG
			fwprintf(stderr,L"string lengths are empty\n");
#endif
			return ERROR_BAD_FORMAT;
		}

		LONG lRes = CRegKey::Create( hkParent, wcSubKey, wcName); 
		if (lRes != ERROR_SUCCESS) {
#ifdef _DEBUG
			fwprintf(stderr,L"create key \"%s\", value \"%s\" failed with %ld\n",wcSubKey,wcName,lRes);
#endif
		}
		return lRes;
	}
	long SetValue(char *cValue, char *cName) {
		USES_CONVERSION;
		wchar_t *wcValue = A2W(cValue);
		wchar_t *wcName = A2W(cName);
		return CRegKey::SetValue(wcValue,wcName);
	}
	long SetValue(DWORD dwValue, char *cName) {
		USES_CONVERSION;
		wchar_t *wcName = A2W(cName);
		return CRegKey::SetValue(dwValue,wcName);
	}
};


bool writeRegOption(char *cSubKey, char *cName, char *cValue)
{
	return writeRegOption(cSubKey, cName, cValue, HKEY_LOCAL_MACHINE);
}

bool writeRegOption(char *cSubKey, char *cName, char *cValue, HKEY hkParent)
{
	MyCRegKey		CKey;

	LONG lRes = CKey.CreateOrOpen( hkParent, cSubKey, cName); 
	if (lRes != ERROR_SUCCESS) {
		return false;
	}

	lRes = CKey.SetValue( cValue, cName );
	if (lRes != ERROR_SUCCESS) {
#ifdef _DEBUG
		fprintf(stderr,"set value \"%s\" failed\n",cName);
#endif
		return false;
	}
	CKey.Close();
	return true;
}	


bool writeRegOption(char *cSubKey, char *cName, DWORD dwValue, HKEY hkParent)
{
	MyCRegKey		CKey;

	LONG lRes = CKey.CreateOrOpen( hkParent, cSubKey, cName); 
	if (lRes != ERROR_SUCCESS) {
		return false;
	}

	lRes = CKey.SetValue( dwValue, cName );
	if (lRes != ERROR_SUCCESS) {
#ifdef _DEBUG
		fprintf(stderr,"set value \"%s\" failed\n",cName);
#endif
		return false;
	}
	CKey.Close();
	return true;
}	
