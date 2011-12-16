#ifdef UNICODE
#warning "UNICODE HERE!"
#endif

#include "vjoy.hpp"

#undef UNICODE
#undef _UNICODE

#ifdef UNICODE
#warning "UNICODE HERE!"
#endif

#include "stdafx.h"

#ifdef UNICODE
#warning "UNICODE HERE!"
#endif

#include "vJoyInstall.h"

#ifdef UNICODE
#warning "UNICODE HERE!"
#endif

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
