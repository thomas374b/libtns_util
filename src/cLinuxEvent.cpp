/*
 * cLinuxEvent.cpp
 *
 *  Created on: 24.11.2015
 *      Author: pantec
 */


/*
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
*/



#include "tns_util/porting.h"
#include "tns_util/cLinuxEvent.h"
#include "tns_util/utils.h"
#include "tns_util/daemonic.h"

#include <string.h>

#ifndef MODNAME
#define MODNAME __FILE__
#endif
#include "tns_util/copyright.h"

cLinuxEvent::cLinuxEvent(const char *dev)
{
	fd = openFd(dev, O_RDONLY);
	fcntl(fd, F_SETFL, O_NONBLOCK|O_NDELAY);
/*
	for (int i=0; i<N_SCAN_CODES; i++) {
		scancodes[i] = -;
	}*/
	used_scancodes = 0;
}

cLinuxEvent::~cLinuxEvent()
{
	close(fd);
}

int cLinuxEvent::getFD(void)
{
	return fd;
}

/**
 * @param buffer
 * @param len
 * available buffer space should be (16 times len) + 32
 */
char *cLinuxEvent::get(char *buffer, short len)
{
	if (!FD_Ready(fd)) {
//		EPRINTF("not yet ready");
		return NULL;
	}
	input_event scan[16];

	int got = read(fd, (char*)&scan, len * sizeof(input_event));
	if (got == 0) {
		EPRINTF("got zero");
		return NULL;
	}
	if (got < 0) {
		EPRINTF("error: %s", strerror(errno));
		return NULL;
	}

	int n = got / sizeof(input_event);
	sprintf(buffer, "got %d, %d bytes, %d items: ", got , sizeof(input_event), n);

	char C[16];
	for (int i=0; i<n; i++) {
		sprintf(C, " t:%d c:%d v:%d", scan[n].type, scan[n].code, scan[n].value);
		strcat(buffer, C);
	}
//	strcat(buffer, "\n");
	return buffer;
}

__u16 *cLinuxEvent::getFiltered(void)
{
	if (!FD_Ready(fd)) {
		return NULL;
	}
	while(FD_Ready(fd)) {
		input_event scan[2];
		int got = read(fd, (char*)&scan, sizeof(input_event));
		if (got == 0) {
			EPRINTF("got zero");
			return NULL;
		}
		if (got < 0) {
			EPRINTF("error: %s", strerror(errno));
			return NULL;
		}
		if (got != sizeof(input_event)) {
			// TODO: sync reading with blocking IO
			EPRINTF("got garbage: %d != %d",  got, sizeof(input_event));
			return NULL;
		}
		for (int i=0; i<used_scancodes; i++) {
			if (scancodes[i].type == scan[0].type) {
				if (scancodes[i].code == scan[0].code) {
					if (scancodes[i].value == scan[0].value) {
						return &scancodes[i].code;
					}
				}
			}
		}
	}
	return NULL;
}


void cLinuxEvent::addCursorButtons()
{
	t_scancode cBtns[] = {
			{EV_KEY, KEY_DOWN,  SYN_REPORT},
			{EV_KEY, KEY_UP,    SYN_REPORT},
			{EV_KEY, KEY_LEFT,  SYN_REPORT},
			{EV_KEY, KEY_RIGHT, SYN_REPORT}
	};

	for (unsigned int i=0; i<sizeof(cBtns)/sizeof(t_scancode); i++) {
		addScanCode(&cBtns[i]);
	}
}

bool cLinuxEvent::addScanCode(t_scancode *sc)
{
	if (used_scancodes >= N_SCAN_CODES) {
		return false;
	}
	scancodes[used_scancodes].code = sc->code;
	scancodes[used_scancodes].type = sc->type;
	scancodes[used_scancodes].value = sc->value;
	used_scancodes++;
	return true;
}

