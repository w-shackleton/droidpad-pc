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

/*
   This code contains the functions used to output code on the windows input API,
   as opposed to the PPJoy API
   */
#ifndef DP_VJOY_OUTPUTS_H
#define DP_VJOY_OUTPUTS_H

#include <stdexcept>
#include <map>

#define UNICODE
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <windows.h>

#include "ext/win32/js/defs.h"

namespace droidpad {
	namespace win32 {
		extern INPUT_DATA neutralInputData;
		class JSOutputs {
			public:
				// Opens the joystick.
				// returns true on success.
				int OpenJoystick();
				virtual ~JSOutputs();

				int SendPositions(INPUT_DATA &data);

			private:
				HANDLE dosDeviceHandle;
		};
	};
};

#endif
