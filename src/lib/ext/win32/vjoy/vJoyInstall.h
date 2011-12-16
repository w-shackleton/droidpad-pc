#include "public.h"
#include <conio.h>
#include <stdio.h>
#include <time.h>

#ifdef _UNICODE
#define UPDATEDRIVERFORPLUGANDPLAYDEVICES "UpdateDriverForPlugAndPlayDevicesW"
#define SETUPUNINSTALLOEMINF "SetupUninstallOEMInfW"
#else
#define UPDATEDRIVERFORPLUGANDPLAYDEVICES "UpdateDriverForPlugAndPlayDevicesA"
#define SETUPUNINSTALLOEMINF "SetupUninstallOEMInfA"
#endif
#define SETUPSETNONINTERACTIVEMODE "SetupSetNonInteractiveMode"
#define SETUPVERIFYINFFILE "SetupVerifyInfFile"

#define SETUPSETNONINTERACTIVEMODE "SetupSetNonInteractiveMode"
//
// exit codes
//
#define EXIT_OK      (0)
#define EXIT_REBOOT  (1)
#define EXIT_FAIL    (2)
#define EXIT_USAGE   (3)

#define COMPATIBLE_ID	TEXT("hid_device_system_game")
#define	INFFILE			TEXT("vJoy.inf")
#define	HWID_TMPLT		TEXT("root\\VID_%04X&PID_%04X&REV_%04X")
#define	HWID_PPJOY0		TEXT("PPJoyBus\\VID_DEAD&PID_BEF0")
#define	INSTALL_LOG		TEXT("vJoyInstall.log")

// Sort of a hack - keeping original def, while getting around sal
#define __reserved

//
// UpdateDriverForPlugAndPlayDevices
//
typedef BOOL (WINAPI *UpdateDriverForPlugAndPlayDevicesProto)(__in HWND hwndParent,
                                                              __in LPCTSTR HardwareId,
                                                              __in LPCTSTR FullInfPath,
                                                              __in DWORD InstallFlags,
                                                              __out_opt PBOOL bRebootRequired
                                                         );
typedef BOOL (WINAPI *SetupSetNonInteractiveModeProto)(__in BOOL NonInteractiveFlag
                                                      );

typedef BOOL (WINAPI *SetupUninstallOEMInfProto)(__in LPCTSTR InfFileName,
                                                 __in DWORD Flags,
                                                 __reserved PVOID Reserved
                                                 );



enum VERBTYPE {INSTALL, REMOVE, REPAIR, CLEAN, INVALID};


// Function Prototypes
BOOL AssignCompatibleId(TCHAR * ParentDeviceNode, TCHAR * CompatibleId);
int cmdUpdateNI( __in DWORD Flags, LPCTSTR inf, LPCTSTR hwid );
int cmdUpdate( __in DWORD Flags, LPCTSTR inf, LPCTSTR hwid );
int InstallDriverOnDevice( TCHAR *InstanceId, LPCTSTR inf);
BOOL Install(LPCTSTR inf, LPCTSTR hwid, TCHAR *InstanceId);
BOOL FindInstalled(LPCTSTR hwid, TCHAR *InstanceId);
BOOL RestartDevice(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo);
#if (KMDF_MINOR_VERSION == 005 || KMDF_MINOR_VERSION == 007)
LPTSTR * GetRegMultiSz(__in HKEY hKey, __in LPCTSTR Val);
LPTSTR * GetMultiSzIndexArray(__in  LPTSTR MultiSz);
LPTSTR * GetDevMultiSz(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo, __in DWORD Prop);
void DelMultiSz(__in_opt PZPWSTR Array);
#else
__drv_allocatesMem(object) LPTSTR * GetRegMultiSz(__in HKEY hKey, __in LPCTSTR Val);
__drv_allocatesMem(object) LPTSTR * GetMultiSzIndexArray(__in __drv_aliasesMem LPTSTR MultiSz);
__drv_allocatesMem(object) LPTSTR * GetDevMultiSz(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo, __in DWORD Prop);
void DelMultiSz(__in_opt __drv_freesMem(object) PZPWSTR Array);
#endif
BOOL isPPJoyInstalled();
BOOL isvJoyInstalled();

// Major functions 
int Installation(LPCTSTR DeviceHWID, TCHAR * InfFile);
int Removal(TCHAR * DeviceHWID, TCHAR * InfFile, BOOL DelInf);
int Repair(TCHAR * DeviceHWID, TCHAR * InfFile);

// Helper Function Prototypes
BOOL GetErrorString(TCHAR * Msg, int Size);
BOOL  GetInputInfFullPath(TCHAR * Str);
int RemoveDevice(TCHAR *ParentDeviceNode, BOOL DelInf);
VERBTYPE GetVerb(int argc, PZPWSTR argv);
BOOL GetInfFile(int argc, PZPWSTR argv, TCHAR * InfFile);
BOOL GetDevHwId(int argc, PZPWSTR argv, TCHAR * DeviceHWID);
void PrintHeader(FILE * dst);
BOOL StatusMessageToStream(void * reserved, TCHAR * buffer, ERRLEVEL level);
BOOL GetOEMInfFileName( HDEVINFO DeviceInfoSet, SP_DEVINFO_DATA DeviceInfoData, TCHAR * OEMInfFileName);

