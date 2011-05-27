#include "events.hpp"

using namespace droidpad;

IMPLEMENT_DYNAMIC_CLASS(DMEvent, wxEvent)
DEFINE_EVENT_TYPE(dpDM_INITIALISED)
DEFINE_EVENT_TYPE(dpDM_CLOSED)

DMEvent::DMEvent(wxEventType type, int status) :
	status(status)
{
	SetEventType(type);
}

wxEvent* DMEvent::Clone() const
{
	DMEvent* n = new DMEvent(GetEventType(), status);
	return n;
}
