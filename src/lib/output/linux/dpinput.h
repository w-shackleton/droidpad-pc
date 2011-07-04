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
#ifdef __cplusplus
extern "C" {
#endif

#ifndef DP_INPUT_H
#define DP_INPUT_H

#define AXIS_SIZE 16384
#define AXIS_CUTOFF_MULTIPLIER (16384 * 3)

#include <fcntl.h>
#include <linux/input.h>

typedef struct dpinfo
{
	int ufile;
	int axisMin;
	int axisMax;
	int buttonNum;
	int axisNum;
	int type;
} dpInfo;

enum {
TYPE_JS,
TYPE_MOUSE,
TYPE_KEYBD
};

extern __u16 joystickKeys[];

#define ARRAY_COUNT(_array, _vartype)	(sizeof(_array) / sizeof(_vartype))

int dpinput_setup(dpInfo *info, int type);
int dpinput_close(dpInfo *info);

int dpinput_send2Pos(dpInfo *info, int posX, int posY);
int dpinput_sendNPos(dpInfo *info, int pos[], int count);
int dpinput_sendButtons(dpInfo *info, int buttons[], int count);
int dpinput_sendButton(dpInfo *info, int code, int val);

#endif

#ifdef __cplusplus
}
#endif
