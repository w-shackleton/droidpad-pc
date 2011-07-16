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
#include "winOutputs.hpp"

// ???
#define _WIN32_WINNT 0x0500
#include <windows.h>

using namespace droidpad::win32;

bool WinOutputs::prevLeft;
bool WinOutputs::prevMiddle;
bool WinOutputs::prevRight;

bool WinOutputs::SendMouseEvent(int dx, int dy, bool left, bool middle, bool right, int scrollDelta)
{
	INPUT event[3];
	ZeroMemory(event, sizeof(event));
	event[0].type = INPUT_MOUSE;
	event[0].mi.dwFlags |= left	? (!prevLeft	? MOUSEEVENTF_LEFTDOWN	: 0) : (prevLeft	? MOUSEEVENTF_LEFTUP	: 0);
	event[0].mi.dwFlags |= middle	? (!prevMiddle	? MOUSEEVENTF_MIDDLEDOWN: 0) : (prevMiddle	? MOUSEEVENTF_MIDDLEUP	: 0);
	event[0].mi.dwFlags |= right	? (!prevRight	? MOUSEEVENTF_RIGHTDOWN	: 0) : (prevRight	? MOUSEEVENTF_RIGHTUP	: 0);

	event[1].type = INPUT_MOUSE;
	int wheelClicks = -scrollDelta / 120;
	event[1].mi.mouseData = wheelClicks * 120;
	event[1].mi.dwFlags = MOUSEEVENTF_WHEEL;

	event[2].type = INPUT_MOUSE;
	event[2].mi.dx = dx;
	event[2].mi.dy = dy;
	event[2].mi.dwFlags = MOUSEEVENTF_MOVE;

	prevLeft = left;
	prevMiddle = middle;
	prevRight = right;
	return SendInput(3, event, sizeof(event[0]));
}
