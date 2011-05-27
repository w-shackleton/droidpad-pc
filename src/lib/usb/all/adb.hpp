#ifndef DP_ADB_H
#define DP_ADB_H

#include <string>

namespace droidpad {
	class AdbManager {
		public:
			AdbManager();
			~AdbManager();

			bool initialise();

		private:
			std::string adbCmd;
	};
};

#endif
