#ifndef DP_DEVICE_MANAGER_THREADS_H
#define DP_DEVICE_MANAGER_THREADS_H

#include <wx/thread.h>
#include "include/adb.hpp"
#include "events.hpp"

namespace droidpad {
	class DeviceManager;
	namespace threads
	{
		class DMInitialise : public wxThread
		{
			public:
				DMInitialise(DeviceManager &parent, AdbManager &adb);
				void* Entry();
			private:
				AdbManager &adb;
				DeviceManager &parent;
		};

		/**
		  * Manages general closing of things
		  */
		class DMClose : public wxThread
		{
			public:
				DMClose(DeviceManager &parent, AdbManager **adb);
				void* Entry();
			private:
				AdbManager **adb;
				DeviceManager &parent;
		};
	};
};

#endif
