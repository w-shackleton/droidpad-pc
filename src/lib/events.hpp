#ifndef DP_EVENTS_H
#define DP_EVENTS_H

#include <wx/event.h>

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(dpDM_INITIALISED, 1)
	DECLARE_EVENT_TYPE(dpDM_CLOSED, 1)
END_DECLARE_EVENT_TYPES()

namespace droidpad {
	static int DM_SUCCESS = 0;
	static int DM_FAIL = 1;

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

};

#define EVT_DMEVENT(evt, func)					\
	DECLARE_EVENT_TABLE_ENTRY(evt,				\
			-1,					\
			-1,					\
			(wxObjectEventFunction)			\
			(dmEventFunction) & func,		\
			(wxObject *) NULL),

#endif
