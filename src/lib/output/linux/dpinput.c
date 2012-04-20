/*
 * This file is part of DroidPad.
 * DroidPad lets you use an Android mobile to control a joystick or mouse
 * on a Windows or Linux computer.
 *
 * DroidPad is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DroidPad is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DroidPad, in the file COPYING.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "dpinput.h"

#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <errno.h>


/* Non-Globals */
struct input_event event;

int trimMinMax(int val, int min, int max);

char *uinput_filename[] = {"/dev/uinput", "/dev/input/uinput",
                           "/dev/misc/uinput"};

#define RESET_EVENT()	memset(&event, 0, sizeof(event));	\
      			gettimeofday(&event.time, NULL);

__u16 joystickKeys[] = {
BTN_A,		// Gamepad - from linux/input.h
BTN_B,
BTN_C,
BTN_X,
BTN_Y,
BTN_Z,
BTN_TL,
BTN_TR,
BTN_TL2,
BTN_TR2,
BTN_SELECT,
BTN_START,
BTN_MODE,
BTN_THUMBL,
BTN_THUMBR,

BTN_TRIGGER,	// Joystick - in case program has stupidly large number of buttons
BTN_THUMB,
BTN_THUMB2,
BTN_TOP,
BTN_TOP2,
BTN_PINKIE,
BTN_BASE,
BTN_BASE2,
BTN_BASE3,
BTN_BASE4,
BTN_BASE5,
BTN_BASE6,
BTN_DEAD,
};

__u16 joystickAxes[] = {
ABS_X,
ABS_Y,
ABS_Z,
ABS_RX,
ABS_RY,
ABS_RZ,
ABS_THROTTLE,
ABS_RUDDER,
ABS_WHEEL,
ABS_GAS,
ABS_BRAKE,
ABS_HAT0X,
ABS_HAT0Y,
ABS_HAT1X,
ABS_HAT1Y,
ABS_HAT2X,
ABS_HAT2Y,
ABS_HAT3X,
ABS_HAT3Y,
ABS_PRESSURE,
ABS_DISTANCE,
ABS_TILT_X,
ABS_TILT_Y,
ABS_TOOL_WIDTH,
ABS_VOLUME,
ABS_MISC,
};

int dpinput_checkUInput() {
	system("modprobe uinput"); // Some systems don't have this, try it anyway.
	
	int file, i;
	for (i = 0; i < ARRAY_COUNT(uinput_filename, char *); i++)
	{
		if ((file = open(uinput_filename[i], O_RDWR)) >= 0)
		{
			close(file);
			return CHECKRESULT_SUCCESS; // Worked.
		} else {
			switch(errno) {
				case EACCES:
					return CHECKRESULT_PERMS;
			}
		}
	}
	return CHECKRESULT_NOTFOUND;
}

int dpinput_setup(dpInfo *info, int type)
{
	struct uinput_user_dev uinp;
	if(info == NULL) return -2;
	info->type = type;
	
	int i=0, retcode;
	
	for (i = 0; i < ARRAY_COUNT(uinput_filename, char *); i++)
	{
		if ((info->ufile = open(uinput_filename[i], O_RDWR)) >= 0)
		{
			break;
		}
	}
	if (info->ufile < 0) {
		printf(" ** unable to open uinput, possibly not root / uinput not loaded?\n ** Trying to load uinput kernmod...\n");
	}
	system("modprobe uinput");
	
	for (i = 0; i < ARRAY_COUNT(uinput_filename, char *); i++)
	{
		if ((info->ufile = open(uinput_filename[i], O_RDWR)) >= 0)
		{
			break;
		}
	}
	if (info->ufile < 0) {
		printf(" ** unable to open uinput (second time), probably not root.\n");
		return -1;
	}
	
	memset(&uinp, 0, sizeof(uinp));
	strncpy(uinp.name, "DroidPad", 20);
	uinp.id.version = 4;
	uinp.id.bustype = BUS_USB;
	
	for(i = 0; i < ABS_MAX; i++)
	{
		uinp.absmax[i] = info->axisMax;
		uinp.absmin[i] = info->axisMin;
	}
	
	ioctl(info->ufile, UI_SET_EVBIT, EV_SYN);
	ioctl(info->ufile, UI_SET_EVBIT, EV_KEY);
	switch(type) {
		case TYPE_JS:
			ioctl(info->ufile, UI_SET_EVBIT, EV_ABS);
			for(i = 0; i < info->axisNum; i++)
				ioctl(info->ufile, UI_SET_ABSBIT, joystickAxes[i]);
			for(i = 0; i < info->buttonNum; i++)
				ioctl(info->ufile, UI_SET_KEYBIT, joystickKeys[i]);
			break;
		case TYPE_MOUSE:
			ioctl(info->ufile, UI_SET_EVBIT, EV_REL);
			ioctl(info->ufile, UI_SET_RELBIT, REL_X);
			ioctl(info->ufile, UI_SET_RELBIT, REL_Y);
			ioctl(info->ufile, UI_SET_RELBIT, REL_WHEEL);
			
			ioctl(info->ufile, UI_SET_KEYBIT, BTN_LEFT);
			ioctl(info->ufile, UI_SET_KEYBIT, BTN_MIDDLE);
			ioctl(info->ufile, UI_SET_KEYBIT, BTN_RIGHT);
			break;
		case TYPE_TOUCHSCREEN:
			ioctl(info->ufile, UI_SET_EVBIT, EV_ABS);
			ioctl(info->ufile, UI_SET_ABSBIT, ABS_X);
			ioctl(info->ufile, UI_SET_ABSBIT, ABS_Y);
			ioctl(info->ufile, UI_SET_ABSBIT, ABS_PRESSURE);
			
			ioctl(info->ufile, UI_SET_KEYBIT, BTN_DIGI); // Indicates this is absolute
			ioctl(info->ufile, UI_SET_KEYBIT, BTN_LEFT);
			ioctl(info->ufile, UI_SET_KEYBIT, BTN_MIDDLE);
			ioctl(info->ufile, UI_SET_KEYBIT, BTN_RIGHT);
			break;
		case TYPE_KEYBD:
			// All regular keys on a KB
			for(i = 0; i < 128; i++)
				ioctl(info->ufile, UI_SET_KEYBIT, i);
			break;
	}
	
	if(info->buttonNum > ARRAY_COUNT(joystickKeys, __u16))
		info->buttonNum = ARRAY_COUNT(joystickKeys, __u16);
	
	retcode = write(info->ufile, &uinp, sizeof(uinp));
	
	retcode = (ioctl(info->ufile, UI_DEV_CREATE));
	if (retcode) {
		printf(" ** Error creating uinput device %d.\n", retcode);
		
		ioctl (info->ufile, UI_DEV_DESTROY);
		close (info->ufile); 
		return -1;
	}
	printf(" ** uinput interface created\n");
	return 0;
}

