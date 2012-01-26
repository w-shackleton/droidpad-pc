#include "stdafx.h"
#include "vJoyInstall.h"

extern FILE *stream;

// StatusMessageFunc StatusMessage = StatusMessageToStream;
#define StatusMessage StatusMessageToStream

TCHAR prt[MAX_PATH];

BOOL FindInstalled(LPCTSTR hwid, TCHAR *InstanceId)
{
	TCHAR ErrMsg[1000];
	BOOL out = FALSE;
	SP_DEVINFO_LIST_DETAIL_DATA devInfoListDetail;

	if (!hwid)
	{
		_stprintf(prt, _T("FindInstalled: HWID cannot be (null)"));
		StatusMessage( NULL, prt,  FATAL);
		return TRUE;
	};

	if (!InstanceId)
	{
		_stprintf(prt, _T("FindInstalled: Instance ID cannot be (null)"));
		StatusMessage( NULL, prt,  FATAL);
		return TRUE;
	};

	//_ftprintf(stream,_T(">> FindInstalled: Serching for HWID %s\n"), hwid);
	_stprintf(prt, _T("FindInstalled: Searching for HWID %s"), hwid);
	StatusMessage( NULL, prt,  INFO);


	HDEVINFO devs = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT );
	if (devs == INVALID_HANDLE_VALUE)
	{
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] FindInstalled: Function SetupDiGetClassDevs failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("FindInstalled: Function SetupDiGetClassDevs failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	};

	devInfoListDetail.cbSize = sizeof(devInfoListDetail);
	if(!SetupDiGetDeviceInfoListDetail(devs,&devInfoListDetail)) {
		GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] FindInstalled: Function SetupDiGetDeviceInfoListDetail failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("FindInstalled: Function SetupDiGetDeviceInfoListDetail failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	};

	SP_DEVINFO_DATA devInfo;
	devInfo.cbSize = sizeof(devInfo);
	for(int devIndex=0;SetupDiEnumDeviceInfo(devs,devIndex,&devInfo);devIndex++)
	{
		//TCHAR devID[MAX_DEVICE_ID_LEN];
		LPTSTR *hwIds = NULL;
		if(CM_Get_Device_ID_ExW(devInfo.DevInst,InstanceId,MAX_DEVICE_ID_LEN,0,devInfoListDetail.RemoteMachineHandle)!=CR_SUCCESS) {
			InstanceId[0] = TEXT('\0');
		};
		hwIds = GetDevMultiSz(devs,&devInfo,SPDRP_HARDWAREID);
		if (!hwIds)
			continue;

		//_ftprintf(stream,_T(">> FindInstalled: hwIds[%d] is %s\n"), devIndex, *hwIds);
		    
		// Test is this is the vJoy device (Comparing HWid)
		// If it is then restart it and return TRUE
		int cmp = _tcsnicmp(*hwIds,hwid, _tcslen(hwid));
		DelMultiSz((PZPWSTR)hwIds);
		if (!cmp)
		{
			//_ftprintf(stream,_T(">> FindInstalled: DevID[%d] is %s\n"), devIndex, InstanceId);
			 _stprintf(prt, _T("FindInstalled: DevID[%d] is %s"), devIndex, InstanceId);
			StatusMessage( NULL, prt,  INFO);
			// This is done for rare case where the child was removed manually
			RestartDevice(devs, &devInfo); 
			out = TRUE;
			break;
		}

	};

	return out;
}


BOOL Install(LPCTSTR inf, LPCTSTR hwid, TCHAR *InstanceId)
/*++

Routine Description:

    CREATE
    Creates a root enumerated devnode and installs drivers on it

Arguments:

    BaseName  - name of executable
    Machine   - machine name, must be NULL
    argc/argv - remaining parameters

Return Value:

    EXIT_xxxx

--*/
{
    HDEVINFO DeviceInfoSet = INVALID_HANDLE_VALUE;
    SP_DEVINFO_DATA DeviceInfoData;
    GUID ClassGUID;
    TCHAR ClassName[MAX_CLASS_NAME_LEN];
    TCHAR hwIdList[LINE_LEN+4];
    TCHAR InfPath[MAX_PATH];
    BOOL failcode = FALSE;
	TCHAR ErrMsg[1000];


    if(!inf[0]) 
	{
		//_ftprintf(stream,_T("[E] Install: Illegal Inf file\n"));
		 _stprintf(prt, _T("Install: Illegal Inf file"));
		StatusMessage( NULL, prt,  ERR);
		return FALSE;

		return FALSE;
	}

    if(!hwid[0]) 
	{
		//_ftprintf(stream,_T("[E] Install: Illegal  Hardware Id\n"));
		_stprintf(prt, _T("Install: Illegal  Hardware Id"));
		StatusMessage( NULL, prt,  ERR);

		return FALSE;
	}

    //
    // Inf must be a full pathname
    //
    if(GetFullPathName(inf,MAX_PATH,InfPath,NULL) >= MAX_PATH) 	
	{
		//_ftprintf(stream,_T("[E] Install: InfPath too long\n"));
		_stprintf(prt, _T("Install: InfPath too long"));
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}
	//_ftprintf(stream,_T(">> Install: GetFullPathName --> %s \n"), InfPath);
	_stprintf(prt, _T("Install: GetFullPathName --> %s "), InfPath);
	StatusMessage( NULL, prt,  INFO);

    //
    // List of hardware ID's must be double zero-terminated
    //
    ZeroMemory(hwIdList,sizeof(hwIdList));
    if (FAILED(lstrcpy(hwIdList,hwid))) 
	{
		//_ftprintf(stream,_T("[E] Install: Function StringCchCopy failed\n"));
		_stprintf(prt, _T("Install: Function StringCchCopy failed"));
		StatusMessage( NULL, prt,  ERR);
        goto final;
    }
	//_ftprintf(stream,_T(">> Install: hwIdList --> %s \n"), hwIdList);
	 _stprintf(prt, _T("Install: hwIdList --> %s"), hwIdList);
	StatusMessage( NULL, prt,  INFO);

    //
    // Use the INF File to extract the Class GUID.
    //
    if (!SetupDiGetINFClass(InfPath,&ClassGUID,ClassName,sizeof(ClassName)/sizeof(ClassName[0]),0))
	{
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] SetupDiGetINFClass failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("SetupDiGetINFClass failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		goto final;
	};
	//_ftprintf(stream,_T(">> Install: SetupDiGetINFClass --> Class Name %s \n"), ClassName);
	_stprintf(prt, _T("Install: SetupDiGetINFClass --> Class Name %s"), ClassName);
	StatusMessage( NULL, prt,  INFO);

    //
    // Create the container for the to-be-created Device Information Element.
    //
    DeviceInfoSet = SetupDiCreateDeviceInfoList(&ClassGUID,0);
    if(DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] SetupDiCreateDeviceInfoList failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("SetupDiCreateDeviceInfoList failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		goto final;
	};
	//_ftprintf(stream,_T(">> Install: SetupDiCreateDeviceInfoList OK\n"));
	_stprintf(prt, _T("Install: SetupDiCreateDeviceInfoList OK"));
	StatusMessage( NULL, prt,  INFO);

    //
    // Now create the element.
    // Use the Class GUID and Name from the INF file.
    //
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    if (!SetupDiCreateDeviceInfo(DeviceInfoSet, ClassName, &ClassGUID, NULL,  0, DICD_GENERATE_ID, &DeviceInfoData))
	{
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] SetupDiCreateDeviceInfo failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("SetupDiCreateDeviceInfo failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		goto final;
	};
	//_ftprintf(stream,_T(">> Install: SetupDiCreateDeviceInfo  OK\n"));
	_stprintf(prt, _T("Install: SetupDiCreateDeviceInfo  OK"));
	StatusMessage( NULL, prt,  INFO);

    //
    // Add the HardwareID to the Device's HardwareID property.
    //
    if(!SetupDiSetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID,(LPBYTE)hwIdList, (lstrlen(hwIdList)+1+1)*sizeof(TCHAR)))
    {
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] SetupDiSetDeviceRegistryProperty failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("SetupDiSetDeviceRegistryProperty failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
        goto final;
    }
	//_ftprintf(stream,_T(">> Install: SetupDiSetDeviceRegistryProperty  OK\n"));
	_stprintf(prt, _T("Install: SetupDiSetDeviceRegistryProperty  OK"));
	StatusMessage( NULL, prt,  INFO);

    //
    // Transform the registry element into an actual devnode
    // in the PnP HW tree.
    //
    if (!SetupDiCallClassInstaller(DIF_REGISTERDEVICE, DeviceInfoSet, &DeviceInfoData))
    {
		GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] SetupDiCallClassInstaller failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("SetupDiCallClassInstaller failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		goto final;
	}
	//_ftprintf(stream,_T(">> Install: SetupDiCallClassInstaller  OK\n"));
	_stprintf(prt, _T("Install: SetupDiCallClassInstaller  OK"));
	StatusMessage( NULL, prt,  INFO);

#if 0 // On XP - this causes a pop-up window even if already installed
	// Preinstalling package
	TCHAR DestinationInfFileName[MAX_PATH];
	//_ftprintf(stream,_T(">> Install: Starting SetupCopyOEMInf\n"));
	_stprintf(prt, _T("Install: Starting SetupCopyOEMInf"));
	StatusMessage( NULL, prt,  INFO);
	if (!SetupCopyOEMInf(InfPath, NULL, SPOST_PATH , NULL, DestinationInfFileName, MAX_PATH, NULL, NULL))
	{
		GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] SetupCopyOEMInf failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("SetupCopyOEMInf failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		goto final;
	}
	if (!DestinationInfFileName)
		DestinationInfFileName[0] = '\0';
	_ftprintf(stream,_T(">> Install: SetupCopyOEMInf (Destination file name is: [%s])  OK\n"), DestinationInfFileName);
	_stprintf(prt, _T("Install: SetupCopyOEMInf (Destination file name is: [%s])  OK"), DestinationInfFileName);
	StatusMessage( NULL, prt,  INFO);
#endif // On XP - ...

#define UPDATE
#ifdef UPDATE
	//
	// update the driver for the device we just created
	//
	//_ftprintf(stream,_T(">> Install: Starting cmdUpdate\n"));
	_stprintf(prt, _T("Install: Starting cmdUpdate"));
	StatusMessage( NULL, prt,  INFO);
	failcode = cmdUpdate( INSTALLFLAG_FORCE, inf,  hwid );
	//_ftprintf(stream,_T(">> Install: Finished cmdUpdate\n"));
	_stprintf(prt, _T("Install: Finished cmdUpdate"));
	StatusMessage( NULL, prt,  INFO);
#endif //UPDATE

	// Get Instence ID
	if (!SetupDiGetDeviceInstanceId(DeviceInfoSet, &DeviceInfoData, InstanceId, 1000, NULL))
	{
		GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] SetupDiGetDeviceInstanceId failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("SetupDiGetDeviceInstanceId failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		goto final;
	}
	//_ftprintf(stream,_T(">> Install: SetupDiGetDeviceInstanceId (Device Instance Path=%s) OK\n"), InstanceId);
	_stprintf(prt, _T("Install: SetupDiGetDeviceInstanceId (Device Instance Path=%s) OK"), InstanceId);
	StatusMessage( NULL, prt,  INFO);

#ifndef  UPDATE
	InstallDriverOnDevice(InstanceId, inf);
#endif // !UPDATE
	failcode = TRUE;

final:

	if (DeviceInfoSet != INVALID_HANDLE_VALUE) {
		SetupDiDestroyDeviceInfoList(DeviceInfoSet);
	}

    return failcode;
}


