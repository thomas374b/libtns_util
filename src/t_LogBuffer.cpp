/*
 * t_LogBuffer.cpp
 *
 *  Created on: 16.01.2013
 *      Author: pantec
 */


#include <stdio.h>
#include <string.h>

#include "tns_util/t_LogBuffer.h"
#include "tns_util/daemonic.h"
#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"


t_LogBuffer::t_LogBuffer(int s, int bs) {
	bufSize = bs;
	buffer = new char[bufSize+16];
	memset(buffer, 0, bufSize+8);
	stepping = s;
	count = 0;
	filename = NULL;
}

t_LogBuffer::~t_LogBuffer() {
	flush();

	delete buffer;
	buffer = NULL;
	bufSize = 0;

	delete filename;
	filename = NULL;
}

bool t_LogBuffer::flush() {
	if (filename == NULL) {
		return true;
	}
	bool ret = true;
	if (count > 0) {
		ret = (writeOrAppend(filename, buffer) == 0);
		if (ret) {
			memset(buffer, 0, bufSize+8);
			count = 0;
		}
	}
	return ret;
}

void t_LogBuffer::initEntry(char *fname, char *buf) {
	if (filename == NULL) {
		filename = new char[256];
	}
	strcpy(filename, fname);
	count = 1;
	strcpy(buffer,buf);
}

bool t_LogBuffer::addEntry(char *fname, char *buf) {
	if (filename == NULL) {
		// no file yet
		initEntry(fname, buf);
	} else {
		if (strcmp(filename, fname) == 0) {
			// same file

			unsigned int free = bufSize - strlen(buffer);
			if (free > strlen(buf)) {
				// free space
				strcat(buffer, buf);
				count++;

				if (count >= stepping) {
					return flush();
				}
			} else {
				EPRINTF("bufsize for burst write exhausted after %d entries", count);
				bool ret = flush();
				initEntry(fname, buf);
			}
		} else {
			// different file
			bool ret = flush(); // first
			initEntry(fname, buf);
			return ret;
		}
	}
	return true;
}

bool t_LogBuffer::mkEntryNow(char *fname, char *msg)
{
	char buffer[8192];
	time_t now = time(NULL);
	return addEntry(fname, mk_logString(now, false, msg, buffer));
}