int dpinput_close(dpInfo *info)
{
	if(info == NULL) return -2;
	RESET_EVENT();
	
	event.type = SYN_CONFIG;
	event.code = 0;
	event.value = 0;
	write(info->ufile,&event,sizeof(event));
	
	ioctl(info->ufile, UI_DEV_DESTROY);
	close(info->ufile);
	
	printf(" ** uinput closed.\n");
	return 0;
}

int dpinput_sendPos(dpInfo *info, int code, int val)
{
	if(info == NULL) return -2;
	RESET_EVENT();
	
	if(info->type == TYPE_JS || info->type == TYPE_TOUCHSCREEN)
		event.type = EV_ABS;
	else if(info->type == TYPE_MOUSE)
		event.type = EV_REL;
	event.code = code;
	event.value = trimMinMax(val, info->axisMin, info->axisMax);
	write(info->ufile, &event, sizeof(event));
	
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(info->ufile, &event, sizeof(event));
}

int dpinput_send2Pos(dpInfo *info, int posX, int posY)
{
	if(info == NULL) return -2;
	RESET_EVENT();
	
	if(info->type == TYPE_JS || info->type == TYPE_TOUCHSCREEN)
	{
		event.type = EV_ABS;
		event.code = ABS_X;
		event.value = trimMinMax(posX, info->axisMin, info->axisMax);
		write(info->ufile, &event, sizeof(event));
		event.code = ABS_Y;
		event.value = trimMinMax(posY, info->axisMin, info->axisMax);
		write(info->ufile, &event, sizeof(event));

		event.code = ABS_PRESSURE;
		event.value = info->axisMax;
		write(info->ufile, &event, sizeof(event));
	}
	else if(info->type == TYPE_MOUSE)
	{
		event.type = EV_REL;
		event.code = REL_X;
		event.value = posX;
		write(info->ufile, &event, sizeof(event));
		event.code = REL_Y;
		event.value = posY;
		write(info->ufile, &event, sizeof(event));
	}
	
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(info->ufile, &event, sizeof(event));
}

int dpinput_sendNPos(dpInfo *info, int pos[], int count)
{
	if(info == NULL) return -2;
	RESET_EVENT();
	
	if(info->type == TYPE_JS)
	{
		if(count > ARRAY_COUNT(joystickAxes, __u16))
			count = ARRAY_COUNT(joystickAxes, __u16);
		event.type = EV_ABS;
		int i;
		for(i = 0; i < count; i++)
		{
			event.code = joystickAxes[i]; // Send event for each button val
			event.value = trimMinMax(pos[i], info->axisMin, info->axisMax);
			write(info->ufile, &event, sizeof(event));
		}
	}
	
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(info->ufile, &event, sizeof(event));
}

int dpinput_sendButtons(dpInfo *info, int buttons[], int count)
{
	if(info == NULL) return -2;
	int i;
	RESET_EVENT();
	
	//printf("Count: %d, Array has %d.\n", count, ARRAY_COUNT(joystickKeys, __u16));
	
	if(count > ARRAY_COUNT(joystickKeys, __u16))
		count = ARRAY_COUNT(joystickKeys, __u16);
	
	event.type = EV_KEY;
	for(i = 0; i < count; i++)
	{
		event.code = joystickKeys[i];
		event.value = buttons[i];
		write(info->ufile, &event, sizeof(event));
	}
	
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(info->ufile, &event, sizeof(event));
}

int dpinput_sendButton(dpInfo *info, int code, int val)
{
	if(info == NULL) return -2;
	int i;
	RESET_EVENT();
	
	event.type = EV_KEY;
	event.code = code;
	event.value = val;
	write(info->ufile, &event, sizeof(event));
	
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(info->ufile, &event, sizeof(event));
}

int trimMinMax(int val, int min, int max)
{
	if(val < min)
		return min;
	if(val > max)
		return max;
	return val;
}
