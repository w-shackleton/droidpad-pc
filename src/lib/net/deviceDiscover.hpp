#ifndef DP_DEVICEDISCOVER_H
#define DP_DEVICEDISCOVER_H

#include <stdint.h>
#include "net/mdns.hpp"

namespace droidpad {
	class DeviceDiscover;

	class DeviceManager;
	class DeviceDiscover {
		public:
			DeviceDiscover(DeviceManager &parent);
			~DeviceDiscover();

			void getDevices();

		private:
			DeviceManager &parent;

			mdns::DeviceFinder df;
	};
}

#endif
