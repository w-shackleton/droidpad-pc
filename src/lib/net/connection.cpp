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
#include "connection.hpp"

#include <wx/tokenzr.h>

#include <iostream>
#include <sstream>
#include <cmath>
#include "include/platformSettings.hpp"

#include "log.hpp"

using namespace droidpad;
using namespace std;

ModeSetting::ModeSetting() :
	initialised(false),
	type(MODE_JS),
	numRawAxes(0),
	numAxes(0),
	numButtons(0) {}

DPMouseData::DPMouseData() :
	x(0),
	y(0),
	scrollDelta(0),
	bLeft(false),
	bMiddle(false),
	bRight(false)
{ }

DPJSData::DPJSData()
{ }

DPJSData::DPJSData(const DPJSData& old) :
	axes(old.axes),
	touchpadAxes(old.touchpadAxes),
	buttons(old.buttons)
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

DPConnection::DPConnection(wxString host, uint16_t port) :
	wxSocketClient(wxSOCKET_BLOCK)
{
	addr.Hostname(host);
	addr.Service(port);
	
	SetTimeout(10);
}

DPConnection::~DPConnection() {
	SendMessage("<STOP>\n");
	LOGV("Sent Stop message to server");
	Close();
}

bool DPConnection::Start()
{
	if(!Connect(addr, true)) return false;
	try {
		GetMode();
	} catch (runtime_error err) {
		LOGEwx(wxT("Error getting mode info: ") + wxString(err.what(), wxConvUTF8));
		return false;
	}
	return true;
}

void DPConnection::SendMessage(string message) {
	Write(message.c_str(), message.length());
}

wxString DPConnection::GetLine() throw (runtime_error)
{
	while(inData.Find('\n') == wxNOT_FOUND) {
		if(!ParseFromNet()) throw runtime_error("Connection closed");
	}
	wxString ret = inData.Left(inData.Find('\n'));
	inData = inData.Mid(inData.Find('\n') + 1); // Trim old stuff off
	return ret;
}

/**
 * Returns true if the parse was successful.
 */
bool DPConnection::ParseFromNet()
{
	memset(buffer, 0, CONN_BUFFER_SIZE);
	Read(buffer, CONN_BUFFER_SIZE);
	inData += wxString(buffer, wxConvUTF8);
	return !Error();
}

const ModeSetting &DPConnection::GetMode() throw (runtime_error)
{
	if(mode.initialised) return mode;
	wxString line = GetLine();
	cout << "Settings line: " << line.mb_str() << endl;

	int mp1 = line.Find(wxT("<MODE>")) + 6;
	int mp2 = line.Find(wxT("</MODE>"));
	if(mp1 == wxNOT_FOUND || mp2 == wxNOT_FOUND) throw runtime_error("Couldn't parse settings - Mode");
	wxString dpMode = line.Mid(mp1, mp2 - mp1);

	if(dpMode.StartsWith(wxT("mouse"))) {
		mode.type = MODE_MOUSE;
	} else if(dpMode.StartsWith(wxT("slide"))) {
		mode.type = MODE_SLIDE;
	} else {
		mode.type = MODE_JS;
	}

	int sp1 = line.Find(wxT("<MODESPEC>")) + 10;
	int sp2 = line.Find(wxT("</MODESPEC>"));
	if(sp1 == wxNOT_FOUND || sp2 == wxNOT_FOUND) throw runtime_error("Couldn't parse settings - Specs");
	wxString specs = line.Mid(sp1, sp2 - sp1);

	wxStringTokenizer tkz(specs, wxT(","));
	if(!tkz.HasMoreTokens()) throw runtime_error("Couldn't parse settings - Specs are wrong length (0)");
	long numRawAxes;
	tkz.GetNextToken().ToLong(&numRawAxes);
	mode.numRawAxes = numRawAxes;

	if(!tkz.HasMoreTokens()) throw runtime_error("Couldn't parse settings - Specs are wrong length (1)");
	long numAxes;
	tkz.GetNextToken().ToLong(&numAxes);
	mode.numAxes = numAxes;

	if(!tkz.HasMoreTokens()) throw runtime_error("Couldn't parse settings - Specs are wrong length (2)");
	long numButtons;
	tkz.GetNextToken().ToLong(&numButtons);
	mode.numButtons = numButtons;

	mode.initialised = true;
	return mode;
}

/*
AXES LAYOUTS
  *   *
   *  *
\---*-*-\
 \   **  \
**\*******\** <-X
   \  **   \
    \-*-*---\ <- Phone
      *  *
      *   *
      ^   ^
      Z   Y
*/
const DPJSData DPConnection::GetData() throw (runtime_error)
{
	DPJSData data;

	wxString line = GetLine();
	int start = line.Find(wxT("[")) + 1;
	int end = line.Find(wxT("]"));

	locale cLocale("C");

	wxStringTokenizer tk(line(start, end - start), wxT(";"));
	while(tk.HasMoreTokens()) {
		wxString t = tk.GetNextToken();
		start = 2;
		end = t.Find(wxT("}"));
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

					while(valTk.HasMoreTokens()) {
						{
							int value;
							istringstream inNum(string(valTk.GetNextToken().mb_str()));
							inNum.imbue(cLocale);
							inNum >> value;
							data.touchpadAxes.push_back(value);
						}
					}

					break;
				default: // Must be a raw JS
					start = 1;
					if(end < start) break; // Malformed?
					valTk = wxStringTokenizer(t(start, end - start), wxT(","));
					if(valTk.CountTokens() == 3) { // Else something wrong / malformed
						int i = 0;
						float x, y, z, ax, ay;
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
						ax = atan2(x, sqrt(y * y + z * z)) / M_PI * AXIS_CUTOFF_MULTIPLIER;
						if(ax < -AXIS_SIZE) ax = -AXIS_SIZE;
						if(ax > +AXIS_SIZE) ax = +AXIS_SIZE;
						ay = atan2(y, z) / M_PI * AXIS_CUTOFF_MULTIPLIER;
						if(ay < -AXIS_SIZE) ay = -AXIS_SIZE;
						if(ay > +AXIS_SIZE) ay = +AXIS_SIZE;

						data.axes.push_back(ax);
						data.axes.push_back(ay);
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

