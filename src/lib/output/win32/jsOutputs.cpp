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
#include "jsOutputs.hpp"

#include "log.hpp"
#include "wPlatformSettings.hpp"
#include <wx/string.h>

#include <winioctl.h>

using namespace droidpad::win32;

INPUT_DATA droidpad::win32::neutralInputData = {
	JS_OFFSET,
	JS_OFFSET,
	JS_OFFSET,
	JS_OFFSET,
	JS_OFFSET,
	JS_OFFSET,
	0
};

int JSOutputs::OpenJoystick() {
	dosDeviceHandle = CreateFile(TEXT(DOS_FILE_NAME), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(dosDeviceHandle == INVALID_HANDLE_VALUE) {
		return GetLastError();
	}
	return 0;
}

JSOutputs::~JSOutputs() {
	SendPositions(neutralInputData);
	CloseHandle(dosDeviceHandle);
}

int JSOutputs::SendPositions(INPUT_DATA &data) {
	unsigned int ioCode = IOCTL_DP_SEND_INPUT_DATA;
	unsigned int ioSize = sizeof(INPUT_DATA);

	LOGVwx(wxString::Format(wxT("(%d, %d)"), data.axisX, data.axisY));

	DWORD bytesReturned;

	if(!DeviceIoControl(dosDeviceHandle, ioCode, &data, ioSize, NULL, 0, &bytesReturned, NULL)) {
		int error = GetLastError();
		LOGWwx(wxString::Format(wxT("Couldn't send JS data, error is %d."), error));
		return error;
	}
	return 0;
}
