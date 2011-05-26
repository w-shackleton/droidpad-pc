#include "deviceManagerThreads.hpp"

#include "deviceManager.hpp"

using namespace droidpad;
using namespace droidpad::threads;

AdbInitialise::AdbInitialise(DeviceManager &parent, AdbManager &adb) :
	adb(adb),
	parent(parent)
{
}

void* AdbInitialise::Entry()
{
	if(adb.initialise())
	{
		AdbEvent evt(dpADB_INITIALISED, ADB_SUCCESS);
		parent.AddPendingEvent(evt);
	}
	else
	{
		AdbEvent evt(dpADB_INITIALISED, ADB_FAIL);
		parent.AddPendingEvent(evt);
	}
}
