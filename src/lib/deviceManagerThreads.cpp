#include "deviceManagerThreads.hpp"

#include "deviceManager.hpp"

#include <iostream>

using namespace droidpad;
using namespace droidpad::threads;
using namespace std;

DMInitialise::DMInitialise(DeviceManager &parent, AdbManager &adb) :
	adb(adb),
	parent(parent)
{
}

void* DMInitialise::Entry()
{
	cout << "Starting ADB" << endl;
	if(adb.initialise())
	{
		DMEvent evt(dpDM_INITIALISED, DM_SUCCESS);
		parent.AddPendingEvent(evt);
	}
	else
	{
		DMEvent evt(dpDM_INITIALISED, DM_FAIL);
		parent.AddPendingEvent(evt);
	}
}

DMClose::DMClose(DeviceManager &parent, AdbManager **adb) :
	adb(adb),
	parent(parent)
{
}

void* DMClose::Entry()
{
	cout << "Stopping ADB" << endl;
	delete *adb;
	*adb = NULL;

	DMEvent evt(dpDM_CLOSED);
	parent.AddPendingEvent(evt);
}
