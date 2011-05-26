#ifndef DP_EVENTS_H
#define DP_EVENTS_H

#include <wx/event.h>

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(dpADB_INITIALISED, 1)
END_DECLARE_EVENT_TYPES()

namespace droidpad {
	static int ADB_SUCCESS = 0;
	static int ADB_FAIL = 1;

	class AdbEvent : public wxEvent
	{
		public:
			AdbEvent(wxEventType type = dpADB_INITIALISED, int status = ADB_SUCCESS);
			wxEvent* Clone() const;

			DECLARE_DYNAMIC_CLASS(AdbEvent)

		private:
			int status;
	};

	typedef void (wxEvtHandler::*adbEventFunction)(AdbEvent&);

};

#define EVT_ADBEVENT(evt, func)					\
	DECLARE_EVENT_TABLE_ENTRY(evt,				\
			-1,					\
			-1,					\
			(wxObjectEventFunction)			\
			(adbEventFunction) & func,		\
			(wxObject *) NULL),

#endif
