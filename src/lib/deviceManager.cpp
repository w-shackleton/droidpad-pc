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
#include "deviceManager.hpp"

#include "log.hpp"

#include <wx/intl.h>

#ifdef OS_WIN32
#include "updater.hpp"
#endif

using namespace droidpad;
using namespace droidpad::threads;
using namespace std;

BEGIN_EVENT_TABLE(DeviceManager, wxEvtHandler)
	EVT_DMEVENT(dpDM_INITIALISED, DeviceManager::OnInitialised)
	EVT_DMEVENT(dpDM_CLOSED, DeviceManager::OnClosed)
	EVT_DMEVENT(dpDEVICE_FINDER_FINISHED, DeviceManager::OnDeviceFinderFinish)

	EVT_DMEVENT(dpTHREAD_STARTED, DeviceManager::OnMainThreadStarted)
	EVT_DMEVENT(dpTHREAD_ERROR, DeviceManager::OnMainThreadError)
	EVT_DMEVENT(dpTHREAD_NOTIFICATION, DeviceManager::OnMainThreadNotification)
	EVT_DMEVENT(dpTHREAD_FINISH, DeviceManager::OnMainThreadFinish)

	EVT_DEVICES_LIST(dpDEVICES_LIST, DeviceManager::OnNewDevicesList)

	EVT_NEW_UPDATES(dpUPDATE_NOTIFICATION, DeviceManager::OnNewUpdates)

	EVT_DL_PROGRESS(dpDL_STARTED, DeviceManager::OnDlStarted)
	EVT_DL_PROGRESS(dpDL_PROGRESS, DeviceManager::OnDlProgress)
	EVT_DL_PROGRESS(dpDL_FAILED, DeviceManager::OnDlFailed)
	EVT_DL_PROGRESS(dpDL_SUCCESS, DeviceManager::OnDlSuccess)
END_EVENT_TABLE()

DeviceManager::DeviceManager(DroidPadCallbacks &callbacks) :
	wxEvtHandler(),
	callbacks(callbacks),
	finishing(false),
	state(DP_STATE_STOPPED),
	mainThread(NULL),
	deviceFinder(NULL)
{
	adb = new AdbManager;
	initThread = new DMInitialise(*this, *adb);
	initThread->Create();
	initThread->Run();
}

DeviceManager::~DeviceManager()
{
	delete adb;
}

void DeviceManager::Close()
{
	finishing = true;
	if(deviceFinder) deviceFinder->stop();
	LOGV("Closing DeviceManager");

	Stop();
}

void DeviceManager::OnInitialised(DMEvent &event)
{
	LOGV("DeviceManager Initialised");
	callbacks.dpInitComplete(event.getStatus() == DM_SUCCESS);

	if(finishing) {
		LOGV("Starting to close (1)");
		closeThread = new DMClose(*this, &adb);
		closeThread->Create();
		closeThread->Run();
	}

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
	devices = event.list;
}

void DeviceManager::Start(int device)
{
	LOGV("Starting");

	AndroidDevice newDevice(devices[device]); // Copy
	if(!callbacks.customiseDevice(&newDevice)) { // If fails
		DMEvent evt(dpTHREAD_FINISH, 0);
		AddPendingEvent(evt);
		return;
	}
	mainThread = new MainThread(*this, newDevice);
	mainThread->Create();
	mainThread->Run();
	state = DP_STATE_STARTING;
}

void DeviceManager::Stop()
{
	if(state == DP_STATE_STARTED || state == DP_STATE_STARTING) {
		mainThread->stop();
		mainThread->Delete();
	}
}

void DeviceManager::RequestUpdates(bool userRequest) {
#ifdef OS_WIN32
	Updater *updater = new Updater(*this, userRequest);
	updater->Create();
	updater->Run();
#endif
	// else do nothing
}

#ifdef OS_WIN32
void DeviceManager::StartUpdate(UpdateInfo update) {
	updateDl = new UpdateDl(*this, update);
	updateDl->Create();
	updateDl->Run();
}

void DeviceManager::CancelUpdate() {
	if(updateDl)
		updateDl->running = false; // Let it finish
	updateDl = NULL;
	callbacks.updateCompleted(true);
}
#endif

void DeviceManager::OnNewUpdates(UpdatesNotification &event) {
	callbacks.updatesAvailable(event.versions, event.latest, event.userRequest);
}


void DeviceManager::OnDlStarted(DlStatus &event) {
	callbacks.updateStarted();
}
void DeviceManager::OnDlProgress(DlStatus &event) {
	callbacks.updateProgress(event.bytesDone, event.totalBytes);
}
void DeviceManager::OnDlFailed(DlStatus &event) {
	callbacks.updateFailed();
	updateDl = NULL; // So functions in it don't get called.
}
void DeviceManager::OnDlSuccess(DlStatus &event) {
	callbacks.updateCompleted();
	updateDl = NULL;
}

void DeviceManager::OnMainThreadStarted(DMEvent &event)
{
	state = DP_STATE_STARTED;
	callbacks.threadStarted();
}

void DeviceManager::OnMainThreadError(DMEvent &event)
{
	switch(event.getStatus()) {
		case THREAD_ERROR_CONNECT_FAIL:
			LOGE("Recieved error when connecting");
			callbacks.threadError(_("Couldn't connect to phone"));
			break;
		case THREAD_ERROR_SETUP_FAIL:
			LOGE("Recieved error when setting up interfaces");
			callbacks.threadError(_("Couldn't setup DroidPad"));
			break;
		case THREAD_ERROR_NO_JS_DEVICE:
			LOGE("Joystick device couldn't be found.");
			callbacks.threadError(_("Couldn't find joystick device. Is it installed properly?"));
			break;
		default:
			LOGE("Other error from thread");
			callbacks.threadError(wxString::Format(_("Unknown Error - %d."), event.getStatus()));
	}
	Stop();
}

void DeviceManager::OnMainThreadNotification(DMEvent &event)
{
	switch(event.getStatus()) {
		case THREAD_WARNING_CONNECTION_LOST:
			LOGE("Connection to phone lost.");
			callbacks.setStatusText(_("Connection to phone lost. Retrying..."));
			break;
		case THREAD_INFO_FINISHED:
			LOGV("Finished.");
			callbacks.setStatusText(_("Done."));
			Stop();
			break;
		case THREAD_INFO_CONNECTED:
			LOGV("Connected.");
			callbacks.setStatusText(_("Connected."));
			break;
		default:
			LOGE("Other error from thread");
			callbacks.threadError(wxString::Format(_("Unknown warning - %d."), event.getStatus()));
	}
}

void DeviceManager::OnMainThreadFinish(DMEvent &event)
{
	state = DP_STATE_STOPPED;
	callbacks.threadStopped();
}