BOOL AssignCompatibleId(TCHAR * ParentDeviceNode, TCHAR * CompatibleId)
{

	TCHAR buf[MAX_DEVICE_ID_LEN]; // Place the Device Node here
	DEVINST  dnDevInst, childDevInst;
	CONFIGRET  rType;
	TCHAR ErrMsg[1000];
	SP_DEVINFO_DATA  DeviceInfoData;
	BOOL	rDi;


	if (!ParentDeviceNode)
	{
		//_ftprintf(stream,_T("[E] AssignCompatibleId: Empty ParentDeviceNode\n"));
		_stprintf(prt, _T("AssignCompatibleId: Empty ParentDeviceNode"));
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	};

	//_ftprintf(stream,_T(">> AssignCompatibleId: ParentDeviceNode = %s , CompatibleId = %s\n"), ParentDeviceNode, CompatibleId);
	_stprintf(prt, _T("AssignCompatibleId: ParentDeviceNode = %s , CompatibleId = %s"), ParentDeviceNode, CompatibleId);
	StatusMessage( NULL, prt,  INFO);

	_tcsncpy(buf, ParentDeviceNode, MAX_DEVICE_ID_LEN);
	// Obtains device instance handle to the parent's device node (dnDevInst)
	// that is associated with a specified device instance identifier (buf)
	rType = CM_Locate_DevNode(&dnDevInst, buf ,0);
	if (rType != CR_SUCCESS)
	{
		//_ftprintf(stream,_T("[E] AssignCompatibleId: Function CM_Locate_DevNode failed with error: %08X\n"), rType);
		_stprintf(prt, _T("AssignCompatibleId: Function CM_Locate_DevNode failed with error: %08X"), rType);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}
	else
	{
		//_ftprintf(stream,_T(">> AssignCompatibleId: Function CM_Locate_DevNode OK\n"));
		_stprintf(prt, _T("AssignCompatibleId: Function CM_Locate_DevNode OK"));
		StatusMessage( NULL, prt,  INFO);
	};



	// Retrieve a device instance handle to the first child node of the parent device node
	// Using the instance handle, retrieve the device instance ID for a child
	rType = CM_Get_Child(&dnDevInst, dnDevInst, 0);
	if (rType != CR_SUCCESS)
	{
		//_ftprintf(stream,_T("[E] AssignCompatibleId: Function CM_Get_Child failed with error: %08X\n"), rType);
		_stprintf(prt, _T("AssignCompatibleId: Function CM_Get_Child failed with error: %08X"), rType);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}
	else
	{
		//_ftprintf(stream,_T(">> AssignCompatibleId: Function CM_Get_Child OK\n"));
		_stprintf(prt, _T("AssignCompatibleId: Function CM_Get_Child OK"));
		StatusMessage( NULL, prt,  INFO);
	};

	ULONG  Len;
	rType = CM_Get_Device_ID_Size(&Len, dnDevInst, 0);
	if (rType != CR_SUCCESS)
	{
		//_ftprintf(stream,_T("[E] AssignCompatibleId: Function CM_Get_Device_ID_Size failed with error: %08X\n"), rType);
		_stprintf(prt, _T("AssignCompatibleId: Function CM_Get_Device_ID_Size failed with error: %08X"), rType);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}	
	else
	{
		//_ftprintf(stream,_T(">> AssignCompatibleId: Function CM_Get_Device_ID_Size OK\n"));
		_stprintf(prt, _T("AssignCompatibleId: Function CM_Get_Device_ID_Size OK"));
		StatusMessage( NULL, prt,  INFO);
	};


	Len+=2;
	TCHAR * DeviceInstanceId = new TCHAR[Len];
	rType = CM_Get_Device_ID(dnDevInst, DeviceInstanceId, Len, 0);
	if (rType != CR_SUCCESS)
	{
		//_ftprintf(stream,_T("[E] AssignCompatibleId: Function CM_Get_Device_ID failed with error: %08X\n"), rType);
		_stprintf(prt, _T("AssignCompatibleId: Function CM_Get_Device_ID failed with error: %08X"), rType);
		StatusMessage( NULL, prt,  ERR);
		delete DeviceInstanceId;
		return FALSE;
	}
	else
	{
		//_ftprintf(stream,_T(">> AssignCompatibleId: Function CM_Get_Device_ID (Device Instance Path = %s) OK\n"),DeviceInstanceId);
		_stprintf(prt, _T("AssignCompatibleId: Function CM_Get_Device_ID (Device Instance Path = %s) OK"),DeviceInstanceId);
		StatusMessage( NULL, prt,  INFO);
	};

	// Refresh child devnode
	rType = CM_Reenumerate_DevNode(dnDevInst, CM_REENUMERATE_RETRY_INSTALLATION );
	if (rType != CR_SUCCESS)
	{
		//_ftprintf(stream,_T("[E] AssignCompatibleId: Function CM_Reenumerate_DevNode[1] failed with error: %08X\n"), rType);
		_stprintf(prt, _T("AssignCompatibleId: Function CM_Reenumerate_DevNode[1] failed with error: %08X"), rType);
		StatusMessage( NULL, prt,  ERR);
	}
	else
	{
		//_ftprintf(stream,_T(">> AssignCompatibleId: Function CM_Reenumerate_DevNode[1] OK\n"));
		_stprintf(prt, _T("AssignCompatibleId: Function CM_Reenumerate_DevNode[1] OK"));
		StatusMessage( NULL, prt,  INFO);
	};

	// Test the existance of the child devnode
	rType = CM_Locate_DevNode(&childDevInst, DeviceInstanceId, CM_LOCATE_DEVNODE_NORMAL);
	if (rType == CR_NO_SUCH_DEVNODE)
	{
		//_ftprintf(stream,_T(">> AssignCompatibleId: DeviceInstanceId %s is missing\n"), DeviceInstanceId);
		_stprintf(prt, _T("AssignCompatibleId: DeviceInstanceId %s is missing"), DeviceInstanceId);
		StatusMessage( NULL, prt,  INFO);
		rType = CM_Reenumerate_DevNode(dnDevInst, CM_REENUMERATE_RETRY_INSTALLATION );
		if (rType != CR_SUCCESS)
		{
			//_ftprintf(stream,_T("[E] AssignCompatibleId: Function CM_Reenumerate_DevNode failed with error: %08X\n"), rType);
			_stprintf(prt, _T("AssignCompatibleId: Function CM_Reenumerate_DevNode failed with error: %08X"), rType);
			StatusMessage( NULL, prt,  ERR);
			delete DeviceInstanceId;
			return FALSE;
		}
		else
		{
			//_ftprintf(stream,_T(">> AssignCompatibleId: Function CM_Reenumerate_DevNode  OK\n"));
			_stprintf(prt, _T("AssignCompatibleId: Function CM_Reenumerate_DevNode  OK"));
			StatusMessage( NULL, prt,  INFO);
		};

	} else if (rType != CR_SUCCESS)
	{
		//_ftprintf(stream,_T("[E] AssignCompatibleId: Function CM_Locate_DevNode failed with error: %08X\n"), rType);
		_stprintf(prt, _T("AssignCompatibleId: Function CM_Locate_DevNode failed with error: %08X"), rType);
		StatusMessage( NULL, prt,  ERR);
		delete DeviceInstanceId;
		return FALSE;
	}
	else
	{
		//_ftprintf(stream,_T(">> AssignCompatibleId: Function CM_Locate_DevNode (Device Instance Path = %s) OK\n"),DeviceInstanceId);
		_stprintf(prt, _T("AssignCompatibleId: Function CM_Locate_DevNode (Device Instance Path = %s) OK"),DeviceInstanceId);
		StatusMessage( NULL, prt,  INFO);
	};



	// Accessing the HW registry key
	HDEVINFO DeviceInfoSet = SetupDiCreateDeviceInfoList(NULL, NULL);
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] SetupDiCreateDeviceInfoList failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("SetupDiCreateDeviceInfoList failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		delete DeviceInstanceId;
		return FALSE;
	}
	else
	{
		//_ftprintf(stream,_T(">> AssignCompatibleId: Function SetupDiCreateDeviceInfoList OK\n"));
		_stprintf(prt, _T("AssignCompatibleId: Function SetupDiCreateDeviceInfoList OK"));
		StatusMessage( NULL, prt,  INFO);
	};


	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	rDi = SetupDiOpenDeviceInfo(DeviceInfoSet, DeviceInstanceId, NULL, 0, &DeviceInfoData);
	if (!rDi)
	{
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] SetupDiOpenDeviceInfo failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("SetupDiOpenDeviceInfo failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		delete DeviceInstanceId;
		return FALSE;
	}
	else
	{
		//_ftprintf(stream,_T(">> AssignCompatibleId: Function SetupDiOpenDeviceInfo OK\n"));
		_stprintf(prt, _T("AssignCompatibleId: Function SetupDiOpenDeviceInfo OK"));
		StatusMessage( NULL, prt,  INFO);
	};


	delete DeviceInstanceId;

	// Setup Compatible ID (No need?)
	TCHAR PropBufferClass[LINE_LEN];
	ZeroMemory(PropBufferClass,sizeof(PropBufferClass));
	lstrcpy(PropBufferClass,CompatibleId);

	rDi = SetupDiSetDeviceRegistryProperty(DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID, (LPBYTE)PropBufferClass, sizeof(PropBufferClass));
	if (!rDi)
	{
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] SetupDiSetDeviceRegistryProperty failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("SetupDiSetDeviceRegistryProperty failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}
	else
	{
		//_ftprintf(stream,_T(">> AssignCompatibleId: Function SetupDiSetDeviceRegistryProperty OK\n"));
		_stprintf(prt, _T("AssignCompatibleId: Function SetupDiSetDeviceRegistryProperty OK"));
		StatusMessage( NULL, prt,  INFO);
	};

	//_ftprintf(stream,_T(">> AssignCompatibleId was OK\n"));
	_stprintf(prt, _T("AssignCompatibleId was OK"));
	StatusMessage( NULL, prt,  INFO);
	return TRUE;

}


