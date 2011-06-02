#include "adb.hpp"

using namespace droidpad;

#include <sstream>
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
#include "log.hpp"

AdbManager::AdbManager() {
	adbCmd = string(Data::getFilePath(ADB_PATH).mb_str());
	LOGVwx(wxString(("ADB: " + adbCmd).c_str(), wxConvUTF8));
}

bool AdbManager::initialise() {
	try {
		runProcess(adbCmd, ADB_START);
	} catch (string e) {
		LOGEwx(wxString::Format(wxT("Failed to run ADB: %s."), e.c_str()));
		return false;
	}
	return true;
}

AdbManager::~AdbManager() {
#ifndef DEBUG // Don't stop ADB in debug - it loads faster next time.
	try {
		runProcess(adbCmd, ADB_STOP);
	} catch (int e) {
	}
#endif
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

void AdbManager::forwardDevice(string serial, uint16_t from, uint16_t to)
{
	stringstream args;
	args << "-s " << serial << " forward " << "tcp:" << from << " tcp:" << to;
	runProcess(adbCmd, args.str());
}

