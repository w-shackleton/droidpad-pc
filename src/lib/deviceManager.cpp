#include "deviceManager.hpp"

#include "log.hpp"

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
	finishing(false),
	state(DP_STATE_STOPPED)
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
	LOGV("Closing DeviceManager");
}

void DeviceManager::OnInitialised(DMEvent &event)
{
	LOGV("DeviceManager Initialised");
	callbacks.dpInitComplete(event.getStatus() == DM_SUCCESS);

	deviceFinder = new DeviceFinder(*this, *adb); // These threads delete themselves once started
	deviceFinder->Create();
	deviceFinder->Run();
}

void DeviceManager::OnClosed(DMEvent &event)
{
	LOGV("DeviceManager Closed");
	callbacks.dpCloseComplete();
}

void DeviceManager::OnDeviceFinderFinish(DMEvent &event)
{
	LOGV("Starting to close");
	closeThread = new DMClose(*this, &adb);
	closeThread->Create();
	closeThread->Run();
}

void DeviceManager::OnNewDevicesList(DevicesList &event)
{
	callbacks.dpNewDeviceList(event.list);
}

