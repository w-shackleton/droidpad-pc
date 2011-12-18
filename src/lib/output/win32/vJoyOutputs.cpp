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
#include "vJoyOutputs.hpp"

using namespace droidpad::win32;

int VJoyOutputs::OpenJoystick() {
	vJoyHandle = CreateFile(TEXT(DOS_FILE_NAME), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(vJoyHandle == INVALID_HANDLE_VALUE) {
		return GetLastError();
	}
	return 0;
}

VJoyOutputs::~VJoyOutputs() {
	CloseHandle(vJoyHandle);
}

int VJoyOutputs::SendPositions(JOYSTICK_POSITION &data) {
	unsigned int ioCode = LOAD_POSITIONS;
	unsigned int ioSize = sizeof(JOYSTICK_POSITION);

	DWORD bytesReturned;

	if(!DeviceIoControl(vJoyHandle, ioCode, &data, ioSize, NULL, 0, &bytesReturned, NULL)) {
		return GetLastError();
	}
	return 0;
}
