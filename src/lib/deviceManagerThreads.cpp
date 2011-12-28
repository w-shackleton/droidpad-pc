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
#include "deviceManagerThreads.hpp"

#include "deviceManager.hpp"
#include "log.hpp"

#include <wx/thread.h>
#include <wx/intl.h>

#include "net/deviceDiscover.hpp"

using namespace droidpad;
using namespace droidpad::threads;
using namespace std;

#define LOCALHOST "127.0.0.1"
#define LOCALHOST_PORT 3141

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

		// Custom device
		AndroidDevice custom;
		custom.type = DEVICE_CUSTOMHOST;
		custom.usbId = wxT("Custom device");
		custom.name = wxT("");
		devs.push_back(custom);

		vector<wxString> usbDevices = adb.getDeviceIds();
		for(int i = 0; i < usbDevices.size(); i++) {
			AndroidDevice dev;
			dev.type = DEVICE_USB;
			dev.usbId = usbDevices[i];
			dev.ip = wxT(LOCALHOST);
			dev.port = LOCALHOST_PORT;
			dev.name = _("USB Device");

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

