#include "events.hpp"

using namespace droidpad;

IMPLEMENT_DYNAMIC_CLASS(AdbEvent, wxEvent)
DEFINE_EVENT_TYPE(dpADB_INITIALISED)

AdbEvent::AdbEvent(wxEventType type, int status) :
	status(status)
{
	SetEventType(type);
}

wxEvent* AdbEvent::Clone() const
{
	AdbEvent* n = new AdbEvent(GetEventType(), status);
	return n;
}
