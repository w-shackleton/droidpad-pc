#ifndef DP_DEVICE_MANAGER_THREADS_H
#define DP_DEVICE_MANAGER_THREADS_H

#include <wx/thread.h>
#include "include/adb.hpp"
#include "events.hpp"

namespace droidpad {
	class DeviceManager;
	namespace threads
	{
		class AdbInitialise : public wxThread
		{
			public:
				AdbInitialise(DeviceManager &parent, AdbManager &adb);
				void* Entry();
			private:
				AdbManager &adb;
				DeviceManager &parent;
		};

	};
};

#endif
