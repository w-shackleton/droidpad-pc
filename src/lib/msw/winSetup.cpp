#include "winSetup.hpp"

#include "data.hpp"
#include "log.hpp"

#include "win32/winUtil.hpp"
#include "win32/vjoy.hpp"

#include <wx/intl.h>
#include <wx/file.h>
#include <wx/stdpaths.h>

#include <string>
#include <iostream>
using namespace std;
using namespace droidpad;

DEFINE_LOCAL_EVENT_TYPE(SETUP_BLANKSTATUS)
DEFINE_LOCAL_EVENT_TYPE(SETUP_INITIALISED)
DEFINE_LOCAL_EVENT_TYPE(REMOVE_INITIALISED)

DEFINE_LOCAL_EVENT_TYPE(SETUP_REMOVING_OLD)
DEFINE_LOCAL_EVENT_TYPE(SETUP_INSTALLING_NEW)

DEFINE_LOCAL_EVENT_TYPE(SETUP_SHOW_DRIVERCHOICE)

DEFINE_LOCAL_EVENT_TYPE(REMOVE_REMOVING)

DEFINE_LOCAL_EVENT_TYPE(SETUP_ERROR)

DEFINE_LOCAL_EVENT_TYPE(SETUP_FINISHED)
DEFINE_LOCAL_EVENT_TYPE(SETUP_EXIT) 
IMPLEMENT_DYNAMIC_CLASS(SetupEvent, wxEvent)

SetupThread::SetupThread(wxEvtHandler &callback, int mode) :
	mode(mode),
	callback(callback)
{
}

SetupThread::~SetupThread() {
}

// Sets up everything required on Windows.
void* SetupThread::Entry() {
	switch(mode) {
	case MODE_SETUP: { // Setup DP if necessary.
		SetupEvent evt(SETUP_INITIALISED);
		callback.AddPendingEvent(evt);
		 } break;
	case MODE_REMOVE: { // Remove
		SetupEvent evt(REMOVE_INITIALISED);
		callback.AddPendingEvent(evt);
		} break;
	}

#ifdef OS_64BIT
	{
		SetupEvent evt(SETUP_SHOW_DRIVERCHOICE);
		callback.AddPendingEvent(evt);
		return NULL; // TODO: Add waiting for user response.
	}
#endif

	LOGM("Starting setup");

	wxString infFileLocation;
	switch(cpuType()) {
		case CPU_amd64:
			infFileLocation = Data::getFilePath(wxT("driver/amd64/vJoy.inf"));
			break;
		case CPU_x86:
			infFileLocation = Data::getFilePath(wxT("driver/x86/vJoy.inf"));
			break;
		default:
			SetupEvent evt(SETUP_ERROR, _("Incompatible processor architecture."));
			callback.AddPendingEvent(evt);
			return NULL;
	}

	if(!wxFileExists(infFileLocation)) {
		SetupEvent evt(SETUP_ERROR, _("Couldn't find drivers in DroidPad installation."));
		callback.AddPendingEvent(evt);
		Cleanup();
		return NULL;
	}

	// Add log for install.
	wxString confLocation = wxStandardPaths::Get().GetUserDataDir();
	if(!wxDirExists(confLocation)) wxMkdir(confLocation);
	wxString logFile = confLocation + wxT("/installLog.txt");

	LOGMwx(wxString(wxT("Using inf file at ") + infFileLocation));

	vJoyOpenLog(logFile);

	wxString hwId = vJoyGetDevHwId();
	LOGMwx(wxString(wxT("HW ID is ") + hwId));

	switch(mode) {
		case MODE_SETUP: // Setup DP if necessary.
			if(vJoyIsInstalled()) { // Installed, so remove first.
				LOGM("vJoy already installed, removing first... ");
				{
					SetupEvent evt(SETUP_REMOVING_OLD);
					callback.AddPendingEvent(evt);
				}
				if(!vJoyRemove(infFileLocation, hwId)) {
					LOGE("ERROR: Couldn't remove old vJoy version!");
				}
			} else {
				LOGM("first vJoy install.");
			}
			{
				SetupEvent evt(SETUP_INSTALLING_NEW);
				callback.AddPendingEvent(evt);
			}
			if(!vJoyInstall(infFileLocation, hwId)) {
				LOGE("ERROR: Couldn't install new vJoy version!");
				{
					SetupEvent evt(SETUP_ERROR, _("New driver couldn't be installed correctly."));
					callback.AddPendingEvent(evt);
					Cleanup();
					return NULL;
				}
			}

			break;
		case MODE_REMOVE: // Remove
			{
				SetupEvent evt(REMOVE_REMOVING);
				callback.AddPendingEvent(evt);
			}
			vJoyPurge(infFileLocation, hwId);
			break;
	}

	SetupEvent evt(SETUP_FINISHED);
	callback.AddPendingEvent(evt);

	Sleep(1000);

	{
		SetupEvent evt(SETUP_EXIT);
		callback.AddPendingEvent(evt);
		Cleanup();
	}
	return NULL;
}

void SetupThread::Cleanup() {
	vJoyCloseLog();
}


SetupEvent::SetupEvent(wxEventType type, wxString message) :
	message(message)
{
	SetEventType(type);
}

wxEvent* SetupEvent::Clone() const
{
	SetupEvent* n = new SetupEvent(GetEventType(), message);
	return n;
}
