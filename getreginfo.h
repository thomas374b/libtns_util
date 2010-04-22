/* 
 * Thomas Pantzer: getreginfo.h, Version: v0.1a  15.01.2006 12:31 
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

//static char SCCS_ID [] = "@(#) getreginfo.h \main\4 @(#)";
/******************************************************************************/
/*                                                                            */
/* FILE NAME :     getreginfo.h                                               */
/* VERSION :       \main\4                                                    */
/*                                                                            */
/* FUNCTION :      simple registry handling                                   */
/*                                                                            */
/* AUTHOR :        Thomas Pantzer                                             */
/* CREATION DATE : 13.11.2002                                                 */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* Copyright (C) Siemens AG 2001                                              */
/*----------------------------------------------------------------------------*/
/* HISTORY OF CHANGE                                                          */
/*----------------------------------------------------------------------------*/
/* VER| DATE | CHANGE                                       | AUTHOR          */
/*----------------------------------------------------------------------------*/
/* +++ \main +++                                            |                 */
/* 001|101201|                                              | pan             */
/* 001|131102| simple registry handling                     | pan             */
/* 002|181102| remote registry lesen                        | pan             */
/* 003|181102| ohne MFC                                     | pan             */
/* 004|191102| cleanup debugging directive                  | pan             */
/*    |      |                                              |                 */
/******************************************************************************/

#ifndef _GETREGINFO_H
#define _GETREGINFO_H

#include "tns_utils/win32_export.h"

DLL_EXPORT bool getRegOption(char *cSubKey, char *cName, char *cValue);
DLL_EXPORT bool getRegOption(char *cSubKey, char *cName, char *cValue, HKEY hkParent);

DLL_EXPORT bool writeRegOption(char *cSubKey, char *cName, char *value);
DLL_EXPORT bool writeRegOption(char *cSubKey, char *cName, char *cValue, HKEY hkParent);
DLL_EXPORT bool writeRegOption(char *cSubKey, char *cName, DWORD dwValue, HKEY hkParent);

DLL_EXPORT HRESULT getRegOption(wchar_t *subkey, wchar_t *name, wchar_t *value);
DLL_EXPORT HRESULT getRegOption(wchar_t *wcSubKey, wchar_t *wcName, wchar_t *csValue, wchar_t *wcMachine);



#undef DLL_EXPORT
#undef DLL_CLASS


#endif
