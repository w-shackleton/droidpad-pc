#include "jsDriver.hpp"

#include "stdafx.h"

#include "vJoyInstall.h"

#include <windows.h>


wxString jsGetDevHwId() {
	TCHAR hwId[MAX_PATH];
	GetDevHwId(0, NULL, hwId);
	return wxString(hwId);
}

// Extern included from lib installer.
FILE* stream;

void jsInstallOpenLog(wxString location) {
	stream = fopen(location.mb_str(), "a");
	fprintf(stream, "\n\n");
}

bool isJsInstalledAndRunning() {
	return isvJoyInstalled();
}

void jsInstallCloseLog() {
	fclose(stream);
}

bool jsInstall(wxString infPath, wxString hwId) {
	return Installation(hwId.c_str(), infPath.wchar_str()) == 0;
}
bool jsRemove(wxString infPath, wxString hwId) {
	return Removal(hwId.wchar_str(), infPath.wchar_str(), false) == 0;
}
bool jsPurge(wxString infPath, wxString hwId) {
	return Removal(hwId.wchar_str(), infPath.wchar_str(), true) == 0;
}
bool jsRepair(wxString infPath, wxString hwId) {
	return Repair(hwId.wchar_str(), infPath.wchar_str()) == 0;
}
