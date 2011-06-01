#include "droidpadCallbacks.hpp"

using namespace droidpad;

DroidPadCallbacks::~DroidPadCallbacks()
{
}

AndroidDevice& AndroidDevice::operator =(const AndroidDevice& b)
{
	AndroidDevice ret;
	ret.type = type;
	ret.usbId = usbId;
	ret.ip = ip;
	ret.name = name;
}

bool AndroidDevice::operator ==(const AndroidDevice& b)
{
	return (type == b.type && usbId == b.usbId && ip == b.ip && name == b.name);
}

AndroidDevice::operator wxString() const {
	switch(type) {
		case DEVICE_USB:
			return wxT("USB ") + usbId;
		case DEVICE_NET:
			return wxString(wxT("Wifi ") + name + wxT(" (") + ip + wxT(":") + wxString::Format(wxT("%d"), port) + wxT(")"));
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
