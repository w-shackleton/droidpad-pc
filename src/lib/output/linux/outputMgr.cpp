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

using namespace std;
using namespace droidpad;

OutputManager::OutputManager(const int type, const int numAxes, const int numButtons) {
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
			throw invalid_argument("Invalid type");
	}
	if(ret < 0) {
		throw runtime_error("Couldn't setup uinput");
	}
}

OutputManager::~OutputManager() {
	dpinput_close(dpinput);
	delete dpinput;
}
