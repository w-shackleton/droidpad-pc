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

#include "log.hpp"

using namespace droidpad;
using namespace std;

ModeSetting::ModeSetting() :
	initialised(false),
	type(MODE_JS),
	numRawAxes(0),
	numAxes(0),
	numButtons(0) {}

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

wxString DPConnection::GetLine()
{
	while(inData.Find('\n') == wxNOT_FOUND) {
		ParseFromNet();
	}
	wxString ret = inData.Left(inData.Find('\n'));
	inData = inData.Mid(inData.Find('\n') + 1); // Trim old stuff off
	return ret;
}

void DPConnection::ParseFromNet()
{
	Read(buffer, CONN_BUFFER_SIZE);
	inData += wxString(buffer, wxConvUTF8);
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

const DPData DPConnection::GetData() {
	DPData data;

	wxString line = GetLine();
	int start = line.Find(wxT("[")) + 1;
	int end = line.Find(wxT("]"));

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
						long value;
						if(valTk.GetNextToken().ToLong(&value)) {
							data.axes.push_back(value);
						}
					}

					break;
				default: // Must be a raw JS
					start = 1;
					if(end < start) break; // Malformed?
					break;
			}
		}
		else { // Must be a button.
			data.buttons.push_back(t == wxT("1"));
		}
	}

	return data;
}

