/*++

Copyright (c) Shaul Eizikovich.  All rights reserved.

    THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
    KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
    PURPOSE.

Module Name:

    public.h
    
Abstract:

    Public header file for the vJoy project
	Developpers that need to interface with vJoy need to include this file

Author:


Environment:

    kernel mode and User mode

Notes:


Revision History:


--*/

// Compilation directives
#define PPJOY_MODE
#undef PPJOY_MODE	// Comment-out for compatibility mode

#ifdef PPJOY_MODE
#include "PPJIoctl.h"
#endif

// Sideband comunication with vJoy Device
//
// Usage example:
//		CreateFile(TEXT("\\\\.\\DPvJoy"), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
#ifdef PPJOY_MODE
#define DEVICENAME_STRING			"PPJoyIOCTL1"
#else
#define DEVICENAME_STRING			"DPvJoy"
#endif
#define NTDEVICE_NAME_STRING		"\\Device\\"DEVICENAME_STRING
#define SYMBOLIC_NAME_STRING		"\\DosDevices\\"DEVICENAME_STRING
#define	DOS_FILE_NAME				"\\\\.\\"DEVICENAME_STRING

// Device Attributes
//
#define VENDOR_N_ID		0x8191
#define	PRODUCT_N_ID	0xBEAD
#define	VERSION_N		0x0002

// Function codes;
#define LOAD_POSITIONS	0x910
#define GETATTRIB		0x911


// IO Device Control codes;
#define IOCTL_VJOY_LOAD_POSITIONS	CTL_CODE (FILE_DEVICE_UNKNOWN, LOAD_POSITIONS, METHOD_BUFFERED, FILE_WRITE_ACCESS)
#define IOCTL_VJOY_GET_ATTRIB	CTL_CODE (FILE_DEVICE_UNKNOWN, GETATTRIB, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#ifndef __HIDPORT_H__
// Copied from hidport.h
typedef struct _HID_DEVICE_ATTRIBUTES {

    ULONG           Size;
    //
    // sizeof (struct _HID_DEVICE_ATTRIBUTES)
    //

    //
    // Vendor ids of this hid device
    //
    USHORT          VendorID;
    USHORT          ProductID;
    USHORT          VersionNumber;
    USHORT          Reserved[11];

} HID_DEVICE_ATTRIBUTES, * PHID_DEVICE_ATTRIBUTES;
#endif

// Error levels for status report
enum ERRLEVEL {INFO, WARN, ERR, FATAL, APP};
// Status report function prototype
#ifdef WINAPI
typedef BOOL (WINAPI *StatusMessageFunc)(void * output, TCHAR * buffer, enum ERRLEVEL level);
#endif

///////////////////////////////////////////////////////////////

/////////////////////// Joystick Position ///////////////////////
//
// This structure holds data that is passed to the device from
// an external application such as SmartPropoPlus.
//
// Usage example:
//	JOYSTICK_POSITION iReport;
//	:
//	DeviceIoControl (hDevice, 100, &iReport, sizeof(HID_INPUT_REPORT), NULL, 0, &bytes, NULL)
typedef struct _JOYSTICK_POSITION
{
	LONG	wThrottle;
	LONG	wRudder;
	LONG	wAileron;
	LONG	wAxisX; // USED - 1
	LONG	wAxisY; // USED - 2
	LONG	wAxisZ;
	LONG	wAxisXRot; // USED - 3
	LONG	wAxisYRot; // USED - 4
	LONG	wAxisZRot;
	LONG	wSlider; // USED - 5
	LONG	wDial; // USED - 6
	LONG	wWheel;
	LONG	wAxisVX;
	LONG	wAxisVY;
	LONG	wAxisVZ;
	LONG	wAxisVBRX;
	LONG	wAxisVBRY;
	LONG	wAxisVBRZ;
	LONG	lButtons;	// 12 buttons
	UCHAR	bHats;
} JOYSTICK_POSITION, *PJOYSTICK_POSITION;


