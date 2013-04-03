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
#include "log.hpp"
#include <iostream>
#include <sstream>

#include "data.hpp"
#include "mathUtil.hpp"

#include <wx/tokenzr.h>

#ifdef OS_LINUX
#include <arpa/inet.h>
#elif OS_WIN32
#include <winsock2.h>
#endif

// The size of the angle each way from the center of the screen which the phone can be moved in.
#define POINTING_ANGLE_RANGE (M_PI / 4)

#define GAMMA_CONST 6
#define GAMMA_RANGE ((float)100)

using namespace std;
using namespace droidpad;
using namespace droidpad::decode;

Vec2 droidpad::decode::accelToAxes(float x, float y, float z) {
	// Here, we multiply each axis by a constant determined by the user.
	// This effectively sets the range - the constant = 360 / (user range)
	if(Data::tweaks.tilt[0].totalAngle == 0)
		Data::tweaks.tilt[0].totalAngle = 120;
	if(Data::tweaks.tilt[1].totalAngle == 0)
		Data::tweaks.tilt[1].totalAngle = 120;

	float rangex = (float)360 / (float)Data::tweaks.tilt[0].totalAngle;
	float ax = atan2(x, sqrt(y * y + z * z)) / M_PI;
	ax *= rangex;
	trim(ax, -1, 1);
	ax = applyGamma(ax, (float)-Data::tweaks.tilt[0].gamma / GAMMA_RANGE);

	float rangey = (float)360 / (float)Data::tweaks.tilt[1].totalAngle;
	float ay = atan2(y, z) / M_PI;
	ay *= rangey;
	trim(ay, -1, 1);
	ay = applyGamma(ay, (float)-Data::tweaks.tilt[1].gamma / GAMMA_RANGE);

	return Vec2(-ax * AXIS_SIZE, -ay * AXIS_SIZE);
}

float droidpad::decode::applyGamma(float value, float gamma) {
	// Power applied to value must be in the range 1/n to n,
	// where n is around 10.
	float G = pow(GAMMA_CONST, gamma);
	return sign(value) * pow(abs(value), G);
}

DPJSData::DPJSData() :
	connectionClosed(false),
	reset(false)
{ }

DPJSData::DPJSData(const DPJSData& old) :
	axes(old.axes),
	touchpadAxes(old.touchpadAxes),
	buttons(old.buttons),
	connectionClosed(connectionClosed),
	containsAccel(containsAccel),
	containsGyro(containsGyro),
	reset(reset)
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

DPMouseData::DPMouseData() :
	x(0),
	y(0),
	scrollDelta(0),
	bLeft(false),
	bMiddle(false),
	bRight(false)
{ }

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
		incrementalScrollDelta = rawData.touchpadAxes[0] / (50 * 120) - prevData.touchpadAxes[0] / (50 * 120); // Scroll is last.
		incrementalScrollDelta *= 120;
		scrollDelta = rawData.touchpadAxes[0] / 50 - prevData.touchpadAxes[0] / 50;
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
	if(rawData.buttons.size() >= 3) {
		bLeft = rawData.buttons[0];
		bMiddle = rawData.buttons[1];
		bRight = rawData.buttons[2];
	} else {
		bLeft = false;
		bMiddle = false;
		bRight = false;
	}
}

DPTouchData::DPTouchData() :
	scrollDelta(0),
	bLeft(false),
	bMiddle(false),
	bRight(false)
{ }

DPTouchData::DPTouchData(const DPTouchData& old) :
	Vec2(old),
	scrollDelta(old.scrollDelta),
	incrementalScrollDelta(old.incrementalScrollDelta),
	bLeft(old.bLeft),
	bMiddle(old.bMiddle),
	bRight(old.bRight),
	xOffset(old.xOffset),
	yOffset(old.yOffset)
{ }

