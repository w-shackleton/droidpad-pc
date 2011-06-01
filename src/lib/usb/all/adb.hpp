#ifndef DP_ADB_H
#define DP_ADB_H

#include <wx/string.h>
#include <vector>

#include <stdint.h>

namespace droidpad {
	class AdbManager {
		public:
			AdbManager();
			~AdbManager();

			bool initialise();

			std::vector<wxString> getDeviceIds();

			void forwardDevice(std::string serial, uint16_t from, uint16_t to);
			inline void forwardDevice(std::string serial, uint16_t port) {
				forwardDevice(serial, port, port);
			}

		private:
			std::string adbCmd;
	};
};

#endif
