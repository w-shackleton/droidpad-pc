#include "connection.hpp"

#include <iostream>

#include <wx/tokenzr.h>

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
	} catch (const char* err) {
		LOGEwx(wxT("Error getting mode info: ") + wxString(err, wxConvUTF8));
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
	return inData.Left(inData.Find('\n'));
}

void DPConnection::ParseFromNet()
{
	Read(buffer, CONN_BUFFER_SIZE);
	inData += wxString(buffer, wxConvUTF8);
}

const ModeSetting &DPConnection::GetMode()
{
	if(mode.initialised) return mode;
	wxString line = GetLine();

	int mp1 = line.Find(wxT("<MODE>")) + 6;
	int mp2 = line.Find(wxT("</MODE>"));
	if(mp1 == wxNOT_FOUND || mp2 == wxNOT_FOUND) throw "Couldn't parse settings - Mode";
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
	if(sp1 == wxNOT_FOUND || sp2 == wxNOT_FOUND) throw "Couldn't parse settings - Specs";
	wxString specs = line.Mid(sp1, sp2 - sp1);

	cout << "+=+++============" << specs.mb_str() << endl;

	wxStringTokenizer tkz(specs, wxT(","));
	if(!tkz.HasMoreTokens()) throw "Couldn't parse settings - Specs are wrong length (0)";
	long numRawAxes;
	tkz.GetNextToken().ToLong(&numRawAxes);
	mode.numRawAxes = numRawAxes;

	if(!tkz.HasMoreTokens()) throw "Couldn't parse settings - Specs are wrong length (1)";
	long numAxes;
	tkz.GetNextToken().ToLong(&numAxes);
	mode.numAxes = numAxes;

	if(!tkz.HasMoreTokens()) throw "Couldn't parse settings - Specs are wrong length (2)";
	long numButtons;
	tkz.GetNextToken().ToLong(&numButtons);
	mode.numButtons = numButtons;

	mode.initialised = true;
	return mode;
}
