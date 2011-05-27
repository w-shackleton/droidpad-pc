#include "deviceManager.hpp"

#include <iostream>

using namespace droidpad;
using namespace droidpad::threads;
using namespace std;

BEGIN_EVENT_TABLE(DeviceManager, wxEvtHandler)
	EVT_DMEVENT(dpDM_INITIALISED, DeviceManager::OnInitialised)
	EVT_DMEVENT(dpDM_CLOSED, DeviceManager::OnClosed)
END_EVENT_TABLE()

DeviceManager::DeviceManager(DroidPadCallbacks &callbacks) :
	wxEvtHandler(),
	callbacks(callbacks)
{
	adb = new AdbManager;
	initThread = new DMInitialise(*this, *adb);
	initThread->Create();
	initThread->Run();
}

void DeviceManager::Close()
{
	closeThread = new DMClose(*this, &adb);
	closeThread->Create();
	closeThread->Run();
}

void DeviceManager::OnInitialised(DMEvent &event)
{
	cout << "DeviceManager Initialised" << endl;
	callbacks.dpInitComplete(event.getStatus() == DM_SUCCESS);
}

void DeviceManager::OnClosed(DMEvent &event)
{
	cout << "DeviceManager Closed" << endl;
	callbacks.dpCloseComplete();
}
