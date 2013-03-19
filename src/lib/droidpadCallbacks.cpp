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
#include "droidpadCallbacks.hpp"

using namespace droidpad;

DroidPadCallbacks::~DroidPadCallbacks()
{
}

bool AndroidDevice::operator ==(const AndroidDevice& b)
{
	// No need to check secureSupported when checking equality
	return (type == b.type && usbId == b.usbId && ip == b.ip && name == b.name);
}

AndroidDevice::operator wxString() const {
	switch(type) {
		case DEVICE_USB:
			return wxString(wxT("USB ")) + usbId;
		case DEVICE_NET:
			return wxString::Format(wxT("Wifi %s (%s:%d) %s"), name.c_str(), ip.c_str(), port, secureSupported ? wxT("(Secure connection supported") : wxT(""));
		case DEVICE_CUSTOMHOST:
			return wxString(wxT("(Custom device)"));
	}
}

AndroidDevice::AndroidDevice() {
}

AndroidDevice::AndroidDevice(const AndroidDevice& dev) :
	type(dev.type),
	usbId(dev.usbId),
	ip(dev.ip),
	port(dev.port),
	name(dev.name)
{
}
