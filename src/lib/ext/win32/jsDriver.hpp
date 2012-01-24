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

// wxWidgets-ized definitions for the vJoy installer functions.

#ifndef DP_VJOY_H
#define DP_VJOY_H

#include <wx/string.h>

wxString jsGetDevHwId();

void jsInstallOpenLog(wxString location);

void jsInstallCloseLog();

bool jsInstall(wxString infPath, wxString hwId);
bool jsRemove(wxString infPath, wxString hwId);
bool jsPurge(wxString infPath, wxString hwId);
bool jsRepair(wxString infPath, wxString hwId);

/**
 * If true, the DP driver is installed and also running.
 * This could return false if installed but not running.
 */
bool isJsInstalledAndRunning();

#endif
