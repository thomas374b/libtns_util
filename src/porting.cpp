

#include <stdio.h>

#include "tns_util/porting.h"

#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"

char *winErrorStr(DWORD dwErr, char *buffer)
{
	if (buffer == NULL) {
		return buffer;
	}

	LPVOID lpMsgBuf;
	lpMsgBuf = NULL;
	
	DWORD dwRC = FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			 NULL,
			dwErr, //  from GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL);
		
	if (dwRC && lpMsgBuf) {
		sprintf(buffer,"Error: %d\t%s",dwErr, lpMsgBuf);
	} else {
		sprintf(buffer,"Error: %d\t%s",dwErr, lpMsgBuf);
	}
	return buffer;
}

int lseekFd(fileHandle fd, int offs, int whence) 
{
	DWORD dwMoveMethod = FILE_CURRENT;

	if (whence == SEEK_END) {
		dwMoveMethod = FILE_END;
	} else {
		if (whence == SEEK_SET) {
			dwMoveMethod = FILE_BEGIN;
		}
	}
	
	int newPos = SetFilePointer(fd, offs, 0, dwMoveMethod);
	if (newPos == INVALID_SET_FILE_POINTER) {
		DWORD dwError = GetLastError();
		if (dwError != NO_ERROR) {
			char buf[512];
			fprintf(stderr, "lseekFd() failed: %s", winErrorStr(dwError,buf));
			return -1;
		}
	}

	return newPos;
}



fileHandle openFd(char *name, int mode)
{
	DWORD dwError;
/*
#define _O_RDONLY       0x0000  // open for reading only 
#define _O_WRONLY       0x0001  // open for writing only
#define _O_RDWR         0x0002  // open for reading and writing 
#define _O_APPEND       0x0008  // writes done at eof 

#define _O_CREAT        0x0100  // create and open file 
#define _O_TRUNC        0x0200  // open and truncate 
#define _O_EXCL         0x0400  // open only if file doesn't already exist
*/
	unsigned long perm = GENERIC_READ;
	unsigned long create = OPEN_EXISTING;

	if (mode & O_WRONLY) {
		perm = GENERIC_WRITE;
	}

	if (mode & O_RDWR) {
		perm = (GENERIC_READ|GENERIC_WRITE);
	}
	
	if (mode & O_CREAT) {
		create = CREATE_ALWAYS;
	}
	
	if (mode & O_TRUNC) {
		create = OPEN_ALWAYS;
	}

	if (mode & O_EXCL) {
		create = CREATE_NEW;
	}	
/*
#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5
*/
	fileHandle fd = CreateFile(name, perm, 
		0, // exclusive access
		NULL, // no security
		create,
		0, // no overlapped I/O
		NULL); // null template
 
	BOOL	m_bPortReady = true;	
	char buf[512];

	if (fd == INVALID_HANDLE_VALUE) {
		m_bPortReady = false;
		dwError = GetLastError();
		fprintf(stderr, "\nCreateFile failed: filename=\"%s\" mode=x%04x p/c:%d/%d\t%s\n",	name, mode, perm, create, winErrorStr(dwError,buf));
	}
	
	if (mode & O_TRUNC) {
		if (SetEndOfFile(fd) == 0) {
			dwError = GetLastError();
			fprintf(stderr, "truncate file \"%s\" failed: %s", name, winErrorStr(dwError,buf));
		}
	}

	if (mode & O_APPEND) {
		SetFilePointer(fd, 0, 0, FILE_END);
	}

	return fd;
}

int readFd(fileHandle fd, char *buffer, int len) 
{
	BOOL	bReadRC;
	DWORD	iBytesRead;
	memset(buffer, 0, len);
	bReadRC = ReadFile(fd, buffer, len, &iBytesRead, NULL);
	if (bReadRC && iBytesRead > 0) {
		return (int)iBytesRead;
	}
	DWORD dwError = GetLastError();
	if (dwError != 0) {
		char buf[512];
		fprintf(stderr, "%s\n",	winErrorStr(dwError, buf));
//		fprintf(stderr, "Read length failed: RC=%d Bytes read=%d, Error=%d ",	bReadRC, iBytesRead, dwError);
	}
	return -1;
}


int writeFd(fileHandle fd, char *buffer, int len) 
{
	BOOL	bWriteRC;
	DWORD	iBytesWritten;
	bWriteRC = WriteFile(fd, buffer, len, &iBytesWritten, NULL);
	if (bWriteRC && iBytesWritten > 0) {
		return (int)iBytesWritten;
	}

	DWORD dwError = GetLastError();
	char buf[512];
	if ((bWriteRC == 0) && (iBytesWritten == 0) && (dwError == 995)) {
//		fprintf(stderr, "%s\n",	winErrorStr(dwError,buf));
		return 0;
	}
	fprintf(stderr, "writeFd() failed: RC=%d Bytes Written=%d, %s\n",	bWriteRC, iBytesWritten, winErrorStr(dwError,buf));
//	exit(-2);
	return -1;	
}


int getTimeOfDay(struct timeVal *tv, struct timeZone *tz)
{
	time_t ltime;
	time(&ltime);
	tv->tv_sec = (int)ltime;
	tv->tv_usec = (GetTickCount() % 1000) * 1000;
	return 0;
}

