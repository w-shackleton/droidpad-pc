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

#include "driverChoice.hpp"

#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(DriverChoice, wxDialog)
END_EVENT_TABLE()

#include "data.hpp"
#include "log.hpp"

#include <wx/stattext.h>
#include <wx/icon.h>

using namespace droidpad;

#ifdef __WXMSW__
#define _FRAME_ICON wxT("icon.xpm")
#else
#define _FRAME_ICON wxT("iconlarge.xpm")
#endif

#define FRAME_TITLE "Choose Joystick mode"

DriverChoice::DriverChoice(wxWindow *parent) :
	wxDialog(parent, -1, _(FRAME_TITLE), wxDefaultPosition)
{
	// wxPanel *panel = wxXmlResource::Get()->LoadPanel(this, wxT("mainPanel"));
	// panel->SetSizerAndFit(panel->GetSizer());
	if(!wxXmlResource::Get()->LoadDialog(this, parent, wxT("driverChoice"))) {
		LOGE("Failed to load dialog XML.");
	}

	SetIcon(wxIcon(Data::getFilePath(_FRAME_ICON), wxBITMAP_TYPE_XPM));
}

DriverChoice::~DriverChoice() { }
