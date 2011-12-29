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

#include "data.hpp"

#include <wx/button.h>
#include <wx/icon.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>

BEGIN_EVENT_TABLE(CustomHost, wxDialog)
	EVT_BUTTON(wxID_OK, CustomHost::onDone)
END_EVENT_TABLE()

using namespace droidpad;

#ifdef __WXMSW__
#define _FRAME_ICON wxT("icon.xpm")
#else
#define _FRAME_ICON wxT("iconlarge.xpm")
#endif

#define FRAME_TITLE "Custom device"

#define DEFAULT_PORT 3141

CustomHost::CustomHost(wxWindow *parent, droidpad::AndroidDevice &device) :
	wxDialog(parent, -1, _(FRAME_TITLE), wxDefaultPosition, wxSize(250, 150)),
	device(device)
{
	SetIcon(wxIcon(wxString(Data::getFilePath(_FRAME_ICON).c_str(), wxConvUTF8), wxBITMAP_TYPE_XPM));

	parent = new wxPanel(this);
	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);
	parent->SetSizer(parentSizer);


	wxPanel *entryPanel = new wxPanel(parent);
	wxGridSizer *entrySizer = new wxGridSizer(2);
	entryPanel->SetSizer(entrySizer);

	entrySizer->Add(new wxStaticText(entryPanel, -1, _("IP Address / hostname")));

	host = new wxTextCtrl(entryPanel, _ID_HOST, Data::host);
	entrySizer->Add(host, 1);

	entrySizer->Add(new wxStaticText(entryPanel, -1, _("Port (default 3141)")));

	port = new wxTextCtrl(entryPanel, _ID_PORT, wxString::Format(wxT("%d"), Data::port));
	entrySizer->Add(port, 1);

	parentSizer->Add(entryPanel, 1, wxALL | wxEXPAND, 5);

	
	wxPanel *buttons = new wxPanel(parent);
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttons->SetSizer(buttonSizer);

	wxButton *cancel = new wxButton(buttons, wxID_CANCEL);
	wxButton *ok = new wxButton(buttons, wxID_OK);

	buttonSizer->Add(cancel, 1, wxEXPAND | wxALL, 5);
	buttonSizer->Add(ok, 1, wxEXPAND | wxALL, 5);

	parentSizer->Add(buttons, 0, wxALL, 5);
}

void CustomHost::onDone(wxCommandEvent &evt) {
	// Save to prefs
	Data::host = host->GetValue();
	long portVal;
	if(!port->GetValue().ToLong(&portVal)) {
		portVal = DEFAULT_PORT;
	}
	Data::port = portVal;

	device.port = Data::port;
	device.ip = Data::host;

	Data::savePreferences();
	EndModal(wxID_OK);
}
