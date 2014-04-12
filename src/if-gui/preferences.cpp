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

#include "preferences.hpp"

#include <wx/xrc/xmlres.h>
#include <wx/msgdlg.h>
#include "data.hpp"
#include "log.hpp"

BEGIN_EVENT_TABLE(Preferences, wxDialog)
	EVT_BUTTON(XRCID("okButton"), Preferences::onDone)
	EVT_BUTTON(XRCID("cancelButton"), Preferences::onCancel)
END_EVENT_TABLE()

using namespace droidpad;
using namespace std;

#ifdef __WXMSW__
#define _FRAME_ICON wxT("icon.xpm")
#else
#define _FRAME_ICON wxT("iconlarge.xpm")
#endif

#define CHECK_XML(_name) if(!_name) handleXMLError(wxT(#_name))

#define LOADXRC(_xml, _name, _type) _name = XRCCTRL(*this, #_xml, _type); \
					    if(_name == NULL) handleXMLError(wxT(#_xml));

#define NUM_AXIS 6
#define NUM_BUTTONS 12

Preferences::Preferences(wxWindow *parent) {
	if(!wxXmlResource::Get()->LoadDialog(this, parent, wxT("preferences"))) {
		LOGE("Failed to load preferences dialog XML.");
	}

	SetIcon(wxIcon(Data::getFilePath(_FRAME_ICON), wxBITMAP_TYPE_XPM));

	LOADXRC(slideshowBlackKey, blackKey, wxTextCtrl);
	LOADXRC(slideshowWhiteKey, whiteKey, wxTextCtrl);

	blackKey->SetValue(Data::blackKey);
	whiteKey->SetValue(Data::whiteKey);

	Fit();
}

void Preferences::onDone(wxCommandEvent &evt) {
	// Save selection to prefs again
	if(blackKey->GetValue().size() > 0)
		Data::blackKey = blackKey->GetValue().GetChar(0);
	else Data::blackKey = 'b';
	if(whiteKey->GetValue().size() > 0)
		Data::whiteKey = whiteKey->GetValue().GetChar(0);
	else Data::whiteKey = 'w';
	Data::savePreferences();
	EndModal(1);
}
void Preferences::onCancel(wxCommandEvent &evt) {
	EndModal(0);
}

void Preferences::handleXMLError(wxString name)
{
	wxMessageDialog(this, wxString::Format(_("Error loading layout while loading \"%s\".\nPlease report this as a bug."), name.c_str()), _("Error loading layout"), wxOK | wxICON_EXCLAMATION).ShowModal();
}

