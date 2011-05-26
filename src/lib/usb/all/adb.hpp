#ifndef DP_ADB_H
#define DP_ADB_H

namespace droidpad {
	class AdbManager {
		public:
			AdbManager();
			~AdbManager();

			bool initialise();
	};
};

#endif
