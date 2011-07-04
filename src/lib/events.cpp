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
#include "events.hpp"

using namespace droidpad;
using namespace std;

IMPLEMENT_DYNAMIC_CLASS(DMEvent, wxEvent)
DEFINE_LOCAL_EVENT_TYPE(dpDM_INITIALISED)
DEFINE_LOCAL_EVENT_TYPE(dpDM_CLOSED)
DEFINE_LOCAL_EVENT_TYPE(dpDEVICE_FINDER_FINISHED)

DEFINE_LOCAL_EVENT_TYPE(dpDEVICES_LIST)

DEFINE_LOCAL_EVENT_TYPE(dpTHREAD_STARTED)
DEFINE_LOCAL_EVENT_TYPE(dpTHREAD_ERROR)
DEFINE_LOCAL_EVENT_TYPE(dpTHREAD_FINISH)

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
