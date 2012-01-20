
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
#ifndef DP_WIN_SETUP_H
#define DP_WIN_SETUP_H

#include <wx/thread.h>
#include <wx/event.h>
#include <wx/string.h>

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_LOCAL_EVENT_TYPE(SETUP_BLANKSTATUS, 0)
	DECLARE_LOCAL_EVENT_TYPE(SETUP_INITIALISED, 1)
	DECLARE_LOCAL_EVENT_TYPE(REMOVE_INITIALISED, 2)

	DECLARE_LOCAL_EVENT_TYPE(SETUP_REMOVING_OLD, 3)
	DECLARE_LOCAL_EVENT_TYPE(SETUP_INSTALLING_NEW, 4)

	DECLARE_LOCAL_EVENT_TYPE(SETUP_SHOW_DRIVERCHOICE, 5)

	DECLARE_LOCAL_EVENT_TYPE(REMOVE_REMOVING, 6)

	DECLARE_LOCAL_EVENT_TYPE(SETUP_ERROR, 50)

	DECLARE_LOCAL_EVENT_TYPE(SETUP_FINISHED, 100)
	DECLARE_LOCAL_EVENT_TYPE(SETUP_EXIT, 101)
END_DECLARE_EVENT_TYPES()

namespace droidpad {

	enum {
		MODE_SETUP,
		MODE_REMOVE
	};

	class SetupThread : public wxThread {
		public:
			SetupThread(wxEvtHandler &callback, int mode);
			~SetupThread();
			void* Entry();

		private:
			void Cleanup();

			int mode;
			wxEvtHandler &callback;
	};

	/**
	 * Something has occured while setting up / removing
	 */
	class SetupEvent : public wxEvent
	{
		public:
			SetupEvent(wxEventType type = SETUP_BLANKSTATUS, wxString message = wxT(""));
			wxEvent* Clone() const;

			inline wxString GetMessage() {
				return message;
			}

			DECLARE_DYNAMIC_CLASS(SetupEvent)
		private:
			wxString message;
	};

	typedef void (wxEvtHandler::*setupEventFunction)(SetupEvent&);
};

#define EVT_SETUP(evt, func)					\
	DECLARE_EVENT_TABLE_ENTRY(evt,				\
			-1,					\
			-1,					\
			(wxObjectEventFunction)			\
			(setupEventFunction) & func,		\
			(wxObject *) NULL),



#endif
