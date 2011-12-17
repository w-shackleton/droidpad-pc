#include "vjoy.hpp"

#include "stdafx.h"

#include "vJoyInstall.h"

#include <windows.h>


wxString vJoyGetDevHwId() {
	TCHAR hwId[MAX_PATH];
	GetDevHwId(0, NULL, hwId);
	return wxString(hwId);
}

// Extern included from lib installer.
FILE* stream;

void vJoyOpenLog(wxString location) {
	stream = fopen(location.mb_str(), "a");
	fprintf(stream, "\n\n");
}

bool vJoyIsInstalled() {
	return isvJoyInstalled();
}

void vJoyCloseLog() {
	fclose(stream);
}

bool vJoyInstall(wxString infPath, wxString hwId) {
	return Installation(hwId.c_str(), infPath.wchar_str()) == 0;
}
bool vJoyRemove(wxString infPath, wxString hwId) {
	return Removal(hwId.wchar_str(), infPath.wchar_str(), false) == 0;
}
bool vJoyPurge(wxString infPath, wxString hwId) {
	return Removal(hwId.wchar_str(), infPath.wchar_str(), true) == 0;
}
bool vJoyRepair(wxString infPath, wxString hwId) {
	return Repair(hwId.wchar_str(), infPath.wchar_str()) == 0;
}
