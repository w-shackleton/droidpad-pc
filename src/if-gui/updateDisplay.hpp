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
#ifndef _UPDATEDISPLAY_H
#define _UPDATEDISPLAY_H

#include <wx/frame.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/gauge.h>
#include <wx/stattext.h>

#include "deviceManager.hpp"

class UpdateDisplay : public wxFrame
{
	public:
		UpdateDisplay(wxWindow *parent, droidpad::DeviceManager &devices);
		DECLARE_EVENT_TABLE()
	public:
		void SetProgress(int bytesDone, int bytesTotal);

		/**
		 * Closes the window without signaling to the devicemanager, ie. this call should come when the DL is done.
		 */
		void Close();
	protected:
		void OnCancel(wxCommandEvent &evt);
		void OnClose(wxCloseEvent &evt);

		droidpad::DeviceManager &devices;

		wxGauge *progress;
		wxStaticText *progressText;
};

#endif