DPTouchData::DPTouchData(const DPJSData& rawData, const DPJSData& prevData, const DPTouchData& prevAbsData) {
	if(rawData.containsAccel && rawData.containsGyro) {
		x = rawData.axes[2]; // Gyro
		if(rawData.reset)
			xOffset = x;
		y = -rawData.axes[1];
	} else if(rawData.containsAccel) {
		x = rawData.axes[0];
		y = -rawData.axes[1];
	} else if(rawData.containsGyro) {
		x = rawData.axes[0];
		if(rawData.reset)
			xOffset = x;
	}

	x -= xOffset;
	// y -= yOffset;

	if(rawData.touchpadAxes.size() == 1 && prevData.touchpadAxes.size() == 1) {
		incrementalScrollDelta = rawData.touchpadAxes[0] / (50 * 120) - prevData.touchpadAxes[0] / (50 * 120);
		incrementalScrollDelta *= 120;
		scrollDelta = rawData.touchpadAxes[0] / 50 - prevData.touchpadAxes[0] / 50;
	}

	if(rawData.buttons.size() >= 3) {
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

const DPJSData droidpad::decode::getTextData(wxString line) {
	DPJSData data;

	if(line.find(wxT("<STOP>")) != wxNOT_FOUND) {
		data.connectionClosed = true;
		return data;
	} else data.connectionClosed = false;

	int start = line.Find(wxT("[")) + 1;
	int end = line.Find(wxT("]"));

	locale cLocale("C");

	wxStringTokenizer tk(line(start, end - start), wxT(";"));
	while(tk.HasMoreTokens()) {
		wxString t = tk.GetNextToken();
		start = 2;
		end = t.Find(wxT("}"));
		int pos;
		wxStringTokenizer valTk;
		if(t.StartsWith(wxT("{"))) { // Axis of some sort

			switch(t[1]) {
				case 'A': // 2way axis
				case 'S': // 1way axis
					if(end < start) break; // Malformed?
					// cout << t(start, end - start).mb_str() << endl;
					valTk = wxStringTokenizer(t(start, end - start), wxT(","));

					while(valTk.HasMoreTokens()) {
						{
							int value;
							istringstream inNum(string(valTk.GetNextToken().mb_str()));
							inNum.imbue(cLocale);
							inNum >> value;
							data.axes.push_back(value);
						}
					}

					break;
				case 'C': // 1way touchpad
				case 'T': // 2way touchpad
					if(end < start) break; // Malformed?
					// cout << t(start, end - start).mb_str() << endl;
					valTk = wxStringTokenizer(t(start, end - start), wxT(","));

					pos = 0;
					while(valTk.HasMoreTokens()) {
						{
							int value;
							istringstream inNum(string(valTk.GetNextToken().mb_str()));
							inNum.imbue(cLocale);
							inNum >> value;
							if(pos == 1) // 'y' axis on 2way pad
								value = -value;
							data.touchpadAxes.push_back(value);
						}
						pos++;
					}

					break;
				default: // Must be a raw JS
					start = 1;
					if(end < start) break; // Malformed?
					valTk = wxStringTokenizer(t(start, end - start), wxT(","));
					if(valTk.CountTokens() == 3) { // Else something wrong / malformed
						int i = 0;
						float x, y, z;
						while(valTk.HasMoreTokens()) {
							{
								float value;
								istringstream inNum(string(valTk.GetNextToken().mb_str()));
								inNum.imbue(cLocale);
								switch(i) {
									case 0: inNum >> x; break;
									case 1: inNum >> y; break;
									case 2: inNum >> z; break;
								}
								i++;
							}
						}
						Vec2 a = accelToAxes(x, y, z);

						data.axes.push_back(a.x);
						data.axes.push_back(a.y);
						data.containsAccel = true;
					}
					break;
			}
		}
		else { // Must be a button.
			data.buttons.push_back(t == wxT("1"));
		}
	}

	return data;
}

const BinaryConnectionInfo droidpad::decode::getBinaryConnectionInfo(const char *binaryInfo) {
	BinaryConnectionInfo info;
	memcpy(&info, binaryInfo, sizeof(BinaryConnectionInfo));
	NTOH(info.modeType);
	NTOH(info.rawDevices);
	NTOH(info.axes);
	NTOH(info.buttons);
	return info;
}

const RawBinaryHeader droidpad::decode::getBinaryHeader(const char *binaryHeader) {
	RawBinaryHeader header;
	memcpy(&header, binaryHeader, sizeof(RawBinaryHeader));
	
	// Flip round various endianness issues
	NTOH(header.numElements);
	NTOH(header.flags);

	NTOH(header.raw.ax);
	NTOH(header.raw.ay);
	NTOH(header.raw.az);
	NTOH(header.raw.gx);
	NTOH(header.raw.gy);
	NTOH(header.raw.gz);
	NTOH(header.raw.gzn);
	NTOH(header.raw.rx);
	NTOH(header.raw.ry);
	NTOH(header.raw.rz);

#if 0
#ifdef DEBUG
	cout << "Binary header:" << endl;
	cout << header.numElements << " elements, flags = " << header.flags << endl;
	cout << "Accel: " << header.axis.ax << ", " << header.axis.ay << "," << header.axis.az << endl;
#endif
#endif
	return header;
}

const RawBinaryElement droidpad::decode::getBinaryElement(const char *binaryElement) {
	RawBinaryElement elem;
	memcpy(&elem, binaryElement, sizeof(RawBinaryElement));

	NTOH(elem.flags);
	NTOH(elem.raw.data1);
	NTOH(elem.raw.data2);
	NTOH(elem.raw.data3);

	return elem;
}

const DPJSData droidpad::decode::getBinaryData(const RawBinaryHeader header, std::vector<RawBinaryElement> elems) {
	DPJSData ret;
	ret.connectionClosed = header.flags & HEADER_FLAG_STOP;
	// TODO: Add support for gyro when modes are implemented
	if(header.flags & HEADER_FLAG_HAS_ACCEL) {
		Vec2 accel = accelToAxes(header.axis.ax, header.axis.ay, header.axis.az);
		ret.axes.push_back(accel.x);
		ret.axes.push_back(accel.y);
		ret.containsAccel = true;
	}
	// Gyro but no accel
	if((header.flags & HEADER_FLAG_HAS_GYRO) && !(header.flags & HEADER_FLAG_HAS_ACCEL)) {
		// Range on each side of the centre
		float pointingAngleRange = (float)Data::tweaks.rotation[0].totalAngle * DEG_TO_RAD / 2;
		ret.axes.push_back(header.axis.gz / pointingAngleRange * AXIS_SIZE); // Put z-component
		ret.containsGyro = true;
	}
	// Both - use the gyro which was normalised with the accelerometer
	if((header.flags & HEADER_FLAG_HAS_GYRO) && (header.flags & HEADER_FLAG_HAS_ACCEL)) {
		// Range on each side of the centre
		float pointingAngleRange = (float)Data::tweaks.rotation[0].totalAngle * DEG_TO_RAD / 2;
		ret.axes.push_back(header.axis.gzn / pointingAngleRange * AXIS_SIZE);
		ret.containsGyro = true;
		ret.containsAccel = true;
	}

	for(vector<RawBinaryElement>::iterator it = elems.begin(); it != elems.end(); it++) {
		if(it->flags & ITEM_FLAG_BUTTON) {
			ret.buttons.push_back(it->raw.data1);
		}
		if(it->flags & ITEM_FLAG_SLIDER) {
			if(it->flags & ITEM_FLAG_HAS_X_AXIS) {
				// Rearrange axis between -1 and 1
				float num = (float)it->integer.data1 / 16384;
				int pos = ret.axes.size() - 1;
				num = applyGamma(num, (float)-Data::tweaks.onScreen[pos].gamma / GAMMA_RANGE);
				num *= AXIS_SIZE;
				ret.axes.push_back(num);
			}
			if(it->flags & ITEM_FLAG_HAS_Y_AXIS) {
				// Rearrange axis between -1 and 1
				float num = (float)it->integer.data2 / 16384;
				int pos = ret.axes.size() - 1;
				num = applyGamma(num, (float)-Data::tweaks.onScreen[pos].gamma / GAMMA_RANGE);
				num *= AXIS_SIZE;
				ret.axes.push_back(num);
			}
		}
		if(it->flags & ITEM_FLAG_TRACKPAD) {
			if(it->flags & ITEM_FLAG_HAS_X_AXIS)
				ret.touchpadAxes.push_back(it->integer.data1);
			if(it->flags & ITEM_FLAG_HAS_Y_AXIS)
				ret.touchpadAxes.push_back(-it->integer.data2);
		}
		if(it->flags & ITEM_FLAG_BUTTON && it->flags & ITEM_FLAG_IS_RESET && it->integer.data1) {
			LOGV("Reset pressed");
			ret.reset = true;
		}
	}
	return ret;
}
