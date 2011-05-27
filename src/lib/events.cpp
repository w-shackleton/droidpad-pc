#include "events.hpp"

using namespace droidpad;
using namespace std;

IMPLEMENT_DYNAMIC_CLASS(DMEvent, wxEvent)
DEFINE_EVENT_TYPE(dpDM_INITIALISED)
DEFINE_EVENT_TYPE(dpDM_CLOSED)
DEFINE_EVENT_TYPE(dpDEVICE_FINDER_FINISHED)

DEFINE_EVENT_TYPE(dpDEVICES_LIST)

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

IMPLEMENT_DYNAMIC_CLASS(DevicesList, wxEvent)

DevicesList::DevicesList(std::vector<AndroidDevice> list) :
	list(list)
{
	SetEventType(dpDEVICES_LIST);
}

wxEvent* DevicesList::Clone() const
{
	DevicesList* n = new DevicesList(list);
	return n;
}
