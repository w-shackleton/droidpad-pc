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
#include <cmath>
#include "include/platformSettings.hpp"
#include "hexdump.h"

#include "log.hpp"

using namespace droidpad;
using namespace droidpad::decode;
using namespace std;

ModeSetting::ModeSetting() :
	initialised(false),
	supportsBinary(false),
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

wxString DPConnection::GetLine() throw (runtime_error) {
	size_t returnPosition;
	while((returnPosition = inData.find('\n')) == string::npos) {
		if(!ParseFromNet()) throw runtime_error("Connection closed");
	}
	wxString ret = wxString(inData.substr(0, returnPosition).c_str(), wxConvUTF8);
	inData = inData.substr(returnPosition + 1); // Trim old stuff off
	return ret;
}

/**
 * Returns true if the parse was successful.
 */
bool DPConnection::ParseFromNet() {
	memset(buffer, 0, CONN_BUFFER_SIZE);
	Read(buffer, CONN_BUFFER_SIZE);
	inData.append(buffer, LastCount());
	return !Error();
}

char DPConnection::PeekChar() throw (runtime_error) {
	while(inData.length() < 1) {
		if(!ParseFromNet()) throw runtime_error("Connection closed");
	}
	return inData[0];
}

string DPConnection::GetBytes(size_t n) {
	while(inData.length() < n) {
		if(!ParseFromNet()) throw runtime_error("Connection closed");
	}
	string ret = inData.substr(0, n);
	inData = inData.substr(n); // Trim old stuff off
	return ret;
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

	if(line.Contains(wxT("<SUPPORTSBINARY>")))
		mode.supportsBinary = true;

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
	char first = PeekChar();
	switch(first) {
		case '[': // Indicates text
			return getTextData(GetLine());
		case 'D': { // Binary header begins "DPAD"
			RawBinaryHeader header = getBinaryHeader(GetBytes(sizeof(RawBinaryHeader)).c_str());
			int remainingSize = sizeof(RawBinaryElement) * header.numElements;

			// Iterate through data, parsing each element.
			string elemData = GetBytes(remainingSize);
			const char *start = elemData.c_str();
			vector<RawBinaryElement> elems;
			for(const char *elem = start; elem < start + remainingSize; elem += sizeof(RawBinaryElement)) {
				elems.push_back(getBinaryElement(elem));
			}
			return getBinaryData(header, elems);
			  }
		default:
			LOGW("Unrecognised message recieved from phone");
#ifdef DEBUG
			// cout << first << endl;
			hexdump(GetBytes(sizeof(RawBinaryHeader)).c_str(), sizeof(RawBinaryHeader));
#endif
			  break;
	}
	return DPJSData();
}

void DPConnection::RequestBinary() throw (std::runtime_error) {
	SendMessage("<BINARY>\n");
	LOGV("Binary request sent to server");
}
