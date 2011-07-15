/*
 * This file is part of DroidPad.
 * DroidPad lets you use an Android mobile to control a joystick or mouse
 * on a Windows or Linux computer.
 *
 * DroidPad is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DroidPad is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DroidPad, in the file COPYING.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DP_MAIN_THREAD_H
#define DP_MAIN_THREAD_H

#include <wx/thread.h>
#include "droidpadCallbacks.hpp"
#include "include/adb.hpp"
#include "output/IOutputMgr.hpp"
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
			~MainThread();
			void* Entry();

			void stop();
		private:
			DeviceManager &parent;
			AndroidDevice &device;

			// The implementation changes per platform here
			IOutputManager *mgr;
			bool deleteOutputManager;

			DPConnection *conn;

			DPJSData prevData;

			bool running;

			bool setup();
			void loop();
			void finish();
	};
}

#endif
