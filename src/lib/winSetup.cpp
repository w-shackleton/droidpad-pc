#include "winSetup.hpp"

#include "data.hpp"

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
	wxString logFile = confLocation + wxT("installLog.txt");

	vJoyOpenLog(logFile);

	// Begin descent into WinAPI hell
	wxString hwId = vJoyGetDevHwId();

	switch(mode) {
	case MODE_SETUP: { // Setup DP if necessary.
		 } break;
	case MODE_REMOVE: { // Remove
		} break;
	}

	SetupEvent evt(SETUP_FINISHED);
	callback.AddPendingEvent(evt);

	Sleep(1000);

	{
		SetupEvent evt(SETUP_EXIT);
		callback.AddPendingEvent(evt);
		Cleanup();
	}
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
