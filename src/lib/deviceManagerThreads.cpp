#include "deviceManagerThreads.hpp"

#include "deviceManager.hpp"

#include "log.hpp"

#include <wx/thread.h>

#include "net/deviceDiscover.hpp"

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
	LOGV("Starting ADB");
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
	LOGV("Stopping ADB");
	delete *adb;
	*adb = NULL;
	LOGV("ADB stopped");

	DMEvent evt(dpDM_CLOSED);
	parent.AddPendingEvent(evt);
}

DeviceFinder::DeviceFinder(DeviceManager &parent, AdbManager &adb) :
	adb(adb),
	parent(parent),
	running(true)
{
}

void* DeviceFinder::Entry()
{
	devDiscover = new DeviceDiscover(parent);
	devDiscover->Create(); // TODO: Add error handling
	devDiscover->Run();
	LOGV("Device finder started");
	do {
		AndroidDeviceList devs;
		vector<wxString> usbDevices = adb.getDeviceIds();
		for(int i = 0; i < usbDevices.size(); i++) {
			AndroidDevice dev;
			dev.type = DEVICE_USB;
			dev.usbId = usbDevices[i];
			dev.ip = wxT("");
			dev.port = 0;
			dev.name = wxT("");

			devs.push_back(dev);
		}
		map<wxString, mdns::Device> netDevices = devDiscover->getDevices();

		map<wxString, mdns::Device>::const_iterator end = netDevices.end();
		for(map<wxString, mdns::Device>::const_iterator it = netDevices.begin(); it != end; ++it)
		{
			AndroidDevice dev;
			dev.type = DEVICE_NET;
			dev.usbId = it->second.ip;
			dev.ip = it->second.ip;
			dev.port = it->second.port;
			dev.name = it->second.deviceDescription;

			devs.push_back(dev);
		}

		wxThread::Sleep(1000);

		DevicesList list(devs);
		parent.AddPendingEvent(list);
	} while(running);


	DMEvent evt(dpDEVICE_FINDER_FINISHED, DM_FINISHED);
	parent.AddPendingEvent(evt);
	devDiscover->Delete();
	LOGV("Device finder finished, closing now.");
}

void DeviceFinder::stop() {
	LOGV("Stopping Device finder");
	running = false;
}

