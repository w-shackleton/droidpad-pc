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
			void Stop(bool wait = false);
			
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

			void OnMainThreadStarted(DMEvent &event);
			void OnMainThreadError(DMEvent &event);
			void OnMainThreadNotification(DMEvent &event);
			void OnMainThreadFinish(DMEvent &event);
	};
};

#endif
