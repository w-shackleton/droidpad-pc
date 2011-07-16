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
#include "mainThread.hpp"

#include "deviceManager.hpp"
#include "include/outputMgr.hpp"
#include "output/outputSmoothBuffer.hpp"

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
	mgr(NULL),
	deleteOutputManager(true)
{
	conn = new DPConnection(device.ip, device.port);
}

MainThread::~MainThread() {
	stop();
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

	if(setupDone) { // Skip this if fail
		try { // Setup outputmanager
			const ModeSetting &mode = conn->GetMode();
#ifdef OS_LINUX
			switch(mode.type) {
				case MODE_JS:
				case MODE_SLIDE:
					mgr = new OutputManager(mode.type, mode.numRawAxes * 2 + mode.numAxes, mode.numButtons);
					break;
				case MODE_MOUSE:
					deleteOutputManager = false;
					OutputManager *innerMgr = new OutputManager(mode.type, mode.numRawAxes * 2 + mode.numAxes, mode.numButtons);
					mgr = new OutputSmoothBuffer(innerMgr, mode.type, mode.numRawAxes * 2 + mode.numAxes, mode.numButtons);
					break;
			}
#elif defined OS_WIN32
			switch(mode.type) {
				case MODE_JS:
				case MODE_SLIDE:
					mgr = new OutputManager(mode.type, mode.numRawAxes * 2 + mode.numAxes, mode.numButtons);
					break;
				case MODE_MOUSE:
					deleteOutputManager = false;
					OutputManager *innerMgr = new OutputManager(mode.type, mode.numRawAxes * 2 + mode.numAxes, mode.numButtons);
					mgr = new OutputSmoothBuffer(innerMgr, mode.type, mode.numRawAxes * 2 + mode.numAxes, mode.numButtons);
					break;
			}
#endif
		} catch(invalid_argument e) {
			DMEvent evt(dpTHREAD_ERROR, THREAD_ERROR_SETUP_FAIL);
			parent.AddPendingEvent(evt);
		} catch(runtime_error e) {
			DMEvent evt(dpTHREAD_ERROR, THREAD_ERROR_SETUP_FAIL);
			parent.AddPendingEvent(evt);
		}

		DMEvent evt(dpTHREAD_STARTED, 0);
		parent.AddPendingEvent(evt);
	}

	LOGV("Setup done");

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
	const DPJSData data = conn->GetData();
	// TODO: Call corresponding method.
	switch(conn->GetMode().type) {
		case MODE_JS:
			mgr->SendJSData(data);
			break;
		case MODE_MOUSE:
			mgr->SendMouseData(DPMouseData(data, prevData));
			break;
		case MODE_SLIDE:
			mgr->SendSlideData(DPSlideData(data, prevData));
			break;
	}
	prevData = data;
}

void MainThread::finish()
{
	if(mgr != NULL || deleteOutputManager) delete mgr;
	delete conn;

	DMEvent evt(dpTHREAD_FINISH, 0);
	parent.AddPendingEvent(evt);
}

