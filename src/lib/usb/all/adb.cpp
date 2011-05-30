#include "adb.hpp"

using namespace droidpad;

#include <iostream>
using namespace std;

#include <wx/tokenzr.h>

#include "data.hpp"

#ifdef OS_LINUX
#define ADB_PATH wxT("adb/adb")
#endif
#ifdef OS_WIN32
#define ADB_PATH wxT("adb/adb.exe")
#endif

#define ADB_START "start-server"
#define ADB_STOP "kill-server"
#define ADB_DEVICES "devices"

#include "proc.hpp"

AdbManager::AdbManager() {
	adbCmd = string(Data::getFilePath(ADB_PATH).mb_str());
	cout << "ADB: " << adbCmd << endl;
}

bool AdbManager::initialise() {
	try {
		runProcess(adbCmd, ADB_START);
	} catch (int e) {
		if(e == PROC_FAIL) {
			cout << "Failed to run ADB" << endl;
			return false;
		}
	}
	return true;
}

AdbManager::~AdbManager() {
	try {
		runProcess(adbCmd, ADB_STOP);
	} catch (int e) {
	}
}

vector<wxString> AdbManager::getDeviceIds() {
	wxString ret = wxString(runProcess(adbCmd, ADB_DEVICES).c_str(), wxConvUTF8);

	vector<wxString> devs;

	wxStringTokenizer tkz(ret, wxT("\n"));
	while (tkz.HasMoreTokens())
	{
		wxString line = tkz.GetNextToken();
		if(line.Contains(wxT("List of devices")) || line.IsEmpty()) continue;
		devs.push_back(line.Left(line.Find('\t')));
	}
	return devs;
}
