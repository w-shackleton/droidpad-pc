#include "deviceDiscover.hpp"

using namespace droidpad;
using namespace droidpad::mdns;

#include "deviceManager.hpp"

#include <iostream>
using namespace std;


DeviceDiscover::DeviceDiscover(DeviceManager &parent) :
	parent(parent)
{
}

DeviceDiscover::~DeviceDiscover()
{
}
