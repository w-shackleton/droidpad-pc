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

#ifndef BOOT_CONF_H
#define BOOT_CONF_H

#include <string>

// This class manages and calls bcdedit.exe on 64-bit systems to check and enable necessary modes.

namespace droidpad {
	namespace bootconf {
		struct BootConfig {
			/**
			 * If true, data was read successfully.
			 */
			bool success;
			bool testsigning;
		};

		/**
		 * Gets the current config items by calling bcdedit.exe and looking at the result.
		 */
		BootConfig getCurrentConfig();
	}
}

#endif
