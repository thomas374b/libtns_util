/*
 * t_LogBuffer.h
 *
 *  Created on: 16.01.2013
 *      Author: pantec
 */

#ifndef T_LOGBUFFER_H_
#define T_LOGBUFFER_H_


class t_LogBuffer {
	int stepping;
	int count;
	unsigned int bufSize;
	char *filename;
	char *buffer;
public:
	t_LogBuffer(int s, int bs);

	bool flush(void);
	void initEntry(char *fname, char *buf);
	bool addEntry(char *fname, char *buf);
	bool mkEntryNow(char *fname, char *buf);
	virtual ~t_LogBuffer();
};

#endif /* T_LOGBUFFER_H_ */
