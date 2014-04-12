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
#include "data.hpp"

#include "net/connection.hpp"

#include <iostream>

using namespace std;
using namespace droidpad;
using namespace droidpad::decode;

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
		case MODE_ABSMOUSE:
			ret = dpinput_setup(dpinput,TYPE_TOUCHSCREEN);
			break;
		default:
			delete dpinput;
			throw invalid_argument("Invalid type");
	}
	if(ret < 0) {
		delete dpinput;
		throw OutputException(ERROR_NO_UINPUT, "Couldn't setup uinput");
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

void OutputManager::SendJSData(const DPJSData& data, bool firstIteration) {
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

void OutputManager::SendMouseData(const DPMouseData& data, bool firstIteration) {
	dpinput_send2Pos(dpinput, data.x / 400, -data.y / 400); // TODO: Customise?
	dpinput_sendPos(dpinput, REL_WHEEL, firstIteration ? (data.incrementalScrollDelta / 120) : 0);
	dpinput_sendButton(dpinput, BTN_LEFT, data.bLeft);
	dpinput_sendButton(dpinput, BTN_MIDDLE, data.bMiddle);
	dpinput_sendButton(dpinput, BTN_RIGHT, data.bRight);
}

void OutputManager::SendTouchData(const decode::DPTouchData& data, bool firstIteration) {
	dpinput_send2Pos(dpinput, data.x, -data.y);
	dpinput_sendPos(dpinput, ABS_WHEEL, firstIteration ? (data.incrementalScrollDelta / 120) : 0);
	dpinput_sendButton(dpinput, BTN_LEFT, data.bLeft);
	dpinput_sendButton(dpinput, BTN_MIDDLE, data.bMiddle);
	dpinput_sendButton(dpinput, BTN_RIGHT, data.bRight);
}

void OutputManager::SendSlideData(const DPSlideData& data, bool firstIteration)
{
	dpinput_sendButton(dpinput,	KEY_UP,		data.prev);
	dpinput_sendButton(dpinput,	KEY_DOWN,	data.next);
	dpinput_sendButton(dpinput,	KEY_F5,		data.start);
	dpinput_sendButton(dpinput,	KEY_ESC,	data.finish);
	dpinput_sendButton(dpinput,	getKeycode(Data::whiteKey), data.white);
	dpinput_sendButton(dpinput,	getKeycode(Data::blackKey), data.black);
	dpinput_sendButton(dpinput,	KEY_HOME,	data.beginning);
	dpinput_sendButton(dpinput,	KEY_END,	data.end);
}


int OutputManager::getKeycode(char letter) {
	char lower = tolower(letter);
	switch(lower) {
		case 'a': return KEY_A;
		case 'b': return KEY_B;
		case 'c': return KEY_C;
		case 'd': return KEY_D;
		case 'e': return KEY_E;
		case 'f': return KEY_F;
		case 'g': return KEY_G;
		case 'h': return KEY_H;
		case 'i': return KEY_I;
		case 'j': return KEY_J;
		case 'k': return KEY_K;
		case 'l': return KEY_L;
		case 'm': return KEY_M;
		case 'n': return KEY_N;
		case 'o': return KEY_O;
		case 'p': return KEY_P;
		case 'q': return KEY_Q;
		case 'r': return KEY_R;
		case 's': return KEY_S;
		case 't': return KEY_T;
		case 'u': return KEY_U;
		case 'v': return KEY_V;
		case 'w': return KEY_W;
		case 'x': return KEY_X;
		case 'y': return KEY_Y;
		case 'z': return KEY_Z;
		case '0': return KEY_0;
		case '1': return KEY_1;
		case '2': return KEY_2;
		case '3': return KEY_3;
		case '4': return KEY_4;
		case '5': return KEY_5;
		case '6': return KEY_6;
		case '7': return KEY_7;
		case '8': return KEY_8;
		case '9': return KEY_9;
	}
	return KEY_B;
}
