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

#include "devicePair.hpp"

#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>

#include <boost/uuid/uuid_io.hpp>

#include <sstream>

#include "log.hpp"

using namespace boost::uuids;
using namespace droidpad;
using namespace std;

BEGIN_EVENT_TABLE(DevicePair, wxDialog)
END_EVENT_TABLE()

#ifdef __WXMSW__
#define _FRAME_ICON wxT("icon.xpm")
#else
#define _FRAME_ICON wxT("iconlarge.xpm")
#endif

#define FRAME_TITLE "Pair new device"

DevicePair::DevicePair(wxWindow *parent, uuid id) :
	wxDialog(parent, -1, _(FRAME_TITLE), wxDefaultPosition),
	newCredentials(CredentialStore::createNewSet()),
	computerId(id)
{
	SetIcon(wxIcon(Data::getFilePath(_FRAME_ICON), wxBITMAP_TYPE_XPM));

	wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(panelSizer);

	wxStaticText *title = new wxStaticText(this, -1, _("Pair new device"));
	wxFont largeFont = title->GetFont();
	largeFont.SetPointSize(18);
	title->SetFont(largeFont);
	panelSizer->Add(title, 0, wxALIGN_CENTRE | wxALL, 5);

	wxStaticText *desc = new wxStaticText(this, -1, _("From DroidPad, select 'Pair' on your device"));
	panelSizer->Add(desc, 0, wxALL, 5);

	wxBoxSizer *compNameSizer = new wxBoxSizer(wxHORIZONTAL);

	compNameSizer->Add(new wxStaticText(this, -1, _("Computer name:")), 0, wxALL, 5);
	// Using computer name from settings
	compName = new wxTextCtrl(this, ID_COMP_NAME, Data::computerName);
	Connect(ID_COMP_NAME, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(DevicePair::OnComputerNameChanged));
	compName->SetMaxLength(40);
	compNameSizer->Add(compName, 1, wxALL | wxEXPAND, 5);

	panelSizer->Add(compNameSizer, 0, wxALL | wxEXPAND, 5);

	// Set up QR code part
	qrCode = new qrPanel(this, createContent());
	panelSizer->Add(qrCode, 0, wxALIGN_CENTRE | wxTOP, 5);

	// Bottom button panel
	wxBoxSizer *buttons = new wxBoxSizer(wxHORIZONTAL);

	panelSizer->Add(buttons, 0, wxEXPAND);

	panelSizer->SetSizeHints(this);
	Fit();
}

void DevicePair::OnComputerNameChanged(wxCommandEvent &evt) {
	qrCode->setContent(createContent());
	Data::computerName = compName->GetValue();
	Data::savePreferences();
}

wxString DevicePair::createContent() {
	// Order of things
	// * Computer uuid
	// * Computer name
	// * Device uuid
	// * PSK (b64)
	stringstream result;
	result << boost::uuids::to_string(computerId) << "\n";
	result << compName->GetValue().mb_str() << "\n";
	result << boost::uuids::to_string(newCredentials.deviceId) << "\n";
	result << newCredentials.psk64_std();
	return wxString(result.str().c_str(), wxConvUTF8);

}
