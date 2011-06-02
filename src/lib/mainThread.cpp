#include "mainThread.hpp"

#include "deviceManager.hpp"

#include "events.hpp"
#include "log.hpp"

#include <string>
#include <iostream>
using namespace std;

using namespace droidpad;

MainThread::MainThread(DeviceManager &parent, AndroidDevice &device) :
	parent(parent),
	device(device),
	running(true),
	mgr(NULL)
{
	conn = new DPConnection(device.ip, device.port);
}

void* MainThread::Entry()
{
	LOGV("Starting DroidPad");
	LOGVwx(wxT("Using device ") + device);
	bool setupDone = true;
	if(!setup()) {
		setupDone = false;
		DMEvent evt(dpTHREAD_ERROR, THREAD_ERROR_CONNECT_FAIL);
		parent.AddPendingEvent(evt);
	}

	if(!setupDone) { // Skip this if fail
		try { // Setup outputmanager
			const ModeSetting &mode = conn->GetMode();
			mgr = new OutputManager(conn->GetMode().type, mode.numRawAxes + mode.numAxes, mode.numButtons);
		} catch(invalid_argument e) {
			DMEvent evt(dpTHREAD_ERROR, THREAD_ERROR_SETUP_FAIL);
			parent.AddPendingEvent(evt);
		} catch(runtime_error e) {
			DMEvent evt(dpTHREAD_ERROR, THREAD_ERROR_SETUP_FAIL);
			parent.AddPendingEvent(evt);
		}
	}

	while(running) {
		if(setupDone) {
			loop();
		} else {
			wxThread::Sleep(30); // Wait to be killed by parent.
		}
	}
	finish();
}

void MainThread::stop()
{
	running = false;
}

bool MainThread::setup()
{
	if(device.type == DEVICE_USB) parent.adb->forwardDevice(string(device.usbId.mb_str()), device.port);
	if(!conn->Start()) {
		LOGE("Couldn't connect to phone");
		return false;
	}

	return true;
}

void MainThread::loop()
{
	const DPData data = conn->GetData();
}

void MainThread::finish()
{
	if(mgr != NULL) delete mgr;
	delete conn;

	DMEvent evt(dpTHREAD_FINISH, 0);
	parent.AddPendingEvent(evt);
}

