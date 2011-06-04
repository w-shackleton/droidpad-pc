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
