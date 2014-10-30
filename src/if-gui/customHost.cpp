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

CustomHost::CustomHost(wxWindow *parent, droidpad::AndroidDevice *device) :
	wxDialog(parent, -1, _(FRAME_TITLE), wxDefaultPosition, wxSize(250, 150)),
	device(device)
{
	SetIcon(wxIcon(wxString((const char*) Data::getFilePath(_FRAME_ICON).c_str(), wxConvUTF8), wxBITMAP_TYPE_XPM));

	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(parentSizer);


	wxGridSizer *entrySizer = new wxGridSizer(2);

	entrySizer->Add(new wxStaticText(this, -1, _("IP Address / hostname")), 0, wxALL, 3);
	host = new wxTextCtrl(this, 0, Data::host);
	entrySizer->Add(host, 1, wxALL, 3);

	entrySizer->Add(new wxStaticText(this, -1, _("Port (default 3141)")), 0, wxALL, 3);
	port = new wxTextCtrl(this, 0, wxString::Format(wxT("%d"), Data::port));
	entrySizer->Add(port, 1, wxALL, 3);

	secureSupported = new wxCheckBox(this, 0, _("Use a secure connection"));
	entrySizer->Add(secureSupported, 1, wxALL, 3);

//	entrySizer->Add(new wxStaticText(this, -1, _("Secure Port (default 3142)")), 0, wxALL, 3);
//	securePort = new wxTextCtrl(this, 0, wxString::Format(wxT("%d"), Data::securePort));
//	entrySizer->Add(securePort, 1, wxALL, 3);
	
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	wxButton *cancel = new wxButton(this, wxID_CANCEL);
	wxButton *ok = new wxButton(this, wxID_OK);

	buttonSizer->Add(cancel, 1, wxEXPAND | wxALL, 5);
	buttonSizer->Add(ok, 1, wxEXPAND | wxALL, 5);

	parentSizer->Add(entrySizer, 0, wxALL, 5);
	parentSizer->Add(buttonSizer, 0, wxALL, 5);
	parentSizer->SetSizeHints(this);
}

void CustomHost::onDone(wxCommandEvent &evt) {
	// Save to prefs
	Data::host = host->GetValue();
	long portVal;
	if(!port->GetValue().ToLong(&portVal)) {
		portVal = DEFAULT_PORT;
	}
	Data::port = portVal;

	device->port = Data::port;
	device->ip = Data::host;
	device->securePort = device->port + 1;
	Data::secureSupported = secureSupported->GetValue();
	device->secureSupported = secureSupported->GetValue();

	Data::savePreferences();
	EndModal(wxID_OK);
}
