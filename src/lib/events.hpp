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
#ifndef DP_EVENTS_H
#define DP_EVENTS_H

#include <wx/event.h>

#include <vector>

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_LOCAL_EVENT_TYPE(dpDM_INITIALISED, 1)
	DECLARE_LOCAL_EVENT_TYPE(dpDM_CLOSED, 2)
	DECLARE_LOCAL_EVENT_TYPE(dpDEVICE_FINDER_FINISHED, 3)

	DECLARE_LOCAL_EVENT_TYPE(dpDEVICES_LIST, 4)

	DECLARE_LOCAL_EVENT_TYPE(dpTHREAD_STARTED, 5)
	DECLARE_LOCAL_EVENT_TYPE(dpTHREAD_ERROR, 6)

	DECLARE_LOCAL_EVENT_TYPE(dpTHREAD_NOTIFICATION, 7)

	DECLARE_LOCAL_EVENT_TYPE(dpTHREAD_FINISH, 8)

	DECLARE_LOCAL_EVENT_TYPE(dpUPDATE_NOTIFICATION, 9)

	DECLARE_LOCAL_EVENT_TYPE(dpDL_STARTED, 10)
	DECLARE_LOCAL_EVENT_TYPE(dpDL_PROGRESS, 11)
	DECLARE_LOCAL_EVENT_TYPE(dpDL_FAILED, 12)
	DECLARE_LOCAL_EVENT_TYPE(dpDL_SUCCESS, 13)
END_DECLARE_EVENT_TYPES()

namespace droidpad {
	enum {
		DM_SUCCESS,
		DM_FAIL,
		DM_FINISHED,

		THREAD_ERROR_CONNECT_FAIL,
		THREAD_ERROR_SETUP_FAIL,
		THREAD_ERROR_NO_JS_DEVICE,
		THREAD_WARNING_CONNECTION_LOST,

		THREAD_INFO_CONNECTED,

		/**
		 * The phone disconnected successfully.
		 */
		THREAD_INFO_FINISHED,
	};
	
	enum {
		DL_STARTED,
		DL_PROGRESS,
		DL_FAILED,
		DL_SUCCESS
	};

	/**
	 * Some sort of device manager status notification
	 */
	class DMEvent : public wxEvent
	{
		public:
			DMEvent(wxEventType type = dpDM_INITIALISED, int status = DM_SUCCESS);
			wxEvent* Clone() const;

			inline int getStatus() {return status;}

			DECLARE_DYNAMIC_CLASS(DMEvent)

		private:
			int status;
	};

	typedef void (wxEvtHandler::*dmEventFunction)(DMEvent&);

	class AndroidDevice;

	/**
	 * Represents a list of potential devices
	 */
	class DevicesList : public wxEvent
	{
		public:
			DevicesList(std::vector<AndroidDevice> list = std::vector<AndroidDevice>());
			wxEvent* Clone() const;

			DECLARE_DYNAMIC_CLASS(DevicesList)

			std::vector<AndroidDevice> list;
	};

	typedef void (wxEvtHandler::*devicesListFunction)(DevicesList&);

	class UpdateInfo;

	// No need to remove definition on non-win32
	class UpdatesNotification : public wxEvent {
		public:
			UpdatesNotification(std::vector<UpdateInfo> versions, std::vector<UpdateInfo> latest, bool userRequest = false);
			UpdatesNotification();
			wxEvent *Clone() const;

			DECLARE_DYNAMIC_CLASS(UpdatesNotification)

			std::vector<UpdateInfo> versions;
			// This holds any new update that should be installed now.
			std::vector<UpdateInfo> latest;

			bool userRequest;
	};

	typedef void (wxEvtHandler::*updatesNotificationFunction)(UpdatesNotification&);

	class DlStatus : public wxEvent {
		public:
			DlStatus(wxEventType type = dpDL_PROGRESS, int bytesDone = 0, int totalBytes = 1);
			wxEvent *Clone() const;

			int bytesDone, totalBytes;

			DECLARE_DYNAMIC_CLASS(DlStatus)
	};

	typedef void (wxEvtHandler::*dlStatusFunction)(DlStatus&);
};

#define EVT_DMEVENT(evt, func)					\
	DECLARE_EVENT_TABLE_ENTRY(evt,				\
			-1,					\
			-1,					\
			(wxObjectEventFunction)			\
			(dmEventFunction) & func,		\
			(wxObject *) NULL),


#define EVT_DEVICES_LIST(evt, func)				\
	DECLARE_EVENT_TABLE_ENTRY(evt,				\
			-1,					\
			-1,					\
			(wxObjectEventFunction)			\
			(devicesListFunction) & func,		\
			(wxObject *) NULL),

#define EVT_NEW_UPDATES(evt, func)				\
	DECLARE_EVENT_TABLE_ENTRY(evt,				\
			-1,					\
			-1,					\
			(wxObjectEventFunction)			\
			(updatesNotificationFunction) & func,	\
			(wxObject *) NULL),

#define EVT_DL_PROGRESS(evt, func)				\
	DECLARE_EVENT_TABLE_ENTRY(evt,				\
			-1,					\
			-1,					\
			(wxObjectEventFunction)			\
			(dlStatusFunction) & func,	\
			(wxObject *) NULL),

#endif
