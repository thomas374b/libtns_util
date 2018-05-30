/*
 * cLinuxEvent.h
 *
 *  Created on: 24.11.2015
 *      Author: pantec
 */

#ifndef CLINUXEVENT_H_
#define CLINUXEVENT_H_

#include <linux/input.h>

typedef struct {
	__u16 type;
	__u16 code;
	__s32 value;
} t_scancode;

#ifndef N_SCAN_CODES
#define  N_SCAN_CODES	16
#endif

/**
 * intended to work with keyboards and joystick-buttons
 */
class cLinuxEvent {
private:
	int fd;
	t_scancode scancodes[N_SCAN_CODES];
	short used_scancodes;
public:
	cLinuxEvent(const char *dev);
	virtual ~cLinuxEvent();

	int getFD(void); // support select polling from other

	char *get(char *buffer, short len);
	__u16 *getFiltered(int to);

	bool addScanCode(t_scancode *);
	void addCursorButtons();
	void addJoystickButtons();
};

#endif /* CLINUXEVENT_H_ */
