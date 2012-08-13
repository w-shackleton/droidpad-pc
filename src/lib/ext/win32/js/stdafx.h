// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include "targetver.h"


#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <tchar.h>
#include <basetyps.h>
#include <cfgmgr32.h>
#include <setupapi.h>
// #include <strsafe.h>
#include <newdev.h>
#include <ks.h>
#include <initguid.h>
#include <cguid.h>

#include <specstrings.h>

// Many non existent defs.
#include "winheader/salfix.h"

// Extra defs not present in MinGW

#define SUOI_FORCEDELETE 0x00000001

#ifdef UNICODE
#define SetupUninstallOEMInf SetupUninstallOEMInfW
#define SetupCreateDiskSpaceList SetupCreateDiskSpaceListW
#else
#define SetupUninstallOEMInf SetupUninstallOEMInfA
#define SetupCreateDiskSpaceList SetupCreateDiskSpaceListA
#endif

WINSETUPAPI WINBOOL WINAPI SetupUninstallOEMInfA(PCSTR InfFileName,DWORD Flags,PVOID Reserved);
WINSETUPAPI WINBOOL WINAPI SetupUninstallOEMInfW(PCWSTR InfFileName,DWORD Flags,PVOID Reserved);
WINSETUPAPI WINBOOL WINAPI SetupUninstallNewlyCopiedInfs(HSPFILEQ FileQueue,DWORD Flags,PVOID Reserved);
WINSETUPAPI HDSKSPC WINAPI SetupCreateDiskSpaceListA(PVOID Reserved1,DWORD Reserved2,UINT Flags);
WINSETUPAPI HDSKSPC WINAPI SetupCreateDiskSpaceListW(PVOID Reserved1,DWORD Reserved2,UINT Flags);

typedef int errno_t;
#define DI_FLAGSEX_INSTALLEDDRIVER          0x04000000
