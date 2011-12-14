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
#include <wx/button.h>
#include <wx/stattext.h>

#include "winSetup.hpp"

class WinSetupFrame : public wxFrame {
	public:
		WinSetupFrame(int mode);
		~WinSetupFrame();
		DECLARE_EVENT_TABLE()

		void OnClose(wxCloseEvent& event);

		void OnSetupInitialised(droidpad::SetupEvent& event);
		
		void OnRemoveInitialised(droidpad::SetupEvent& event);
	public:
	private:
		void handleXMLError(wxString name);

		int mode;
		droidpad::SetupThread* setup;

		// This panel simply shows some text.
		wxPanel *textPanel;

		wxStaticText* textPanel_text;
};

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