int cmdUpdateNI( __in DWORD Flags, LPCTSTR inf, LPCTSTR hwid )
/*++

Routine Description:
    UPDATE (non interactive version)
    update driver for existing device(s)

Arguments:

    BaseName  - name of executable
    Machine   - machine name, must be NULL
    argc/argv - remaining parameters

Return Value:

    EXIT_xxxx

--*/
{
    //
    // turn off interactive mode while doing the update
    //
    HMODULE setupapiMod = NULL;
    SetupSetNonInteractiveModeProto SetNIFn;
    int res;
    BOOL prev;
	TCHAR InfPath[MAX_PATH];

	//
	// Inf must be a full pathname
	//
	if(GetFullPathName(inf,MAX_PATH,InfPath,NULL) >= MAX_PATH) 	
	{
		//_ftprintf(stream,_T("[E] cmdUpdateNI: InfPath too long\n"));
		return FALSE;
	}
	//_ftprintf(stream,_T(">> cmdUpdateNI: GetFullPathName --> %s \n"), InfPath);
	_stprintf(prt, _T("cmdUpdateNI: GetFullPathName --> %s"), InfPath);
	StatusMessage( NULL, prt,  INFO);
    setupapiMod = LoadLibrary(_T("setupapi.dll"));
    if(!setupapiMod) 
	{
		//_ftprintf(stream,_T("[E] cmdUpdateNI: LoadLibrary failed to load setupapi.dll\n"));
		_stprintf(prt, _T("cmdUpdateNI: LoadLibrary failed to load setupapi.dll"));
		StatusMessage( NULL, prt,  ERR);
        return cmdUpdate(Flags,InfPath,  hwid);
    }
    SetNIFn = (SetupSetNonInteractiveModeProto)GetProcAddress(setupapiMod,SETUPSETNONINTERACTIVEMODE);
    if(!SetNIFn)
    {
        FreeLibrary(setupapiMod);
		//_ftprintf(stream,_T("[E] cmdUpdateNI: GetProcAddress failed to get SETUPSETNONINTERACTIVEMODE\n"));
		_stprintf(prt, _T("cmdUpdateNI: GetProcAddress failed to get SETUPSETNONINTERACTIVEMODE"));
		StatusMessage( NULL, prt,  ERR);
        return cmdUpdate(Flags,InfPath,  hwid);
    }

    prev = SetNIFn(TRUE);
	//_ftprintf(stream,_T(">> cmdUpdateNI: SetNIFn(TRUE) returned %d\n"), prev);
	_stprintf(prt, _T("cmdUpdateNI: SetNIFn(TRUE) returned %d"), prev);
	StatusMessage( NULL, prt,  INFO);
	//_ftprintf(stream,_T(">> cmdUpdateNI: Calling cmdUpdate with InfPath=%s\n"), InfPath);
	_stprintf(prt, _T("cmdUpdateNI: Calling cmdUpdate with InfPath=%s"), InfPath);
	StatusMessage( NULL, prt,  INFO);
    res = cmdUpdate(Flags,InfPath,  hwid);
	//_ftprintf(stream,_T(">> cmdUpdateNI: cmdUpdate returned %d\n"), res);
	_stprintf(prt, _T("cmdUpdateNI: cmdUpdate returned %d"), res);
	StatusMessage( NULL, prt,  INFO);
    SetNIFn(prev);
    FreeLibrary(setupapiMod);
	//_ftprintf(stream,_T(">> cmdUpdateNI returns value %d\n"), res);
	_stprintf(prt, _T("cmdUpdateNI returns value %d"), res);
	StatusMessage( NULL, prt,  INFO);
    return res;
}

