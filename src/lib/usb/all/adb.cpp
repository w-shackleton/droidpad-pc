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
#ifdef DEBUG
		if(!Data::noAdb)
#endif
		runProcess(adbCmd, ADB_START);
	} catch (string e) {
		LOGEwx(wxString::Format(wxT("Failed to run ADB: %s."), e.c_str()));
		return false;
	}
	return true;
}

AdbManager::~AdbManager() {
	try {
#ifdef DEBUG
		if(!Data::noAdb)
#endif
		runProcess(adbCmd, ADB_STOP);
	} catch (int e) {
	}
}

vector<wxString> AdbManager::getDeviceIds() {
	vector<wxString> devs;
#ifdef DEBUG
	if(Data::noAdb)
		return devs;
#endif

	wxString ret = wxString(runProcess(adbCmd, ADB_DEVICES).c_str(), wxConvUTF8);

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
	cout << "Forwarding device from " << from << " to " << to << endl;
	stringstream args;
	args << "-s " << serial << " forward " << "tcp:" << from << " tcp:" << to;
#ifdef DEBUG
	if(!Data::noAdb)
#endif
	runProcess(adbCmd, args.str());
}

