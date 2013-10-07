

#ifndef _PORTING_H_
#define _PORTING_H_

#ifdef __cplusplus
	extern "C" {
#endif


#if _WINDOWS | WIN32
	#include <WinSock2.h>

	#include <windows.h>
	#include <time.h>
	#include <fcntl.h>
	#include <sys/timeb.h>


	#include "win32_export.h"

	struct timeVal {
		int    tv_sec;        /* seconds */
		int    tv_usec;  /* microseconds */
	};
	struct timeZone {
		int  tz_minuteswest; /* minutes W of Greenwich */
		int  tz_dsttime;     /* type of dst correction */
	};

	/* replacement for unix function, timezone stuff not yet implemented */
	TNS_UTIL_API int getTimeOfDay(struct timeVal *tv, struct timeZone *tz);
	TNS_UTIL_API char *winErrorStr(DWORD dwErr, char *buffer);
	
	/* we use CreateFile/ReadFile/WriteFile on Windows */
	#define fileHandle		HANDLE
	#define	O_NONBLOCK		0x0

	TNS_UTIL_API fileHandle openFd(char *name, int mode); 
	#define openFileMode(a,b,c)		openFd(a,b)
	TNS_UTIL_API int readFd(fileHandle fd, char *buf, int len); 
	TNS_UTIL_API int writeFd(fileHandle fd, char *buf, int len); 	
	TNS_UTIL_API int lseekFd(fileHandle fd, int where, int offset); 

	#define closeFd(x)	CloseHandle(x)

	/* mimic the unix sleep functions */
	#define	usleep(x)	Sleep(x/1000)
	#define sleep(x)	Sleep(x*1000)
	
	#define	false		FALSE
	#define true		TRUE
	#define StringPtr	LPCTSTR

	#define closeSock(x)		closesocket(x)
	#define readSock(a,b,c)		recv(a,b,c,0)
	#define writeSock(a,b,c)	send(a,b,c,0)

	#define ENOTCONN		WSAENOTCONN
	#define EWOULDBLOCK		WSAEWOULDBLOCK
	#define ECONNREFUSED	WSAEHOSTUNREACH
	#define ECONNRESET		WSAECONNRESET
	#define EAFNOSUPPORT	WSAEOPNOTSUPP

	#define SHUT_WR			SD_SEND
	#define SHUT_RDWR		SD_BOTH
	// {SD_SEND,SD_RECEIVE,SD_BOTH}
	
#else
	#include "tns_util/win32_export.h"

	#define DWORD		int
	#define StringPtr	char*
	#define BOOL		bool	

	/* we use standard Unix open/read/write with integer file descriptors */
	#define fileHandle				int
	
	#define openFd(a,b)				open(a,b)
	#define openFileMode(a,b,c)		open(a,b,c)
	#define readFd(a,b,c)			read(a,b,c)
	#define writeFd(a,b,c)			write(a,b,c)
	#define closeFd(a)				close(a)

	#define	INVALID_HANDLE_VALUE	-1

	/* we have natively defined timeval,timezone and gettimeofday */
	#define timeVal			timeval
	#define timeZone		timezone
	#define getTimeOfDay	gettimeofday

	/* some includes not found on with VC++ */
	#include <termios.h>
	#include <sys/time.h>
	#include <unistd.h>
	#include <fcntl.h>


	#define closeSock(x)		close(x)
	#define readSock(a,b,c)		read(a,b,c)
	#define writeSock(a,b,c)	write(a,b,c)
#endif


#ifdef __cplusplus
	}
#endif

#endif /* _PORTING_H_ */
