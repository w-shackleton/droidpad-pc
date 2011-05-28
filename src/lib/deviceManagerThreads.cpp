#include "deviceManagerThreads.hpp"

#include "deviceManager.hpp"

#include <iostream>

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
	cout << "Device finder started" << endl;
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

	devDiscover->Delete();

	DMEvent evt(dpDEVICE_FINDER_FINISHED, DM_FINISHED);
	parent.AddPendingEvent(evt);
}

void DeviceFinder::stop() {
	running = false;
}

