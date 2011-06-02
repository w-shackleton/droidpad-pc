#ifndef DP_DEVICEMANAGER_H
#define DP_DEVICEMANAGER_H

#include <wx/event.h>

#include "include/adb.hpp"
#include "deviceManagerThreads.hpp"
#include "droidpadCallbacks.hpp"
#include "events.hpp"

#include "mainThread.hpp"

#define DP_STATE_STOPPED 0
#define DP_STATE_STARTING 1
#define DP_STATE_STARTED 2

namespace droidpad {
	class DeviceManager : public wxEvtHandler {
		friend class MainThread;
		public:
			DeviceManager(DroidPadCallbacks &callbacks);
			~DeviceManager();

			void Close();

			void Start(int device);
			void Stop();
			
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
			MainThread *mainThread;
			DroidPadCallbacks &callbacks;

			AndroidDeviceList devices; // Reference?

			void OnMainThreadError(DMEvent &event);
			void OnMainThreadFinish(DMEvent &event);
	};
};

#endif
