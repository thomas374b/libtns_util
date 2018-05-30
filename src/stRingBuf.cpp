/*
 * stRingBuf.cpp
 *
 *  Created on: 25.05.2018
 *      Author: pantec
 */

#include "stRingBuf.h"

#include <stdio.h>
#include <string.h>

void stRingBuf::clear()
{
	memset(queue, 0 , lineLen*lineCnt);
}


stRingBuf::stRingBuf(short len, short cnt)
{
	lineLen = len;
	lineCnt = cnt;

	retBuf = new char[lineLen];
	tmpBuf = new char[lineLen];
	queue = new char[ lineLen*lineCnt];

	memset(retBuf, 0 , lineLen);
	memset(tmpBuf, 0 , lineLen);
	clear();
}



stRingBuf::~stRingBuf()
{
	delete retBuf;
	delete tmpBuf;
	delete queue;
}


void stRingBuf::push(char *buffer)
{
#ifdef WITH_DEBUG
	fprintf(stderr,"push %d[%s] => %d{%s}\n", strlen(buffer), buffer, strlen(queue), queue);
#endif
	strcat(queue, buffer);
}



char *stRingBuf::popLine()
{
	char *nl = strchr(queue, '\n');
	if (nl == NULL) {
		return NULL;
	}

	*nl = '\0';
	strcpy(retBuf, queue);
//	*nl = '\n';
	nl++;
	int remain = strlen(nl);
#ifdef WITH_DEBUG
	fprintf(stderr, "remains:%d [%s]\n", remain, nl);
#endif
	if (remain > 0) {
		strcpy(tmpBuf, nl);
		clear();
		strcpy(queue, tmpBuf);
	} else {
		clear();
	}

	return retBuf;
}

