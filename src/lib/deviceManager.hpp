#ifndef DP_DEVICEMANAGER_H
#define DP_DEVICEMANAGER_H

#include <wx/event.h>

#include "include/adb.hpp"
#include "deviceManagerThreads.hpp"
#include "droidpadCallbacks.hpp"
#include "events.hpp"

#define DP_STATE_STOPPED 0
#define DP_STATE_STARTED 1

namespace droidpad {
	class DeviceManager : public wxEvtHandler {
		public:
			DeviceManager(DroidPadCallbacks &callbacks);

			void Close();

			void Start(int device);
			
			inline int getState() {
				return state;
			}

			DECLARE_EVENT_TABLE();

		private:
			int state;

			AdbManager *adb;
			void OnInitialised(DMEvent &event);
			void OnClosed(DMEvent &event);
			void OnDeviceFinderFinish(DMEvent &event);
			void OnNewDevicesList(DevicesList &event);

			bool finishing;

			threads::DMInitialise *initThread;
			threads::DMClose *closeThread;
			threads::DeviceFinder *deviceFinder;
			DroidPadCallbacks &callbacks;
	};
};

#endif
