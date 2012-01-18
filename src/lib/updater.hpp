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
#ifndef DP_UPDATER_H
#define DP_UPDATER_H

#include <wx/thread.h>
#include <wx/string.h>

#include "droidpadCallbacks.hpp"

namespace droidpad {
	class DeviceManager;

	namespace threads
	{
		class Updater : public wxThread {
			public:
				Updater(DeviceManager &parent, bool userRequest = false);
				virtual void* Entry();
			private:
				void parseUpdates(wxString updates);

				DeviceManager &parent;
				bool userRequest;
		};

		class UpdateDl : public wxThread {
			public:
				UpdateDl(DeviceManager &parent, UpdateInfo &info);
				virtual void* Entry();

				bool running;
			private:
				DeviceManager &parent;
				UpdateInfo info;
		};
	}
}

#endif
