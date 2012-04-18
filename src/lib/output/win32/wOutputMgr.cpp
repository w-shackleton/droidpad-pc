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
#include "wOutputMgr.hpp"

#include <sstream>

#include "types.hpp"

#include "net/connection.hpp"
#include "log.hpp"
#include "winOutputs.hpp"

#include <windows.h>

using namespace std;
using namespace droidpad;
using namespace droidpad::decode;
using namespace droidpad::win32;

OutputManager::OutputManager(const int type, const int numAxes, const int numButtons) :
	IOutputManager(type, numAxes, numButtons)
{
	switch(type) {
		case MODE_JS:
			joystick = new VJoyOutputs;
			break;
		default:
			joystick = NULL;
			break;
	}

	if(joystick) {
		int result = joystick->OpenJoystick();
		if(result) { // If fails
			stringstream err;
			int ret = -1;
			switch(result) {
				case ERROR_FILE_NOT_FOUND:
					err << "vJoy device not found / not installed.";
					ret = ERROR_NO_VJOY;
					break;
				default:
					err << "Couldn't open vJoy handle, error is " << result << ".";
					break;
			}
			throw OutputException(ret, err.str());
		}
	}
}

OutputManager::~OutputManager() {
	if(joystick) delete joystick;
}

// Sort of hack.
#define JS_OFFSET 16384

void OutputManager::SendJSData(const DPJSData& data, bool firstIteration) {
	INPUT_DATA pos;
	if(data.axes.size() >= 1) pos.axisX = data.axes[0] + JS_OFFSET;
	if(data.axes.size() >= 2) pos.axisY = data.axes[1] + JS_OFFSET;
	if(data.axes.size() >= 3) pos.axisZ = data.axes[2] + JS_OFFSET;
	if(data.axes.size() >= 4) pos.axisRX = data.axes[3] + JS_OFFSET;
	if(data.axes.size() >= 5) pos.axisRY = data.axes[4] + JS_OFFSET;
	if(data.axes.size() >= 6) pos.axisRZ = data.axes[5] + JS_OFFSET;
	pos.buttons = 0;
	for(int i = 0; i < data.buttons.size(); i++) {
		pos.buttons += data.buttons[i] ? 0x1 << i : 0;
	}
	joystick->SendPositions(pos);
}

void OutputManager::SendMouseData(const DPMouseData& data, bool firstIteration)
{
	if(!WinOutputs::SendMouseEvent(data.x / 400, data.y / 400, data.bLeft, data.bMiddle, data.bRight, firstIteration ? data.incrementalScrollDelta : 0)) // TODO: Customise scale factors?
	{
		LOGWwx(wxT("SendInput failed") + GetLastError());
	}
}

void OutputManager::SendSlideData(const DPSlideData& data, bool firstIteration)
{
	WinOutputs::SendKeystroke(VK_UP,	data.prev);
	WinOutputs::SendKeystroke(VK_DOWN,	data.next);
	WinOutputs::SendKeystroke(VK_F5,	data.start);
	WinOutputs::SendKeystroke(VK_ESCAPE,	data.finish);
	WinOutputs::SendKeystroke('W',		data.white);
	WinOutputs::SendKeystroke('B',		data.black);
	WinOutputs::SendKeystroke(VK_HOME,	data.beginning);
	WinOutputs::SendKeystroke(VK_END,	data.end);
}

