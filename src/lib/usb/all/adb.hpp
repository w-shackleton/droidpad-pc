#ifndef DP_ADB_H
#define DP_ADB_H

#include <wx/string.h>
#include <vector>

namespace droidpad {
	class AdbManager {
		public:
			AdbManager();
			~AdbManager();

			bool initialise();

			std::vector<wxString> getDeviceIds();

		private:
			std::string adbCmd;
	};
};

#endif
