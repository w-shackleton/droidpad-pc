#ifndef DP_DEVICEDISCOVER_H
#define DP_DEVICEDISCOVER_H

#include <stdint.h>
#include "net/mdns.hpp"
#include <wx/thread.h>

namespace droidpad {
	class DeviceManager;

	class DeviceDiscover : public wxThread, protected mdns::Callbacks {
		public:
			DeviceDiscover(DeviceManager &parent);
			~DeviceDiscover();

			
			std::map<wxString, mdns::Device> getDevices();

			bool dataAvailable;

			virtual void* Entry();

		private:
			void cycle();
			virtual void onData();

			DeviceManager &parent;

			mdns::DeviceFinder df;
	};
}

#endif
