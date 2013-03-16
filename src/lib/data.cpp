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
#include "data.hpp"

#include <fstream>
#include <vector>

#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>

#include "ext/b64/base64.hpp"

#include "log.hpp"

using namespace std;
using namespace droidpad;

wxString Data::datadir = wxT("");
wxString Data::confLocation = wxT("");
wxString Data::host = wxT("");
wxString Data::computerName = wxT("");

wxConfig *Data::config = NULL;

Tweaks Data::tweaks = Tweaks();

wxString Data::version = wxT(VERSION);

// This class now uses wxConfig. Any other code is simply for compatibility

// REMEMBER: Update these if more buttons / axes added in future
const int initialButtons[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
vector<int> Data::buttonOrder = vector<int>(initialButtons, initialButtons + 12);
const int initialAxes[] = {0, 1, 2, 3, 4, 5};
vector<int> Data::axisOrder = vector<int>(initialAxes, initialAxes + 6);

int Data::port = 3141;

#define CONF_FILE "dp.conf"

bool Data::initialise()
{
	std::vector<wxString> datadirs;
	wxString testFile = wxT("layout.xrc");

	// TODO: make this code better, to use app directory rather than cwd

	datadirs.push_back(wxString(wxT("") PREFIX "/share/" PACKAGE_NAME));
	datadirs.push_back(wxStandardPaths::Get().GetPluginsDir() + wxT("\\data"));
	datadirs.push_back(wxT("data"));
	datadirs.push_back(wxT("../data"));
	datadirs.push_back(wxT("."));

	//string dataDir;

	for(int i = 0; i < datadirs.size(); i++)
	{
		LOGVwx(wxString(wxT("Trying folder ")) + datadirs[i]);
		ifstream tstream((datadirs[i] + wxT("/") + testFile).mb_str());
		if(tstream)
		{
			LOGVwx(wxString::Format(wxT("Found data folder @ \"%s\"."), datadirs[i].c_str()));
			datadir = datadirs[i];
			break;
		}
	}
	if(datadir == wxT(""))
	{
		LOGE("ERROR: Could not find data!");
		return false;
	}

	// Initialise to default first
	tweaks = createDefaultTweaks();

	// Attempt to open new wxConfig format
	config = new wxConfig(wxT("droidpad"), wxT("digitalsquid"));
	wxString tmp;
	if(config->Read(wxT("initialised"), &tmp)) {
		// Read from wxConfig
		LOGV("Reading new preferences format");
		loadPreferences();
		return true;
	}
	LOGV("Reading old preferences format and converting");
	// Else, continue to read from old format

	confLocation = wxStandardPaths::Get().GetUserDataDir();

	wxTextFile config(wxString(confLocation.c_str(), wxConvUTF8) + wxT("/") + wxT(CONF_FILE));

	if(config.Open())
	{ // Process config file
		for(wxString line = config.GetFirstLine(); !config.Eof(); line = config.GetNextLine())
		{
			wxStringTokenizer tkz(line, wxT(";"));
			if(!tkz.HasMoreTokens()) continue; // Malformed line
			wxString key = tkz.GetNextToken();

			if(!tkz.HasMoreTokens()) continue; // Malformed line
			wxString value = tkz.GetNextToken();

			if(key.Cmp(wxT("host")) == 0) {
				host = value;
			} else if(key.Cmp(wxT("port")) == 0) {
				port = atoi(value.mb_str());
			} else if(key.Cmp(wxT("buttonOrder")) == 0) {
				buttonOrder = decodeOrderConf(value, NUM_BUTTONS);
			} else if(key.Cmp(wxT("axisOrder")) == 0) {
				axisOrder = decodeOrderConf(value, NUM_AXIS);
			} else if(key.Cmp(wxT("tweaks")) == 0) {
				string buf = base64_decode((string)value.mb_str());
				if(buf.size() == sizeof(Tweaks))
					memcpy(&tweaks, buf.c_str(), sizeof(Tweaks));
				else LOGV("Couldn't decode tweaks from config");
			}
		}
		config.Close();

		// Remove old file
		wxRemoveFile(wxString(confLocation.c_str(), wxConvUTF8) + wxT("/") + wxT(CONF_FILE));
		wxRmdir(confLocation);

		// Now, save to new system
		savePreferences();
	} else { // Old prefs not found. Neither exist, so create new.
		savePreferences();
	}

	return true;
}

void Data::loadPreferences() {
	// Host
	host = config->Read(wxT("host"), wxEmptyString);
	// Port
	port = config->Read(wxT("host"), 3141);
	// buttonOrder
	wxString buttonOrderText, axisOrderText, tweaksText;
	if(config->Read(wxT("buttonOrder"), &buttonOrderText))
		buttonOrder = decodeOrderConf(buttonOrderText, NUM_BUTTONS);
	// axisOrder
	if(config->Read(wxT("axisOrder"), &axisOrderText))
		axisOrder = decodeOrderConf(axisOrderText, NUM_AXIS);
	// tweaks
	if(config->Read(wxT("tweaks"), &tweaksText)) {
		string buf = base64_decode((string)tweaksText.mb_str());
		if(buf.size() == sizeof(Tweaks))
			memcpy(&tweaks, buf.c_str(), sizeof(Tweaks));
		else LOGV("Couldn't decode tweaks from config");
	}
}

void Data::savePreferences() {
	config->Write(wxT("initialised"), true);
	config->Write(wxT("host"), host);
	config->Write(wxT("port"), port);
	config->Write(wxT("buttonOrder"), encodeOrderConf(buttonOrder, NUM_BUTTONS));
	config->Write(wxT("axisOrder"), encodeOrderConf(axisOrder, NUM_AXIS));

	// Currently serialising tweaks the very non-portable way. Should probably change this
	char *buf = new char[sizeof(Tweaks)];
	memcpy(buf, &tweaks, sizeof(Tweaks));
	string tweakString = base64_encode((unsigned char* const)buf, sizeof(Tweaks));
	delete[] buf;
	config->Write(wxT("tweaks"), STD_TO_WX_STRING(tweakString));
	config->Flush();
}

wxString Data::getFilePath(wxString file)
{
	return datadir + wxT("/") + file;
}

vector<int> Data::decodeOrderConf(wxString input, int count) {
	vector<int> ret;

	wxStringTokenizer tkz(input, wxT(","));
	for(int i = 0; i < count; i++) {
		if(tkz.HasMoreTokens()) {
			long num;
			if(!tkz.GetNextToken().ToLong(&num)) { // If fail
				num = i;
			}
			ret.push_back(num);
		} else {
			ret.push_back(i);
		}
	}
	return ret;
}

wxString Data::encodeOrderConf(vector<int> input, int count) {
	wxString ret;
	for(int i = 0; i < count; i++) {
		if(i < input.size()) {
			ret += wxString::Format(wxT("%d,"), input[i]);
		} else { // Write default out
			ret += wxString::Format(wxT("%d,"), i);
		}
	}
	return ret;
}

Tweaks Data::createDefaultTweaks() {
	Tweaks ret;
	memset(&ret, 0, sizeof(Tweaks));
	for(int i = 0; i < 2; i++) {
		ret.tilt[i].totalAngle = 120;
		ret.tilt[i].gamma = 0;
	}

	ret.rotation[0].totalAngle = 90;
	
	for(int i = 0; i < NUM_AXIS; i++) {
		ret.onScreen[i].gamma = 0;
	}
	return ret;
}
