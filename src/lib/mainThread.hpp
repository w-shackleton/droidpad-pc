#ifndef DP_MAIN_THREAD_H
#define DP_MAIN_THREAD_H

#include <wx/thread.h>
#include "droidpadCallbacks.hpp"
#include "include/adb.hpp"
#include "net/connection.hpp"

namespace droidpad {
	class DeviceManager;

	/**
	  * Main thread - connects & does all the magick.
	  */
	class MainThread : public wxThread
	{
		public:
			MainThread(DeviceManager &parent, AndroidDevice &device);
			void* Entry();

			void stop();
		private:
			DeviceManager &parent;
			AndroidDevice &device;

			DPConnection *conn;

			bool running;

			bool setup();
			void loop();
			void finish();
	};
}

#endif