int cmdUpdate( __in DWORD Flags, LPCTSTR inf, LPCTSTR hwid )
/*++

Routine Description:
    UPDATE
    update driver for existing device(s)

Arguments:

    BaseName  - name of executable
    Machine   - machine name, must be NULL
    argc/argv - remaining parameters

Return Value:

    EXIT_xxxx

--*/
{
    HMODULE newdevMod = NULL;
    int failcode = EXIT_FAIL;
    UpdateDriverForPlugAndPlayDevicesProto UpdateFn;
    BOOL reboot = FALSE;
    DWORD flags = 0;
    DWORD res;
    TCHAR InfPath[MAX_PATH];
	TCHAR ErrMsg[1000];
	CONFIGRET ret;

    UNREFERENCED_PARAMETER(Flags);

    //
    // Inf must be a full pathname
    //
    res = GetFullPathName(inf,MAX_PATH,InfPath,NULL);
    if((res >= MAX_PATH) || (res == 0)) {
		//
		// inf pathname too long
		//
		//_ftprintf(stream,_T("[E] cmdUpdate:  GetFullPathName for file %s returned: %d\n"), InfPath, res);
		_stprintf(prt, _T("cmdUpdate:  GetFullPathName for file %s returned: %d"), InfPath, res);
		StatusMessage( NULL, prt,  ERR);

        return EXIT_FAIL;
    }
	//_ftprintf(stream,_T(">> cmdUpdate: GetFullPathName --> %s \n"), InfPath);
	_stprintf(prt, _T("cmdUpdate: GetFullPathName --> %s "), InfPath);
	StatusMessage( NULL, prt,  INFO);

    if(GetFileAttributes(InfPath)==(DWORD)(-1)) {
        //
        // inf doesn't exist
        //
		//_ftprintf(stream,_T("[E] cmdUpdate:  GetFileAttributes failed\n"));
		_stprintf(prt, _T("cmdUpdate:  GetFileAttributes failed"));
		StatusMessage( NULL, prt,  ERR);
        return EXIT_FAIL;
    }
	//_ftprintf(stream,_T(">> cmdUpdate: Install: Starting cmdUpdate \n"));
	_stprintf(prt, _T("cmdUpdate: Install: Starting cmdUpdate"));
	StatusMessage( NULL, prt,  INFO);

    inf = InfPath;
    flags |= INSTALLFLAG_FORCE;

    //
    // make use of UpdateDriverForPlugAndPlayDevices
    //
    newdevMod = LoadLibrary(_T("newdev.dll"));
    if(!newdevMod) 
	{
		//_ftprintf(stream,_T("[E] cmdUpdate:  Failed to load file newdev.dll\n"));
		_stprintf(prt, _T("cmdUpdate:  Failed to load file newdev.dll"));
		StatusMessage( NULL, prt,  ERR);
		goto final;
	}
	else
	{
		//_ftprintf(stream,_T(">> cmdUpdate:  File newdev.dll loaded OK\n"));
		_stprintf(prt, _T("cmdUpdate:  File newdev.dll loaded OK"));
		StatusMessage( NULL, prt,  INFO);
	};

    UpdateFn = (UpdateDriverForPlugAndPlayDevicesProto)GetProcAddress(newdevMod,UPDATEDRIVERFORPLUGANDPLAYDEVICES);
    if(!UpdateFn)
    {
		//_ftprintf(stream,_T("[E] cmdUpdate:  Failed to get UPDATEDRIVERFORPLUGANDPLAYDEVICES\n"));
		_stprintf(prt, _T("cmdUpdate:  Failed to get UPDATEDRIVERFORPLUGANDPLAYDEVICES"));
		StatusMessage( NULL, prt,  ERR);
        goto final;
    }
	else
	{
		//_ftprintf(stream,_T(">> cmdUpdate:  UPDATEDRIVERFORPLUGANDPLAYDEVICES got OK\n"));
		_stprintf(prt, _T("cmdUpdate:  UPDATEDRIVERFORPLUGANDPLAYDEVICES got OK"));
		StatusMessage( NULL, prt,  INFO);
	};


	// Update Driver
	ret = CMP_WaitNoPendingInstallEvents(30000);
	if (ret == WAIT_OBJECT_0)
	{
		//_ftprintf(stream,_T(">> cmdUpdate:  CMP_WaitNoPendingInstallEvents returned WAIT_OBJECT_0\n"));
		_stprintf(prt, _T("cmdUpdate:  CMP_WaitNoPendingInstallEvents returned WAIT_OBJECT_0"));
		StatusMessage( NULL, prt,  INFO);
	}
	else if (ret == WAIT_TIMEOUT)
	{
		//_ftprintf(stream,_T(">> cmdUpdate:  CMP_WaitNoPendingInstallEvents returned WAIT_TIMEOUT\n"));
		_stprintf(prt, _T("cmdUpdate:  CMP_WaitNoPendingInstallEvents returned WAIT_TIMEOUT"));
		StatusMessage( NULL, prt,  INFO);
	}
	else
	{
		GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] cmdUpdate:  CMP_WaitNoPendingInstallEvents failed with error: %s\n"), ErrMsg);
 		_stprintf(prt, _T("cmdUpdate:  CMP_WaitNoPendingInstallEvents failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
       goto final;
    }

    if(!UpdateFn(NULL,hwid,InfPath,flags,NULL)) 
	{
		GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] cmdUpdate:  UPDATEDRIVERFORPLUGANDPLAYDEVICES(hwid=%s, InfPath=%s) failed with error: %s\n"), hwid, InfPath, ErrMsg);
 		_stprintf(prt, _T("cmdUpdate:  UPDATEDRIVERFORPLUGANDPLAYDEVICES(hwid=%s, InfPath=%s) failed with error: %s"), hwid, InfPath, ErrMsg);
		StatusMessage( NULL, prt,  ERR);
        goto final;
    }
	else
	{
		//_ftprintf(stream,_T(">> cmdUpdate:  UPDATEDRIVERFORPLUGANDPLAYDEVICES(hwid=%s, InfPath=%s) executed OK\n"), hwid, InfPath);
		_stprintf(prt, _T("cmdUpdate:  UPDATEDRIVERFORPLUGANDPLAYDEVICES(hwid=%s, InfPath=%s) executed OK"), hwid, InfPath);
		StatusMessage( NULL, prt,  INFO);
	};

    //FormatToStream(stdout,MSG_UPDATE_OK);

    failcode = reboot ? EXIT_REBOOT : EXIT_OK;

final:

    if(newdevMod) {
        FreeLibrary(newdevMod);
	}

	//_ftprintf(stream,_T(">> cmdUpdate returns code %d\n"), failcode);
	_stprintf(prt, _T("cmdUpdate returns code %d"), failcode);
	StatusMessage( NULL, prt,  INFO);
	return failcode;
}




