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
#define UNICODE
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <windows.h>

using namespace droidpad::win32;
using namespace std;

bool WinOutputs::prevLeft;
bool WinOutputs::prevMiddle;
bool WinOutputs::prevRight;
map<int, bool> WinOutputs::keyStates;

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

bool WinOutputs::SendAbsMouseEvent(int x, int y, bool left, bool middle, bool right, int scrollDelta)
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
	event[2].mi.dx = x;
	event[2].mi.dy = y;
	event[2].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;

	prevLeft = left;
	prevMiddle = middle;
	prevRight = right;
	return SendInput(3, event, sizeof(event[0]));
}

bool WinOutputs::SendKeystroke(int w32keyCode, bool down)
{
	// Check key hasn't been pressed.
	if(keyStates[w32keyCode] == down) return true; // Don't press if already pressed.
	keyStates[w32keyCode] = down; // Update cache.

	INPUT event[1];
	ZeroMemory(event, sizeof(event));
	event[0].type = INPUT_KEYBOARD;
	event[0].ki.dwFlags |= down ? 0 : KEYEVENTF_KEYUP;
	event[0].ki.wVk = w32keyCode;
	return SendInput(1, event, sizeof(event[0]));
}

