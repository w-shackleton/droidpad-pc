#ifndef DP_DEVICEMANAGER_H
#define DP_DEVICEMANAGER_H

#include <wx/event.h>

#include "include/adb.hpp"
#include "deviceManagerThreads.hpp"
#include "droidpadCallbacks.hpp"
#include "events.hpp"

namespace droidpad {
	class DeviceManager : public wxEvtHandler {
		public:
			DeviceManager(DroidPadCallbacks &callbacks);

			void Close();

			DECLARE_EVENT_TABLE();

		private:
			AdbManager *adb;
			void OnInitialised(DMEvent &event);
			void OnClosed(DMEvent &event);

			threads::DMInitialise *initThread;
			threads::DMClose *closeThread;
			DroidPadCallbacks &callbacks;
	};
};

#endif
