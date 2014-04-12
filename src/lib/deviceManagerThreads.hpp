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
#ifndef DP_DEVICE_MANAGER_THREADS_H
#define DP_DEVICE_MANAGER_THREADS_H

#include <wx/thread.h>
#include "include/adb.hpp"
#include "events.hpp"

namespace droidpad {
	class DeviceManager;
	class DeviceDiscover;
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

		/**
		  * Looping thread for finding devices
		  */
		class DeviceFinder : public wxThread
		{
			public:
				DeviceFinder(DeviceManager &parent, AdbManager &adb);
				void* Entry();

				void stop();
			private:
				AdbManager &adb;
				DeviceManager &parent;
				DeviceDiscover *devDiscover;

				bool running;
		};
	};
};

#endif
