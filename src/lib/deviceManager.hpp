#ifndef DP_DEVICEMANAGER_H
#define DP_DEVICEMANAGER_H

#include <wx/event.h>

#include "include/adb.hpp"
#include "deviceManagerThreads.hpp"
#include "events.hpp"

namespace droidpad {
	class DeviceManager : public wxEvtHandler {
		public:
			DeviceManager();

			DECLARE_EVENT_TABLE();

		private:
			AdbManager adb;
			void OnAdbInitialise(AdbEvent &event);

			threads::AdbInitialise *adbInit;
	};
};

#endif
