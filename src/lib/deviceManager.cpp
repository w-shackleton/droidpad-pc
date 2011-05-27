#include "deviceManager.hpp"

#include <iostream>

using namespace droidpad;
using namespace droidpad::threads;
using namespace std;

BEGIN_EVENT_TABLE(DeviceManager, wxEvtHandler)
	EVT_DMEVENT(dpDM_INITIALISED, DeviceManager::OnInitialised)
	EVT_DMEVENT(dpDM_CLOSED, DeviceManager::OnClosed)
	EVT_DMEVENT(dpDEVICE_FINDER_FINISHED, DeviceManager::OnDeviceFinderFinish)

	EVT_DEVICES_LIST(dpDEVICES_LIST, DeviceManager::OnNewDevicesList)
END_EVENT_TABLE()

DeviceManager::DeviceManager(DroidPadCallbacks &callbacks) :
	wxEvtHandler(),
	callbacks(callbacks),
	finishing(false)
{
	adb = new AdbManager;
	initThread = new DMInitialise(*this, *adb);
	initThread->Create();
	initThread->Run();
}

void DeviceManager::Close()
{
	finishing = true;
	deviceFinder->stop();
	cout << "Closing" << endl;
}

void DeviceManager::OnInitialised(DMEvent &event)
{
	cout << "DeviceManager Initialised" << endl;
	callbacks.dpInitComplete(event.getStatus() == DM_SUCCESS);

	deviceFinder = new DeviceFinder(*this, *adb); // These threads delete themselves once started
	deviceFinder->Create();
	deviceFinder->Run();
}

void DeviceManager::OnClosed(DMEvent &event)
{
	cout << "DeviceManager Closed" << endl;
	callbacks.dpCloseComplete();
}

void DeviceManager::OnDeviceFinderFinish(DMEvent &event)
{
	closeThread = new DMClose(*this, &adb);
	closeThread->Create();
	closeThread->Run();
}

void DeviceManager::OnNewDevicesList(DevicesList &event)
{
	callbacks.dpNewDeviceList(event.list);
}