int RemoveDevice(TCHAR *ParentDeviceNode, BOOL DelInf )
{
	DEVINST  dnDevInst;
	SP_DEVINFO_DATA  DeviceInfoData;
	TCHAR ErrMsg[1000];
	BOOL rDi;
	CONFIGRET  rType;

	//_ftprintf(stream,_T(">> RemoveDevice: ParentDeviceNode = %s\n"), ParentDeviceNode);
	_stprintf(prt, _T("RemoveDevice: ParentDeviceNode = %s"), ParentDeviceNode);
	StatusMessage( NULL, prt,  INFO);

	// Accessing the HW registry key
	HDEVINFO DeviceInfoSet = SetupDiCreateDeviceInfoList(NULL, NULL);
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] RemoveDevice: Function SetupDiCreateDeviceInfoList failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("RemoveDevice: Function SetupDiCreateDeviceInfoList failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return -101;
	}
	else
	{
		//_ftprintf(stream,_T(">> RemoveDevice: Function SetupDiCreateDeviceInfoList OK\n"));
		_stprintf(prt, _T("RemoveDevice: Function SetupDiCreateDeviceInfoList OK"));
		StatusMessage( NULL, prt,  INFO);
	};

	// Obtains device instance handle to the device node (dnDevInst)
	// that is associated with a specified device instance identifier (buf)
	rType = CM_Locate_DevNode(&dnDevInst, ParentDeviceNode ,0);
	if (rType != CR_SUCCESS)
	{
		//_ftprintf(stream,_T("[E] RemoveDevice: Function CM_Locate_DevNode failed with error: %08X\n"), rType);
		_stprintf(prt, _T("RemoveDevice: Function CM_Locate_DevNode failed with error: %08X"), rType);
		StatusMessage( NULL, prt,  ERR);
		return -102;
	}
	else
	{
		//_ftprintf(stream,_T(">> RemoveDevice: Function CM_Locate_DevNode OK\n"));
		_stprintf(prt, _T("RemoveDevice: Function CM_Locate_DevNode failed with error: %08X"), rType);
		StatusMessage( NULL, prt,  INFO);
	};

	// Get the ID of the device instance
	ULONG  Len;
	rType = CM_Get_Device_ID_Size(&Len, dnDevInst, 0);
	if (rType != CR_SUCCESS)
	{
		//_ftprintf(stream,_T("[E] RemoveDevice: Function CM_Get_Device_ID_Size failed with error: %08X\n"), rType);
		_stprintf(prt, _T("RemoveDevice: Function CM_Get_Device_ID_Size failed with error: %08X"), rType);
		StatusMessage( NULL, prt,  ERR);
		return -103;
	}	
	else
	{
		//_ftprintf(stream,_T(">> RemoveDevice: Function CM_Get_Device_ID_Size OK\n"));
		_stprintf(prt, _T("RemoveDevice: Function CM_Get_Device_ID_Size OK"));
		StatusMessage( NULL, prt,  INFO);
	};


	Len+=2;
	TCHAR * DeviceInstanceId = new TCHAR[Len];
	rType = CM_Get_Device_ID(dnDevInst, DeviceInstanceId, Len, 0);
	if (rType != CR_SUCCESS)
	{
		//_ftprintf(stream,_T("[E] RemoveDevice: Function CM_Get_Device_ID failed with error: %08X\n"), rType);
		_stprintf(prt, _T("RemoveDevice: Function CM_Get_Device_ID failed with error: %08X"), rType);
		StatusMessage( NULL, prt,  ERR);
		delete DeviceInstanceId;
		return -104;
	}
	else
	{
		//_ftprintf(stream,_T(">> RemoveDevice: Function CM_Get_Device_ID (Device Instance Path = %s) OK\n"),DeviceInstanceId);
		_stprintf(prt, _T("RemoveDevice: Function CM_Get_Device_ID (Device Instance Path = %s) OK"),DeviceInstanceId);
		StatusMessage( NULL, prt,  INFO);
	};


	// Get the Device Info Data
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	rDi = SetupDiOpenDeviceInfo(DeviceInfoSet, DeviceInstanceId, NULL, 0, &DeviceInfoData);
	if (!rDi)
	{
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] RemoveDevice: Function SetupDiOpenDeviceInfo failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("RemoveDevice: Function SetupDiOpenDeviceInfo failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return -105;
	}
	else
	{
		//_ftprintf(stream,_T(">> RemoveDevice: Function SetupDiOpenDeviceInfo OK\n"));
		_stprintf(prt, _T("RemoveDevice: Function SetupDiOpenDeviceInfo OK"));
		StatusMessage( NULL, prt,  INFO);
	};

	// If Requested to Delete INF file from inf directoy - Get the name of file OEMx.INF
	TCHAR OEMInfFileName[MAX_PATH];
	if (DelInf)
		GetOEMInfFileName(DeviceInfoSet, DeviceInfoData, OEMInfFileName); 

	// Remove
	rDi = SetupDiRemoveDevice(DeviceInfoSet, &DeviceInfoData);
	if (!rDi)
	{
		 GetErrorString(ErrMsg,1000);
		//_ftprintf(stream,_T("[E] RemoveDevice: Function SetupDiRemoveDevice failed with error: %s\n"), ErrMsg);
		_stprintf(prt, _T("RemoveDevice: Function SetupDiRemoveDevice failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return -106;
	}
	else
	{
		//_ftprintf(stream,_T(">> RemoveDevice: Function SetupDiRemoveDevice OK\n"));
		_stprintf(prt, _T("RemoveDevice: Function SetupDiRemoveDevice OK"));
		StatusMessage( NULL, prt,  INFO);
	};

	// If not requested to Delete INF file from inf directoy - just return
	if (!DelInf)
		return 0;

	////// If Requested to Delete INF file from inf directoy ///////
	_stprintf(prt, _T("RemoveDevice: Going to remove file %s"), OEMInfFileName);
	StatusMessage( NULL, prt,  INFO);

	// ADDED BY WILL

    //
    // make use of SetupUninstall...
    //
    HMODULE newdevMod = NULL;
    newdevMod = LoadLibrary(_T("setupapi.dll"));
    if(!newdevMod) 
	{
		//_ftprintf(stream,_T("[E] cmdUpdate:  Failed to load file newdev.dll\n"));
		_stprintf(prt, _T("cmdUpdate:  Failed to load file setupapi.dll"));
		StatusMessage( NULL, prt,  ERR);
		goto final;
	}
	else
	{
		//_ftprintf(stream,_T(">> cmdUpdate:  File newdev.dll loaded OK\n"));
		_stprintf(prt, _T("cmdUpdate:  File setupapi.dll loaded OK"));
		StatusMessage( NULL, prt,  INFO);
	};
	SetupUninstallOEMInfProto SetupUninstallOEMInf;
        SetupUninstallOEMInf = (SetupUninstallOEMInfProto)GetProcAddress(newdevMod,SETUPUNINSTALLOEMINF);

	rDi = SetupUninstallOEMInf(OEMInfFileName, SUOI_FORCEDELETE, NULL);
	if (!rDi)
	{
		GetErrorString(ErrMsg,1000);
		_stprintf(prt, _T("RemoveDevice: Function SetupUninstallOEMInf failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return -116;
	};
	_stprintf(prt, _T("RemoveDevice: File %s removed"), OEMInfFileName);
	StatusMessage( NULL, prt,  INFO);

final:
    if(newdevMod) {
        FreeLibrary(newdevMod);
	}

	return 0;
}

BOOL RestartDevice(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo)
/*++

Routine Description:

Based on Decxon Callback for use by Enable/Disable/Restart
Invokes DIF_PROPERTYCHANGE with correct parameters
uses SetupDiCallClassInstaller so cannot be done for remote devices
Don't use CM_xxx API's, they bypass class/co-installers and this is bad.

In Enable case, we try global first, and if still disabled, enable local

Arguments:

Devs    )_ uniquely identify the device
DevInfo )
Index    - index of device
Context  - GenericContext

Return Value:

EXIT_xxxx

--*/
{
	SP_PROPCHANGE_PARAMS pcp;
	//GenericContext *pControlContext;
	SP_DEVINSTALL_PARAMS devParams;


	//////////////// DISABLE ///////////////////////////////
	//
	// operate on config-specific profile
	//
	pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	pcp.StateChange = DICS_DISABLE;
	pcp.Scope = DICS_FLAG_CONFIGSPECIFIC;
	pcp.HwProfile = 0;


	if(!SetupDiSetClassInstallParams(Devs,DevInfo,&pcp.ClassInstallHeader,sizeof(pcp)) ||
		!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE,Devs,DevInfo))
		// failed to invoke DIF_PROPERTYCHANGE
		return FALSE;
	//else
	//	return TRUE;

	//////////////// ENABLE ///////////////////////////////
	//
	// enable both on global and config-specific profile
	// do global first and see if that succeeded in enabling the device
	// (global enable doesn't mark reboot required if device is still
	// disabled on current config whereas vice-versa isn't true)
	//
	pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	pcp.StateChange = DICS_ENABLE;
	pcp.Scope = DICS_FLAG_GLOBAL;
	pcp.HwProfile = 0;
	//
	// don't worry if this fails, we'll get an error when we try config-
	// specific.
	if(SetupDiSetClassInstallParams(Devs,DevInfo,&pcp.ClassInstallHeader,sizeof(pcp))) 
		SetupDiCallClassInstaller(DIF_PROPERTYCHANGE,Devs,DevInfo);
	//
	// now enable on config-specific
	//
	pcp.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
	pcp.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
	pcp.StateChange = DICS_ENABLE;
	pcp.Scope = DICS_FLAG_CONFIGSPECIFIC;
	pcp.HwProfile = 0;

	if(!SetupDiSetClassInstallParams(Devs,DevInfo,&pcp.ClassInstallHeader,sizeof(pcp)) ||
		!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE,Devs,DevInfo))
		// failed to invoke DIF_PROPERTYCHANGE
		return FALSE;
	else
		return TRUE;

}

BOOL isvJoyInstalled()
{
	TCHAR InstanceId[MAX_DEVICE_ID_LEN];
	TCHAR DeviceHWID[MAX_PATH];

	GetDevHwId(0, NULL, DeviceHWID);
	BOOL installed = FindInstalled(DeviceHWID, InstanceId);
	return installed;
}

/* Major Functions */
int Installation(LPCTSTR DeviceHWID, TCHAR * InfFile)
{
	BOOL	rDi;
	TCHAR InstanceId[MAX_DEVICE_ID_LEN];
	GUID ClassGUID = GUID_NULL;
	TCHAR InfPath[MAX_PATH];

	/////////////////////////////////////
	// Test if device already installed. 
	InstanceId[0] = '\0';
	if (FindInstalled(DeviceHWID,InstanceId))
	{
		//_ftprintf(stream,_T("[E] Device already installed - Install failed\n"));
		_stprintf(prt, _T("Device already installed - Install failed"));
		StatusMessage( NULL, prt,  WARN);
		return -2;
	}
	else
	{	// Not installed so install it
		if (!Install(InfFile, DeviceHWID,InstanceId))
		{
			//_ftprintf(stream,_T("[E] Install failed\n"));
			_stprintf(prt, _T("Install failed"));
			StatusMessage( NULL, prt,  ERR);
			return -1;
		};
	};
	//
	/////////////////////////////////////

	rDi = AssignCompatibleId(InstanceId, COMPATIBLE_ID);
	if (!rDi)
	{
		RemoveDevice(InstanceId, TRUE);
		return -5;
	};
	GetInputInfFullPath(InfPath);
	 _stprintf(prt, _T("installation(): InputInfFullPath --> %s"), InfPath);
	StatusMessage( NULL, prt,  INFO);
	cmdUpdateNI( INSTALLFLAG_FORCE ,InfPath, COMPATIBLE_ID );
	return 0;
}

int Removal(TCHAR * DeviceHWID, TCHAR * InfFile, BOOL DelInf)
{
	TCHAR InstanceId[MAX_DEVICE_ID_LEN];

	/////////////////////////////////////
	// Test if device already installed. 
	InstanceId[0] = '\0';
	if (!FindInstalled(DeviceHWID,InstanceId))
	{
		//_ftprintf(stream,_T(">> Removal(): Device %s not installed\n"),DeviceHWID);
		_stprintf(prt, _T("Removal(): Device %s not installed"),DeviceHWID);
		StatusMessage( NULL, prt,  WARN);
		return -100;
	};

	// Installed - Continue;
	//_ftprintf(stream,_T(">> Removal(): Device %s is installed (InstanceId %s) -- Removing\n"),DeviceHWID, InstanceId);
	_stprintf(prt, _T("Removal(): Device %s is installed (InstanceId %s) -- Removing"),DeviceHWID, InstanceId);
	StatusMessage( NULL, prt,  INFO);
	int result = RemoveDevice(InstanceId, DelInf);
	return result;
}
int Repair(TCHAR * DeviceHWID, TCHAR * InfFile)
{
	BOOL	rDi;
	TCHAR InstanceId[MAX_DEVICE_ID_LEN];
	GUID ClassGUID = GUID_NULL;
	TCHAR InfPath[MAX_PATH];

	/////////////////////////////////////
	// Test if device already installed. 
	InstanceId[0] = '\0';
	if (FindInstalled(DeviceHWID,InstanceId))
	{
		//_ftprintf(stream,_T(">> Device already installed - Repairing\n"));
		_stprintf(prt, _T("Device already installed - Repairing"));
		StatusMessage( NULL, prt,  WARN);
	}
	else
	{	// Not installed so install it
		//_ftprintf(stream,_T(">> Device not installed - Installing\n"));
		_stprintf(prt, _T("Device not installed - Installing"));
		StatusMessage( NULL, prt,  INFO);
		if (!Install(InfFile, DeviceHWID,InstanceId))
		{
			//_ftprintf(stream,_T("[E] Install failed\n"));
			_stprintf(prt, _T("Install failed"));
			StatusMessage( NULL, prt,  ERR);
			return -1;
		};
	};
	//
	/////////////////////////////////////

	rDi = AssignCompatibleId(InstanceId, COMPATIBLE_ID);
	if (!rDi)
	{
		RemoveDevice(InstanceId, FALSE);
		return -5;
	};
	GetInputInfFullPath(InfPath);
	//_ftprintf(stream,_T(">> Repair(): InputInfFullPath --> %s \n"), InfPath);
	_stprintf(prt, _T("Repair(): InputInfFullPath --> %s"), InfPath);
	StatusMessage( NULL, prt,  INFO);
	cmdUpdateNI( INSTALLFLAG_FORCE ,InfPath, COMPATIBLE_ID );
	return 0;
}

/* Helper Functions */
BOOL GetErrorString(TCHAR * Msg, int Size)
{
	LPTSTR s;
	memset(Msg,' ', Size);
	DWORD errorcode = GetLastError();
	int nTChars = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,  NULL, errorcode, 0, (LPTSTR)&s,  0, NULL);
	if (!nTChars)
	{
		_stprintf(Msg, _T("Unknown Error: %08x"),errorcode);
		return FALSE;
	};

	_tcsncpy(Msg, s, Size);
	LocalFree(s);
	return TRUE;
}
BOOL GetInputInfFullPath(TCHAR * Str)
{

	size_t ReturnValue;
	TCHAR  tbuffer[MAX_PATH];
	TCHAR * buffer;

	buffer = tbuffer;
	buffer = _tgetenv(_T("WINDIR"));

	_stprintf(Str, _T("%s\\Inf\\Input.inf"),buffer);
	return TRUE;
}

