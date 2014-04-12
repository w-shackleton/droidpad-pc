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
#include "net/secureConnection.hpp"

#include "events.hpp"
#include "log.hpp"
#include "data.hpp"

#include <string>
#include <iostream>
using namespace std;

using namespace droidpad;
using namespace droidpad::decode;

MainThread::MainThread(DeviceManager &parent, AndroidDevice &device) :
	parent(parent),
	device(device),
	running(true),
	mgr(NULL),
	deleteOutputManager(true)
{
	if(device.secureSupported) {
		LOGV("Starting a secure communication with the device");
		conn = new SecureConnection(device);
	} else {
		LOGV("Starting an INSECURE communication with the device");
		conn = new DPConnection(device);
	}
}

MainThread::~MainThread() {
	stop();
}

void* MainThread::Entry()
{
	LOGV("Starting DroidPad");
	bool connectAgain = false;
	bool setupDone = true;
	do { // connectAgain
		int setupResult;
		if((setupResult = setup()) != SETUP_SUCCESS) {
			if(connectAgain) {
				LOGW("Failed to reconnect, retrying...");
				wxMilliSleep(300);
				delete conn; // Reset
				if(device.secureSupported)
					conn = new SecureConnection(device);
				else
					conn = new DPConnection(device);
				continue;
			} else {
				setupDone = false;
				if(setupResult != SETUP_FAIL_QUIET) {
					DMEvent evt(dpTHREAD_ERROR, THREAD_ERROR_CONNECT_FAIL);
					parent.AddPendingEvent(evt);
				}
			}
		}

		if(setupDone) { // Skip this if fail - only run on first time.
			const ModeSetting &mode = conn->GetMode();
			if(mode.supportsBinary) conn->RequestBinary();
			if(!connectAgain) {
				try { // Setup outputmanager
					LOGV("Setting up OutputManager");

					switch(mode.type) {
						case MODE_JS:
						case MODE_SLIDE:
							mgr = new OutputManager(mode.type, mode.numRawAxes * 2 + mode.numAxes, mode.numButtons);
							break;
						case MODE_ABSMOUSE: {
							deleteOutputManager = false;
							OutputManager *innerMgr = new OutputManager(mode.type, 2 + mode.numAxes, mode.numButtons);
							mgr = new OutputSmoothBuffer(innerMgr, mode.type, 2 + mode.numAxes, mode.numButtons);
							break;
								    }
						case MODE_MOUSE:
							deleteOutputManager = false;
							OutputManager *innerMgr = new OutputManager(mode.type, mode.numRawAxes * 2 + mode.numAxes, mode.numButtons);
							mgr = new OutputSmoothBuffer(innerMgr, mode.type, mode.numRawAxes * 2 + mode.numAxes, mode.numButtons);
							break;
					}
				} catch(invalid_argument &e) {
					LOGEwx(wxString::FromAscii(e.what()));
					DMEvent evt(dpTHREAD_ERROR, THREAD_ERROR_NO_JS_DEVICE);
					parent.AddPendingEvent(evt);

					finish();
					return NULL;
				} catch(OutputException &e) {
					LOGEwx(wxString::FromAscii(e.what()));
					DMEvent evt(dpTHREAD_ERROR, THREAD_ERROR_NO_JS_DEVICE);
					parent.AddPendingEvent(evt);

					finish();
					return NULL;
				}

				DMEvent evt(dpTHREAD_STARTED, 0);
				parent.AddPendingEvent(evt);
			}
		}

		LOGV("Setup done");

		running = true; // Set in case was used to stop loop.
		DMEvent evt(dpTHREAD_NOTIFICATION, THREAD_INFO_CONNECTED);
		parent.AddPendingEvent(evt);

		while(running) {
			if(setupDone) {
				switch(loop()) {
					case LOOP_CONNLOST: {
					        LOGV("Loop sent connlost");
						connectAgain = true; // Try to reconnect.
						running = false; // Exit loop
						DMEvent evt(dpTHREAD_NOTIFICATION, THREAD_WARNING_CONNECTION_LOST); // This is now just a warning, not an error.
						parent.AddPendingEvent(evt);
							    }
						break;
					case LOOP_FINISHED: {
					        LOGV("Loop sent finished");
						setupDone = false; // Enter waiting loop.
						DMEvent evt(dpTHREAD_NOTIFICATION, THREAD_INFO_FINISHED);
						parent.AddPendingEvent(evt);
							    }
						break;
				}
			} else {
				wxThread::Sleep(30); // Wait to be killed by parent.
			}
		}
	} while(connectAgain);
	finish();
}

void MainThread::stop()
{
	LOGV("Thread received stop");
	running = false;
}

int MainThread::setup()
{
	// TODO: Only this on first time round?
	if(device.type == DEVICE_USB) parent.adb->forwardDevice(string(device.usbId.mb_str()), device.port);
	// TODO: Display more fitting errors. Perhaps LOGE displays errors to user in some cases?
	switch(conn->Start()) {
		case Connection::START_AUTHERROR:
			LOGE("Error authenticating with device");
			{ DMEvent evt(dpTHREAD_ERROR, THREAD_ERROR_NOT_PAIRED);
			parent.AddPendingEvent(evt); }
			return SETUP_FAIL_QUIET;
		case Connection::START_INITERROR:
			LOGE("Error while initialising connection with phone");
			return SETUP_FAIL;
		case Connection::START_HANDSHAKEERROR:
			LOGE("Error while communicating settings with phone");
			return SETUP_FAIL;
		case Connection::START_NETERROR:
			LOGE("Couldn't connect to phone");
			return SETUP_FAIL;
		case Connection::START_SUCCESS:
		default:
			return SETUP_SUCCESS;
	}

}

int MainThread::loop()
{
	try {
		DPJSData data = conn->GetData();
		if(data.connectionClosed) {
			LOGV("Received message from device indicating that connection was closed.");
			return LOOP_FINISHED;
		}
		switch(conn->GetMode().type) {
			case MODE_JS:
				data.reorder(Data::buttonOrder, Data::axisOrder);
				mgr->SendJSData(data);
				break;
			case MODE_MOUSE:
				mgr->SendMouseData(DPMouseData(data, prevData));
				break;
			case MODE_ABSMOUSE: {
				DPTouchData touchData = DPTouchData(data, prevData, prevAbsData);
				mgr->SendTouchData(touchData);
				prevAbsData = touchData;
					    } break;
			case MODE_SLIDE:
				mgr->SendSlideData(DPSlideData(data, prevData));
				break;
		}
		prevData = data;
	} catch(runtime_error e) {
		printf("GetData failed: %s\n", e.what());
		return LOOP_CONNLOST;
	}
	return LOOP_OK;
}

void MainThread::finish()
{
	if(mgr != NULL) {
		mgr->BeginToStop(); // If it is a thread, stop it.
		if(deleteOutputManager) delete mgr;
	}
	delete conn;

	DMEvent evt(dpTHREAD_FINISH, 0);
	parent.AddPendingEvent(evt);
}

