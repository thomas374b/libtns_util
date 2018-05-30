/*
 * stRingBuf.h
 *
 *  Created on: 25.05.2018
 *      Author: pantec
 */

#ifndef STRINGBUF_H_
#define STRINGBUF_H_

class stRingBuf {
	short lineLen;
	short lineCnt;

	char *queue;
	char *tmpBuf;
	char *retBuf;
public:
	stRingBuf(short len, short cnt);
	virtual ~stRingBuf();

	void clear();
	void push(char *input);
	char *popLine();
};

#endif /* STRINGBUF_H_ */
