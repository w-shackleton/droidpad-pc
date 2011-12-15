#include "winSetup.hpp"

#include <string>
#include <iostream>
using namespace std;
using namespace droidpad;

DEFINE_LOCAL_EVENT_TYPE(SETUP_BLANKSTATUS)
DEFINE_LOCAL_EVENT_TYPE(SETUP_INITIALISED)
DEFINE_LOCAL_EVENT_TYPE(REMOVE_INITIALISED)

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

	SetupEvent evt(SETUP_FINISHED);
	callback.AddPendingEvent(evt);

	Sleep(1000);

	{
		SetupEvent evt(SETUP_EXIT);
		callback.AddPendingEvent(evt);
	}
}



SetupEvent::SetupEvent(wxEventType type) {
	SetEventType(type);
}

wxEvent* SetupEvent::Clone() const
{
	SetupEvent* n = new SetupEvent(GetEventType());
	return n;
}
