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
#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include "droidpadCallbacks.hpp"

class Preferences : public wxDialog {
	public:
		Preferences(wxWindow *parent);

		DECLARE_EVENT_TABLE()
	protected:
		void onDone(wxCommandEvent &evt);
		void onCancel(wxCommandEvent &evt);

		void handleXMLError(wxString name);
	private:
		wxTextCtrl *blackKey, *whiteKey;
};

#endif
