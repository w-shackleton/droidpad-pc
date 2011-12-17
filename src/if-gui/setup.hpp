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

// This code is for doing any extra setup to droidpad.

#ifndef _DROIDPADSETUP_H
#define _DROIDPADSETUP_H

#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>

#include "pageSizer.hpp"

#ifdef OS_WIN32
#include "winSetup.hpp"

enum {
	VIEW_TEXT,
	VIEW_ERROR
};

class WinSetupFrame : public wxFrame {
	enum {
		ID_ERROR_OK,
	};

	public:
		WinSetupFrame(int mode);
		~WinSetupFrame();
		DECLARE_EVENT_TABLE()

		void OnClose(wxCloseEvent& event);

		void OnSetupInitialised(droidpad::SetupEvent& event);
		
		void OnRemoveInitialised(droidpad::SetupEvent& event);

		void OnSetupRemovingOld(droidpad::SetupEvent& event);
		void OnSetupInstallingNew(droidpad::SetupEvent& event);

		void OnRemoveRemoving(droidpad::SetupEvent& event);

		void OnSetupFinished(droidpad::SetupEvent& event);
		void OnSetupExit(droidpad::SetupEvent& event);

		void OnSetupError(droidpad::SetupEvent& event);
		void OnErrorOkClick(wxCommandEvent& event);
	public:

	protected:
		void activateView(int view);

	private:
		void handleXMLError(wxString name);

		int mode;
		droidpad::SetupThread* setup;

		wxPanel *parent, *panel;
		PageSizer *parentSizer;
		wxSizer *panelSizer;

		// This panel simply shows some text.
		wxPanel *textPanel, *errorPanel;

		wxStaticText* textPanel_text, *errorPanel_text;

		bool running;
};
#endif


class DroidApp;

/**
 * Installs any extra required setup for DroidPad
 * Returns true to keep Main Loop running, false to not.
 */
bool dpSetup(DroidApp& app);

/**
 * Removes any extras (drivers etc) from the PC
 * Returns true to keep Main Loop running, false to not.
 */
bool dpRemove(DroidApp& app);

#endif
