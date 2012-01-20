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

// This dialog lets the user choose how DP should do drivers on win64

#ifndef _DRIVERCHOICE_H
#define _DRIVERCHOICE_H

#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>

#define DRIVER_CHOICE_NOJS 0
#define DRIVER_CHOICE_PERBOOT 1
#define DRIVER_CHOICE_TESTMODE 2

class DriverChoice : public wxDialog {
	enum {
		ID_ERROR_OK,
	};

	public:
		DriverChoice(wxWindow *parent);
		~DriverChoice();
		DECLARE_EVENT_TABLE()

	public:

	private:
		void SelectPermanent(wxCommandEvent &evt);
		void SelectPerBoot(wxCommandEvent &evt);
		void SelectNoInstall(wxCommandEvent &evt);
};

#endif