#if (KMDF_MINOR_VERSION != 005 &&  KMDF_MINOR_VERSION != 007)
__drv_allocatesMem(object)
LPTSTR * GetMultiSzIndexArray(__in __drv_aliasesMem LPTSTR MultiSz)
#else
LPTSTR * GetMultiSzIndexArray(__in  LPTSTR MultiSz)
#endif
/*++

Routine Description:

    Get an index array pointing to the MultiSz passed in

Arguments:

    MultiSz - well formed multi-sz string

Return Value:

    array of strings. last entry+1 of array contains NULL
    returns NULL on failure

--*/
{
    LPTSTR scan;
    LPTSTR * array;
    int elements;

    for(scan = MultiSz, elements = 0; scan[0] ;elements++) {
        scan += lstrlen(scan)+1;
    }
    array = new LPTSTR[elements+2];
    if(!array) {
        return NULL;
    }
    array[0] = MultiSz;
    array++;
    if(elements) {
        for(scan = MultiSz, elements = 0; scan[0]; elements++) {
            array[elements] = scan;
            scan += lstrlen(scan)+1;
        }
    }
    array[elements] = NULL;
    return array;
}

#if (KMDF_MINOR_VERSION != 005 &&  KMDF_MINOR_VERSION != 007)
__drv_allocatesMem(object)
#endif
LPTSTR * GetDevMultiSz(__in HDEVINFO Devs, __in PSP_DEVINFO_DATA DevInfo, __in DWORD Prop)
/*++

Routine Description:

    Get a multi-sz device property
    and return as an array of strings

Arguments:

    Devs    - HDEVINFO containing DevInfo
    DevInfo - Specific device
    Prop    - SPDRP_HARDWAREID or SPDRP_COMPATIBLEIDS

Return Value:

    array of strings. last entry+1 of array contains NULL
    returns NULL on failure

--*/
{
    LPTSTR buffer;
    DWORD size;
    DWORD reqSize;
    DWORD dataType;
    LPTSTR * array;
    DWORD szChars;

    size = 8192; // initial guess, nothing magic about this
    buffer = new TCHAR[(size/sizeof(TCHAR))+2];
    if(!buffer) {
        return NULL;
    }
    while(!SetupDiGetDeviceRegistryProperty(Devs,DevInfo,Prop,&dataType,(LPBYTE)buffer,size,&reqSize)) {
        if(GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            goto failed;
        }
        if(dataType != REG_MULTI_SZ) {
            goto failed;
        }
        size = reqSize;
        delete [] buffer;
        buffer = new TCHAR[(size/sizeof(TCHAR))+2];
        if(!buffer) {
            goto failed;
        }
    }
    szChars = reqSize/sizeof(TCHAR);
    buffer[szChars] = TEXT('\0');
    buffer[szChars+1] = TEXT('\0');
    array = GetMultiSzIndexArray(buffer);
    if(array) {
        return array;
    }

failed:
    if(buffer) {
        delete [] buffer;
    }
    return NULL;
}

