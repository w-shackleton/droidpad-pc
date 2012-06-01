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
#ifndef DP_WIN_OUTPUTS_H
#define DP_WIN_OUTPUTS_H

#include <stdexcept>
#include <map>

#ifndef MOUSEEVENTF_VIRTUALDESK
#define MOUSEEVENTF_VIRTUALDESK 0x4000
#endif

namespace droidpad {
	namespace win32 {
		class WinOutputs {
			public:
				static bool SendMouseEvent(int dx, int dy, bool left, bool middle, bool right, int scrollDelta);
				// x and y should be in the range [0,65536)
				static bool SendAbsMouseEvent(int x, int y, bool left, bool middle, bool right, int scrollDelta);

				static bool SendKeystroke(int w32keyCode, bool up);
			private:
				static bool prevLeft, prevMiddle, prevRight;

				/*
				   The state of the keys, in win32 keycode format. true is pressed.
				   */
				static std::map<int, bool> keyStates;
		};
	};
};

#endif
