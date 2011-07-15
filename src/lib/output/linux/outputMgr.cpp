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
#include "outputMgr.hpp"

#include "types.hpp"

#include "net/connection.hpp"

using namespace std;
using namespace droidpad;

OutputManager::OutputManager(const int type, const int numAxes, const int numButtons) :
	IOutputManager(type, numAxes, numButtons)
{
	dpinput = new dpinfo;

	dpinput->axisMin = -AXIS_SIZE;
	dpinput->axisMax = AXIS_SIZE;
	dpinput->buttonNum = numButtons;
	dpinput->axisNum = numAxes;
	int ret;
	switch(type) {
		case MODE_JS:
			ret = dpinput_setup(dpinput,TYPE_JS);
			break;
		case MODE_MOUSE:
			ret = dpinput_setup(dpinput,TYPE_MOUSE);
			break;
		case MODE_SLIDE:
			ret = dpinput_setup(dpinput,TYPE_KEYBD);
			break;
		default:
			delete dpinput;
			throw invalid_argument("Invalid type");
	}
	if(ret < 0) {
		delete dpinput;
		throw runtime_error("Couldn't setup uinput");
	}

	axesBuffer = new int[numAxes];
	axesBufferSize = numAxes;
	buttonBuffer = new int[numButtons];
	buttonBufferSize = numButtons;
}

OutputManager::~OutputManager() {
	dpinput_close(dpinput);
	delete dpinput;
	dpinput = NULL;
	delete axesBuffer;
	delete buttonBuffer;
}

void OutputManager::SendJSData(const DPJSData& data) {
	int i = 0;
	for(vector<int>::const_iterator it = data.axes.begin(); it != data.axes.end(); it++) {
		axesBuffer[i++] = *it;
	}
	i = 0;
	for(vector<bool>::const_iterator it = data.buttons.begin(); it != data.buttons.end(); it++) {
		buttonBuffer[i++] = *it;
	}

	dpinput_sendNPos(dpinput, axesBuffer, axesBufferSize);
	dpinput_sendButtons(dpinput, buttonBuffer, buttonBufferSize);
}

void OutputManager::SendMouseData(const DPMouseData& data)
{
	dpinput_send2Pos(dpinput, data.x / 400, -data.y / 400); // TODO: Customise?
	dpinput_sendPos(dpinput, REL_WHEEL, data.scrollDelta); // TODO: Check how this will work.
	dpinput_sendButton(dpinput, BTN_LEFT, data.bLeft);
	dpinput_sendButton(dpinput, BTN_MIDDLE, data.bMiddle);
	dpinput_sendButton(dpinput, BTN_RIGHT, data.bRight);
}

void OutputManager::SendSlideData(const DPSlideData& data)
{
	dpinput_sendButton(dpinput,	KEY_UP,		data.prev);
	dpinput_sendButton(dpinput,	KEY_DOWN,	data.next);
	dpinput_sendButton(dpinput,	KEY_F5,		data.start);
	dpinput_sendButton(dpinput,	KEY_ESC,	data.finish);
	dpinput_sendButton(dpinput,	KEY_W,		data.white);
	dpinput_sendButton(dpinput,	KEY_B,		data.black);
	dpinput_sendButton(dpinput,	KEY_HOME,	data.beginning);
	dpinput_sendButton(dpinput,	KEY_END,	data.end);
}

