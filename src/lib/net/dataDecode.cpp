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

#include "dataDecode.hpp"

#include <cmath>
#include "include/platformSettings.hpp"

using namespace std;
using namespace droidpad;
using namespace droidpad::decode;

Vec2 accelToAxes(float x, float y, float z) {
	float ax = atan2(x, sqrt(y * y + z * z)) / M_PI * AXIS_CUTOFF_MULTIPLIER;
	if(ax < -AXIS_SIZE) ax = -AXIS_SIZE;
	if(ax > +AXIS_SIZE) ax = +AXIS_SIZE;
	float ay = atan2(y, z) / M_PI * AXIS_CUTOFF_MULTIPLIER;
	if(ay < -AXIS_SIZE) ay = -AXIS_SIZE;
	if(ay > +AXIS_SIZE) ay = +AXIS_SIZE;
	return Vec2(ax, ay);
}

DPMouseData::DPMouseData() :
	x(0),
	y(0),
	scrollDelta(0),
	bLeft(false),
	bMiddle(false),
	bRight(false)
{ }

DPJSData::DPJSData() :
	connectionClosed(false)
{ }

DPJSData::DPJSData(const DPJSData& old) :
	axes(old.axes),
	touchpadAxes(old.touchpadAxes),
	buttons(old.buttons),
	connectionClosed(connectionClosed)
{ }

void DPJSData::reorder(std::vector<int> bmap, std::vector<int> amap) {
	vector<bool> newButtons(buttons.size());
	for(int i = 0; i < buttons.size(); i++) {
		if(i >= bmap.size()) { // Map doesn't do this part
			newButtons[i] = buttons[i];
		} else {
			int destination = bmap[i];
			if(destination >= newButtons.size()) { // Mapped to invalid space
				// Do nothing
			} else {
				newButtons[destination] = buttons[i];
			}
		}
	}

	vector<int> newAxes(axes.size());
	for(int i = 0; i < axes.size(); i++) {
		if(i >= amap.size()) { // Map doesn't do this part
			newAxes[i] = axes[i];
		} else {
			int destination = amap[i];
			if(destination >= newAxes.size()) { // Mapped to invalid space
				// Do nothing
			} else {
				newAxes[destination] = axes[i];
			}
		}
	}
	buttons = newButtons;
	axes = newAxes;
}

DPMouseData::DPMouseData(const DPMouseData& old) :
	x(old.x),
	y(old.y),
	scrollDelta(old.scrollDelta),
	bLeft(old.bLeft),
	bMiddle(old.bMiddle),
	bRight(old.bRight)
{ }

DPMouseData::DPMouseData(const DPJSData& rawData, const DPJSData& prevData) {
	if(rawData.axes.size() < 2) {
		x = 0;
		y = 0;
	} else {
		x = rawData.axes[0];
		y = -rawData.axes[1];
	}
	if(rawData.touchpadAxes.size() == 1 && prevData.touchpadAxes.size() == 1) {
		incrementalScrollDelta = rawData.touchpadAxes[2] / (50 * 120) - prevData.touchpadAxes[2] / (50 * 120); // Scroll is last.
		incrementalScrollDelta *= 120;
		scrollDelta = rawData.touchpadAxes[2] / 50 - prevData.touchpadAxes[2] / 50;
	} else if(rawData.touchpadAxes.size() >= 3 && prevData.touchpadAxes.size() >= 3) {
		x = rawData.touchpadAxes[0] - prevData.touchpadAxes[0];
		y = rawData.touchpadAxes[1] - prevData.touchpadAxes[1];
		x *= 10;
		y *= 10;

		incrementalScrollDelta = rawData.touchpadAxes[2] / (50 * 120) - prevData.touchpadAxes[2] / (50 * 120); // Scroll is last.
		incrementalScrollDelta *= 120;
		scrollDelta = rawData.touchpadAxes[2] / 50 - prevData.touchpadAxes[2] / 50;
	} else {
		scrollDelta = 0;
		incrementalScrollDelta = 0;
	}
	if(rawData.buttons.size() > 0) {
		bLeft = rawData.buttons[0];
		bMiddle = rawData.buttons[1];
		bRight = rawData.buttons[2];
	} else {
		bLeft = false;
		bMiddle = false;
		bRight = false;
	}
}

DPSlideData::DPSlideData() :
	next(false),
	prev(false),
	start(false),
	finish(false),
	white(false),
	black(false),
	beginning(false),
	end(false)
{ }
DPSlideData::DPSlideData(const DPSlideData& old) :
	next(old.next),
	prev(old.prev),
	start(old.start),
	finish(old.finish),
	white(old.white),
	black(old.black),
	beginning(old.beginning),
	end(old.end)
{
}
DPSlideData::DPSlideData(const DPJSData& rawData, const DPJSData& prevData) {
	if(rawData.buttons.size() >= 8) {
		next	= rawData.buttons[0];
		prev	= rawData.buttons[1];
		start	= rawData.buttons[2];
		finish	= rawData.buttons[3];
		white	= rawData.buttons[4] ^
			(prevData.buttons.size() < 8 ? 0 : prevData.buttons[4]); // Toggle, but only want
		black	= rawData.buttons[5] ^
			(prevData.buttons.size() < 8 ? 0 : prevData.buttons[5]); // to be pressed once.
		beginning=rawData.buttons[6];
		end	= rawData.buttons[7];
	}
}

