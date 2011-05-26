#include "adb.hpp"

using namespace droidpad;

#include <wx/thread.h>

AdbManager::AdbManager() {
}

bool AdbManager::initialise() {
	wxThread::Sleep(5000);
	return true;
}

AdbManager::~AdbManager() {
}
