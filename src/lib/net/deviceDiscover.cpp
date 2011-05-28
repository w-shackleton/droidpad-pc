#include "deviceDiscover.hpp"

using namespace droidpad;
using namespace droidpad::mdns;

#include "deviceManager.hpp"

#include <iostream>
using namespace std;


DeviceDiscover::DeviceDiscover(DeviceManager &parent) :
	parent(parent),
	df(this),
	dataAvailable(false)
{
}

DeviceDiscover::~DeviceDiscover()
{

}

void* DeviceDiscover::Entry()
{
	df.start();
}

void DeviceDiscover::cycle()
{
	if(TestDestroy()) df.stop();
}

void DeviceDiscover::onData()
{
	dataAvailable = true;
}

map<wxString, Device> DeviceDiscover::getDevices()
{
	map<wxString, Device> devs(df.devices);
	return devs;
}
