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
#ifndef _AXISTWEAK_H
#define _AXISTWEAK_H

#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/combobox.h>
#include "data.hpp"

class AxisTweak : public wxDialog {
	public:
		AxisTweak(wxWindow *parent);
		DECLARE_EVENT_TABLE()
	public:
	protected:
		wxPanel *panel;

		void onCancel(wxCommandEvent &evt);
		void onDone(wxCommandEvent &evt);

		void updateFields(wxCommandEvent &evt);
		void updateOnScreen(wxCommandEvent &evt);
		void onTiltUpdate(wxScrollEvent &evt);
		void onRotationUpdate(wxScrollEvent &evt);
		void onOnScreenUpdate(wxScrollEvent &evt);
	private:
		void handleXMLError(wxString name);

		wxSlider *tiltAngleSlider, *tiltGammaSlider,
			 *rotationAngleSlider,
			 *onScreenGammaSlider;
		wxComboBox *tiltSelection, *onScreenSelection;

		droidpad::Tweaks tweaks;
};

#endif
