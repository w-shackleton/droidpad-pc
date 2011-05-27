#ifndef DP_EVENTS_H
#define DP_EVENTS_H

#include <wx/event.h>

#include "droidpadCallbacks.hpp"
#include <vector>

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(dpDM_INITIALISED, 1)
	DECLARE_EVENT_TYPE(dpDM_CLOSED, 2)
	DECLARE_EVENT_TYPE(dpDEVICE_FINDER_FINISHED, 3)

	DECLARE_EVENT_TYPE(dpDEVICES_LIST, 4)
END_DECLARE_EVENT_TYPES()

namespace droidpad {
	static int DM_SUCCESS = 0;
	static int DM_FAIL = 1;
	static int DM_FINISHED = 2;

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

	class DevicesList : public wxEvent
	{
		public:
			DevicesList(std::vector<AndroidDevice> list = std::vector<AndroidDevice>());
			wxEvent* Clone() const;

			DECLARE_DYNAMIC_CLASS(DMEvent)

			std::vector<AndroidDevice> list;
	};

	typedef void (wxEvtHandler::*devicesListFunction)(DevicesList&);

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

#endif
