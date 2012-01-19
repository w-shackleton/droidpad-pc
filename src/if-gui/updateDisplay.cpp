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
#include"updateDisplay.hpp"

#include <wx/icon.h>
#include <wx/panel.h>

#include "log.hpp"
#include "data.hpp"

// This class is win32 only at the moment.
#include "updater.hpp"

#ifdef __WXMSW__
#define _FRAME_ICON wxT("icon.xpm")
#else
#define _FRAME_ICON wxT("iconlarge.xpm")
#endif

#define FRAME_TITLE "Update DroidPad"

using namespace droidpad;

BEGIN_EVENT_TABLE(UpdateDisplay, wxFrame)
	EVT_BUTTON(wxID_CANCEL, UpdateDisplay::OnCancel)

	EVT_CLOSE(UpdateDisplay::OnClose)
END_EVENT_TABLE()

UpdateDisplay::UpdateDisplay(wxWindow *parent, DeviceManager &devices) :
	wxFrame(parent, -1, _(FRAME_TITLE), wxDefaultPosition, wxDefaultSize, wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN),
	devices(devices)
{
	SetIcon(wxIcon(wxString(Data::getFilePath(_FRAME_ICON).c_str(), wxConvUTF8), wxBITMAP_TYPE_XPM));

	wxPanel *panel = new wxPanel(this);

	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	panel->SetSizer(sizer);

	wxStaticText *title = new wxStaticText(panel, -1, _("Downloading update"));
	sizer->Add(title, 0, wxALIGN_CENTRE | wxALL, 3);

	progress = new wxGauge(panel, -1, 1, wxDefaultPosition, wxSize(200, -1));
	sizer->Add(progress, 0, wxEXPAND | wxLEFT | wxRIGHT, 3);

	progressText = new wxStaticText(panel, -1, wxT("..."));
	sizer->Add(progressText, 0);

	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(buttonSizer, 0, wxALIGN_CENTRE | wxALL, 3);

	wxButton *cancel = new wxButton(panel, wxID_CANCEL, _("Cancel"));
	buttonSizer->Add(cancel);

	sizer->SetSizeHints(panel);
	panel->Fit();
	Fit();
}

void UpdateDisplay::SetProgress(int bytesDone, int bytesTotal) {
	progress->SetRange(bytesTotal / 1024);
	progress->SetValue(bytesDone / 1024);
	progressText->SetLabel(wxString::Format(_("%d KB / %d KB"), bytesDone / 1024, bytesTotal / 1024));
}

void UpdateDisplay::Close() {
	Destroy();
}

void UpdateDisplay::OnCancel(wxCommandEvent &evt) {
	devices.CancelUpdate();
	Destroy();
}

void UpdateDisplay::OnClose(wxCloseEvent &evt) {
	wxCommandEvent event;
	OnCancel(event);
}
