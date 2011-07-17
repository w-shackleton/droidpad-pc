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
wxString Data::serial = wxT("__ip");
int Data::ip1 = 0;
int Data::ip2 = 0;
int Data::ip3 = 0;
int Data::ip4 = 0;
int Data::port = 3141;

#define CONF_FILE "dp.conf"

bool Data::initialise()
{
	std::vector<wxString> datadirs;
	wxString testFile = wxT("icon.xpm");

	// TODO: make this code better, to use app directory rather than cwd

	datadirs.push_back(wxString(wxT("") PREFIX "/share/" PACKAGE_NAME));
	datadirs.push_back(wxT("data"));
	datadirs.push_back(wxT("../data"));
	datadirs.push_back(wxT("."));

	//string dataDir;

	for(int i = 0; i < datadirs.size(); i++)
	{
		ifstream tstream((datadirs[i] + wxT("/") + testFile).mb_str());
		if(tstream)
		{
			LOGVwx(wxString::Format(wxT("Found data folder @ \"%s\"."), datadirs[i].c_str()));
			datadir = datadirs[i];
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

			if(key.Cmp(wxT("ip1")) == 0) {
				ip1 = atoi(value.mb_str());
			}
			if(key.Cmp(wxT("ip2")) == 0) {
				ip2 = atoi(value.mb_str());
			}
			if(key.Cmp(wxT("ip3")) == 0) {
				ip3 = atoi(value.mb_str());
			}
			if(key.Cmp(wxT("ip4")) == 0) {
				ip4 = atoi(value.mb_str());
			}
			if(key.Cmp(wxT("port")) == 0) {
				port = atoi(value.mb_str());
			}
			if(key.Cmp(wxT("port")) == 0) {
				port = atoi(value.mb_str());
			}
			if(key.Cmp(wxT("serial")) == 0) {
				serial = value;
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
		config.AddLine(wxString::Format(wxT("%s;%d"), wxT("ip1"), ip1));
		config.AddLine(wxString::Format(wxT("%s;%d"), wxT("ip2"), ip2));
		config.AddLine(wxString::Format(wxT("%s;%d"), wxT("ip3"), ip3));
		config.AddLine(wxString::Format(wxT("%s;%d"), wxT("ip4"), ip4));
		config.AddLine(wxString::Format(wxT("%s;%d"), wxT("port"), port));

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

