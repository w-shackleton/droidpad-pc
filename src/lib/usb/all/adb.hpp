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
#ifndef DP_ADB_H
#define DP_ADB_H

#include <wx/string.h>
#include <vector>

#include <stdint.h>

namespace droidpad {
	class AdbManager {
		public:
			AdbManager();
			~AdbManager();

			bool initialise();

			std::vector<wxString> getDeviceIds();

			void forwardDevice(std::string serial, uint16_t from, uint16_t to);
			inline void forwardDevice(std::string serial, uint16_t port) {
				forwardDevice(serial, port, port);
			}

		private:
			std::string adbCmd;
	};
};

#endif
