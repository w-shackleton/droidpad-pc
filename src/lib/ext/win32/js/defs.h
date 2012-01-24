/*
 * This file is part of DroidPad.
 * DroidPad lets you use an Android mobile to control a joystick or mouse
 * on a Windows or Linux computer.
 * This program is the driver for DroidPad's Joystick.
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

// Definitions etc. for DroidPad driver

// Device Attributes
//
#define VENDOR_N_ID		0x0547
#define	PRODUCT_N_ID	0x1002
#define	VERSION_N		0x0002

#define SEND_INPUT_DATA		0x789
#define IOCTL_DP_SEND_INPUT_DATA	CTL_CODE (FILE_DEVICE_UNKNOWN, SEND_INPUT_DATA, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define DEVICENAME_STRING	"droidpad"

#define NTDEVICE_NAME_STRING		"\\Device\\"DEVICENAME_STRING
#define SYMBOLIC_NAME_STRING		"\\DosDevices\\"DEVICENAME_STRING
#define	DOS_FILE_NAME				"\\\\.\\"DEVICENAME_STRING


// Input data, as fed to the driver from userland. This is different to the HID descriptor as one may change but not the other
#include <pshpack1.h>
typedef struct _INPUT_DATA {
    LONG	axisX;
    LONG	axisY;
    LONG	axisZ;
    LONG	axisRX;
    LONG	axisRY;
    LONG	axisRZ;
    LONG	buttons;	// 16 Buttons (12 used). This is a long type so that less packing issues are run in to.
} INPUT_DATA, *PINPUT_DATA;
#include <poppack.h>

// Error levels for status report
enum ERRLEVEL {INFO, WARN, ERR, FATAL, APP};
// Status report function prototype
// #ifdef WINAPI
// typedef BOOL (WINAPI *StatusMessageFunc)(void * output, TCHAR * buffer, enum ERRLEVEL level);
// #endif