#if (KMDF_MINOR_VERSION != 005 &&  KMDF_MINOR_VERSION != 007)
void DelMultiSz(__in_opt __drv_freesMem(object) PZPWSTR Array)
#else
void DelMultiSz(__in_opt  PZPWSTR Array)
#endif

/*++

Routine Description:

    Deletes the string array allocated by GetDevMultiSz/GetRegMultiSz/GetMultiSzIndexArray

Arguments:

    Array - pointer returned by GetMultiSzIndexArray

Return Value:

    None

--*/
{
    if(Array) {
        Array--;
        if(Array[0]) {
            delete [] Array[0];
        }
        delete [] Array;
    }
}


VERBTYPE GetVerb(int argc, PZPWSTR argv)
{
	// Default
	if (argc == 1)
		return INSTALL;

	if ((((TCHAR *)argv[1])[0] == 'i') || (((TCHAR *)argv[1])[0] == 'I'))
		return INSTALL;
	if ((((TCHAR *)argv[1])[0] == 'u') || (((TCHAR *)argv[1])[0] == 'U'))
		return REMOVE;
	if ((((TCHAR *)argv[1])[0] == 'c') || (((TCHAR *)argv[1])[0] == 'C'))
		return CLEAN;
	if ((((TCHAR *)argv[1])[0] == 'r') || (((TCHAR *)argv[1])[0] == 'R'))
		return REPAIR;

	return INVALID;
}
BOOL GetInfFile(int argc, PZPWSTR argv, TCHAR * InfFile)
{

	if (argc >=3)
		_stprintf(InfFile, (TCHAR *)argv[2]);
	else
		_stprintf(InfFile, INFFILE);

	return TRUE;
}


BOOL GetDevHwId(int argc, PZPWSTR argv, TCHAR * DeviceHWID)
{
	if (argc >=4)
		_stprintf(DeviceHWID, (TCHAR *)argv[3]);
	else
		_stprintf(DeviceHWID, HWID_TMPLT,VENDOR_N_ID, PRODUCT_N_ID, VERSION_N);

	return TRUE;
}


