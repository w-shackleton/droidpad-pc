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
#ifndef DP_CALLBACKS_H
#define DP_CALLBACKS_H

#include <wx/string.h>
#include <wx/clntdata.h>
#include <vector>
#include <stdint.h>

#define DEVICE_USB 1
#define DEVICE_NET 2

namespace droidpad {
	class AndroidDevice : public wxClientData {
		public:
			int type;

			wxString usbId;
			wxString ip;
			uint16_t port;
			wxString name;

			AndroidDevice& operator =(const AndroidDevice& b);
			operator wxString() const;
			bool operator ==(const AndroidDevice& b);
			AndroidDevice(const AndroidDevice& dev);
			AndroidDevice();
	};

	typedef std::vector<AndroidDevice> AndroidDeviceList;

	class DroidPadCallbacks
	{
		public:
			virtual ~DroidPadCallbacks();
			virtual void dpInitComplete(bool complete) = 0;
			virtual void dpCloseComplete() = 0;

			virtual void dpNewDeviceList(AndroidDeviceList &list) = 0;

			virtual void threadStarted() = 0;
			virtual void threadError(wxString failReason) = 0;
			virtual void threadStopped() = 0;
	};
};

#endif
