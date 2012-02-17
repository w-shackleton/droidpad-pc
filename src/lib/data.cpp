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

#include "log.hpp"

using namespace std;
using namespace droidpad;

wxString Data::datadir = wxT("");
wxString Data::confLocation = wxT("");
wxString Data::host = wxT("");

wxString Data::version = wxT(VERSION);

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

	confLocation = wxStandardPaths::Get().GetUserDataDir();
	if(!wxDirExists(confLocation)) wxMkdir(confLocation);

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
			}
		}
		config.Close();
	}
	else
	{
		config.Create();
		LOGV("Created new config file.");
		savePreferences();
	}

	return true;
}

wxString Data::getFilePath(wxString file)
{
	return datadir + wxT("/") + file;
}

void Data::savePreferences()
{
	wxTextFile config(wxString(confLocation.c_str(), wxConvUTF8) + wxT("/") + wxT(CONF_FILE));
	if(config.Open())
	{
		config.Clear();
		config.AddLine(wxString::Format(wxT("%s;%s"), wxT("host"), (const char *)host.c_str()));
		config.AddLine(wxString::Format(wxT("%s;%d"), wxT("port"), port));
		config.AddLine(wxString::Format(wxT("%s;%s"),
					wxT("buttonOrder"), (const wxChar*) encodeOrderConf(buttonOrder, NUM_BUTTONS).c_str()));
		config.AddLine(wxString::Format(wxT("%s;%s"),
					wxT("axisOrder"), (const wxChar*) encodeOrderConf(axisOrder, NUM_AXIS).c_str()));

		config.Write();
	}
	else
		LOGE("ERROR: Couldn't save configuration to file!");
}

wxString stringToUpper(wxString strToConvert)
{
	for(unsigned int i=0;i<strToConvert.length();i++)
	{
		strToConvert[i] = toupper(strToConvert[i]);
	}
	return strToConvert;
}

wxString stringToLower(wxString strToConvert)
{
	for(unsigned int i=0;i<strToConvert.length();i++)
	{
		strToConvert[i] = tolower(strToConvert[i]);
	}
	return strToConvert;
}

vector<int> Data::decodeOrderConf(wxString input, int count) {
}

wxString Data::encodeOrderConf(vector<int> input, int count) {
}
