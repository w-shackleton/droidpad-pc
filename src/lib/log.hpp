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
#ifndef DP_LOG_H
#define DP_LOG_H

#include <wx/log.h>

#define LOGV(_m) wxLogVerbose(wxT(_m))
#define LOGM(_m) wxLogMessage(wxT(_m))
#define LOGW(_m) wxLogWarning(wxT(_m))
#define LOGE(_m) wxLogError  (wxT(_m))

#define LOGVwx(_m) wxLogVerbose(_m)
#define LOGMwx(_m) wxLogMessage(_m)
#define LOGWwx(_m) wxLogWarning(_m)
#define LOGEwx(_m) wxLogError  (_m)

#endif
