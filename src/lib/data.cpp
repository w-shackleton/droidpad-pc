#include "data.hpp"

#include <fstream>
#include <vector>

#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>

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
			cout << "Found data folder @ \"" << datadirs[i].c_str() << "\"" << endl;
			datadir = datadirs[i];
		}
	}
	if(datadir == wxT(""))
	{
		cout << "ERROR: Could not find data!" << endl;
		return false;
	}

	confLocation = wxStandardPaths::Get().GetUserDataDir();
	wxMkdir(confLocation);

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
		cout << "Created new config file." << endl;
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
		cout << "ERROR: Couldn't save configuration to file!" << endl;
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