void PrintHeader(FILE * dst)
{
	#define PRODUCT_PROFESSIONAL 0x00000030
	typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
	time_t long_time;
	char timebuf[26], tmpbuf[128];
	errno_t err;
	struct tm *newtime;
	OSVERSIONINFOEX osvi;
	LPOSVERSIONINFOEX lposvi;
	SYSTEM_INFO sysinfo;
	DWORD ProdType;
	PGPI pGPI;



	_ftprintf(dst, _T("+++++++ +++++++ +++++++ +++++++ +++++++ +++++++ +++++++ +++++++ +++++++ +++++++ \n"));

	//// Time of log file ////////////////////////////////////////////
	// Get time as 64-bit integer.
	time( &long_time ); 
	// Convert to local time.
	newtime = localtime( &long_time );
	// Use strftime to build a customized time string. 
	_ftprintf(dst, _T("+++++++ +++++++ %s"),  asctime( newtime ) );

	//// System information /////////////////////////////////////////
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	lposvi = &osvi;
	GetVersionEx((LPOSVERSIONINFO)lposvi);
	_ftprintf(dst, _T("+++++++ +++++++ OS: %d.%d %s "), osvi.dwMajorVersion,   osvi.dwMinorVersion, osvi.szCSDVersion );

	GetNativeSystemInfo(&sysinfo);
	if (sysinfo.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
		_ftprintf(dst, _T(" (x64)") );
	else if (sysinfo.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL)
		_ftprintf(dst, _T(" (x86)") );
	else
	_ftprintf(dst, _T(" (ARCH?)") );

	// XP
	if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
	{
		_ftprintf(dst, _T("[ Windows XP "));
		if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
			_ftprintf(dst,  _T("Home Edition ]"));
		else _ftprintf(dst,  _T("Professional ]"));
	};

	// Vista/W7
	if ( osvi.dwMajorVersion == 6 )
	{
		pGPI = (PGPI) GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetProductInfo");
		pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &ProdType);

	};

	 _ftprintf(dst,_T("\n"));
}

BOOL StatusMessageToStream(void * reserved, TCHAR * buffer,  ERRLEVEL level)
{
	TCHAR *Prefix;

	switch (level)
	{
	case INFO:
		Prefix = _T("[I] ");
		break;
	case WARN:
		Prefix = _T("[W] ");
		break;
	case ERR:
		Prefix = _T("[E] ");
		break;
	case FATAL:
		Prefix = _T("[F] ");
		break;
	default:
		return FALSE;
	}
	 
	_ftprintf(stream,_T("%s %s\n"), Prefix, buffer);
	return TRUE;
}

/*
	Given DeviceInfoSet and DeviceInfoData
	This function updates string OEMInfFileName with the OEMxxx.INF file name (No path) and returns TRUE
	Otherwise returns FALSE
	*/
BOOL GetOEMInfFileName( HDEVINFO DeviceInfoSet, SP_DEVINFO_DATA DeviceInfoData, TCHAR * OEMInfFileName)
{
	TCHAR ErrMsg[1000];
	BOOL bRes=TRUE;

	StatusMessage( NULL, _T("GetOEMInfFileName: Starting"),  INFO);

	//// Preparations
	SP_DEVINSTALL_PARAMS deviceInstallParams;
	ZeroMemory(&deviceInstallParams, sizeof(deviceInstallParams));
	deviceInstallParams.cbSize = sizeof(SP_DEVINSTALL_PARAMS);
	bRes = SetupDiGetDeviceInstallParams(DeviceInfoSet, &DeviceInfoData, &deviceInstallParams);
	if (!bRes)
	{
		GetErrorString(ErrMsg,1000);
		_stprintf(prt, _T("GetOEMInfFileName: Function SetupDiGetDeviceInstallParams failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}
	else
		StatusMessage( NULL, _T("GetOEMInfFileName: Function SetupDiGetDeviceInstallParams OK"),  INFO);

	deviceInstallParams.FlagsEx |= (DI_FLAGSEX_INSTALLEDDRIVER | DI_FLAGSEX_ALLOWEXCLUDEDDRVS);
	bRes = SetupDiSetDeviceInstallParams(DeviceInfoSet, &DeviceInfoData, &deviceInstallParams);
	if (!bRes)
	{
		GetErrorString(ErrMsg,1000);
		_stprintf(prt, _T("GetOEMInfFileName: Function SetupDiSetDeviceInstallParams failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}
	else
		StatusMessage( NULL, _T("GetOEMInfFileName: Function SetupDiSetDeviceInstallParams OK"),  INFO);

	bRes = 	SetupDiBuildDriverInfoList(DeviceInfoSet, &DeviceInfoData, SPDIT_CLASSDRIVER);
	if (!bRes)
	{
		GetErrorString(ErrMsg,1000);
		_stprintf(prt, _T("GetOEMInfFileName: Function SetupDiBuildDriverInfoList failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}
	else
		StatusMessage( NULL, _T("GetOEMInfFileName: Function SetupDiBuildDriverInfoList OK"),  INFO);

	// Get some details about the driver:
	SP_DRVINFO_DATA driverInfoData;
	ZeroMemory(&driverInfoData,sizeof(driverInfoData));
    driverInfoData.cbSize = sizeof(driverInfoData);
	bRes = 	SetupDiEnumDriverInfo(DeviceInfoSet, &DeviceInfoData, SPDIT_CLASSDRIVER,0, &driverInfoData);
	if (!bRes)
	{
		GetErrorString(ErrMsg,1000);
		_stprintf(prt, _T("GetOEMInfFileName: Function SetupDiEnumDriverInfo failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	}
	else
	{
		_stprintf(prt, _T("GetOEMInfFileName: Function SetupDiEnumDriverInfo for \"%s\" OK"), driverInfoData.Description);
		StatusMessage( NULL, prt,  INFO);
	};

	// Retrieve driver information detail for  a particular device information element in the device information set.
	SP_DRVINFO_DETAIL_DATA driverInfoDetail;
	driverInfoDetail.cbSize = sizeof(SP_DRVINFO_DETAIL_DATA);
	bRes = 	SetupDiGetDriverInfoDetail(DeviceInfoSet, &DeviceInfoData,&driverInfoData,&driverInfoDetail,sizeof(SP_DRVINFO_DETAIL_DATA),NULL);
	if (!bRes)
	{
		DWORD errorcode = GetLastError();
		if (errorcode == ERROR_INSUFFICIENT_BUFFER)
		{
			_stprintf(prt, _T("GetOEMInfFileName: Function SetupDiGetDriverInfoDetail OK. INF file is %s"), driverInfoDetail.InfFileName);
			StatusMessage( NULL, prt,  INFO);
		}
		else
		{
			GetErrorString(ErrMsg,1000);
			_stprintf(prt, _T("GetOEMInfFileName: Function SetupDiGetDriverInfoDetail failed with error: %s"), ErrMsg);
			StatusMessage( NULL, prt,  ERR);
			return FALSE;
		};
	}
	else
	{
		_stprintf(prt, _T("GetOEMInfFileName: Function SetupDiGetDriverInfoDetail OK. INF file is %s"), driverInfoDetail.InfFileName);
		StatusMessage( NULL, prt,  INFO);
	};

	// Extract file name from path
	TCHAR Buffer[MAX_PATH], *p=NULL;
	DWORD size = GetFullPathName(driverInfoDetail.InfFileName, MAX_PATH , Buffer, &p);
	if (size || p)
	{
		_tcsncpy(OEMInfFileName, p, size);
		_stprintf(prt, _T("GetOEMInfFileName: Function GetFullPathName OK. INF file is %s"), p);
		StatusMessage( NULL, prt,  INFO);
	}
	else
	{
		GetErrorString(ErrMsg,1000);
		_stprintf(prt, _T("GetOEMInfFileName: Function GetFullPathName failed with error: %s"), ErrMsg);
		StatusMessage( NULL, prt,  ERR);
		return FALSE;
	};

	return TRUE;
}
