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

#include "customHost.hpp"

#include <wx/button.h>

CustomHost::CustomHost(droidpad::AndroidDevice &device) :
	device(device)
{
	parent = new wxPanel(this);
	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);
	parent->SetSizer(parentSizer);

	wxPanel *buttons = new wxPanel;
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttons->SetSizer(buttonSizer);

	wxButton *cancel = new wxButton(buttons, wxID_CANCEL);
	wxButton *ok = new wxButton(buttons, wxID_OK);

	buttonSizer->Add(cancel, 1, wxEXPAND | wxALL, 5);
	buttonSizer->Add(ok, 1, wxEXPAND | wxALL, 5);

	parentSizer->Add(buttons, 0, wxALL, 5);
}
