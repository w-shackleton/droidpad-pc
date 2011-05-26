#include "deviceManager.hpp"

#include <iostream>

using namespace droidpad;
using namespace droidpad::threads;
using namespace std;

BEGIN_EVENT_TABLE(DeviceManager, wxEvtHandler)
	EVT_ADBEVENT(dpADB_INITIALISED, DeviceManager::OnAdbInitialise)
END_EVENT_TABLE()

DeviceManager::DeviceManager() :
	wxEvtHandler()
{
	adbInit = new AdbInitialise(*this, adb);
	adbInit->Create();
	adbInit->Run();
}

void DeviceManager::OnAdbInitialise(AdbEvent &event)
{
	cout << "ADB Initialised" << endl;
}
