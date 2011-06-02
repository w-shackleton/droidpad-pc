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
	running(true)
{
	conn = new DPConnection(device.ip, device.port);
}

void* MainThread::Entry()
{
	LOGV("Starting DroidPad");
	LOGVwx(wxT("Using device ") + device);
	if(!setup()) {
		DMEvent evt(dpTHREAD_ERROR, THREAD_ERROR_CONNECT_FAIL);
		parent.AddPendingEvent(evt);
	}
	while(running) {
		loop();
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
	switch(conn->GetMode().type) {
		case MODE_JS:
			break;
		case MODE_MOUSE:
			break;
		case MODE_SLIDE:
			break;
	}
	return true;
}

void MainThread::loop()
{
}

void MainThread::finish()
{
	delete conn;
}

