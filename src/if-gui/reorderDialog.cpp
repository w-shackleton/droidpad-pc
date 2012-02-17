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

#include "reorderDialog.hpp"

#include <wx/xrc/xmlres.h>
#include <wx/msgdlg.h>
#include "data.hpp"
#include "log.hpp"

BEGIN_EVENT_TABLE(ReorderDialog, wxDialog)
	EVT_BUTTON(XRCID("okButton"), ReorderDialog::onDone)
	EVT_BUTTON(XRCID("cancelButton"), ReorderDialog::onCancel)
END_EVENT_TABLE()

using namespace droidpad;

#ifdef __WXMSW__
#define _FRAME_ICON wxT("icon.xpm")
#else
#define _FRAME_ICON wxT("iconlarge.xpm")
#endif

#define FRAME_TITLE "Custom device"

#define CHECK_XML(_name) if(!_name) handleXMLError(wxT(#_name))

#define NUM_AXIS 6
#define NUM_BUTTONS 12

ReorderDialog::ReorderDialog(wxWindow *parent) {
	if(!wxXmlResource::Get()->LoadDialog(this, parent, wxT("axisReorder"))) {
		LOGE("Failed to load dialog XML.");
	}

	SetIcon(wxIcon(Data::getFilePath(_FRAME_ICON), wxBITMAP_TYPE_XPM));

	// Locate the required sizers
	wxSizer *top = GetSizer();
	CHECK_XML(top);

	wxSizer *container = top->GetItem((size_t)0)->GetSizer();
	CHECK_XML(container);

	wxSizer *inner1 = container->GetItem((size_t)0)->GetSizer();
	CHECK_XML(inner1);
	wxSizer *inner2 = container->GetItem((size_t)1)->GetSizer();
	CHECK_XML(inner2);

	// These are the necessary sizers
	wxGridSizer *axisGrid   = (wxGridSizer*)inner1->GetItem((size_t)1)->GetSizer();
	CHECK_XML(axisGrid);
	wxGridSizer *buttonGrid = (wxGridSizer*)inner2->GetItem((size_t)1)->GetSizer();
	CHECK_XML(buttonGrid);

	wxString axisSelections[NUM_AXIS];
	for(int i = 0; i < NUM_AXIS; i++) {
		axisSelections[i] = getAxisText(i);
	}
	wxString buttonSelections[NUM_BUTTONS];
	for(int i = 0; i < NUM_BUTTONS; i++) {
		buttonSelections[i] = getButtonText(i);
	}

	for(int i = 0; i < NUM_AXIS; i++) {
		wxStaticText *text = new wxStaticText(this, -1, wxString::Format(_("Axis %d ->"), i+1), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		axisGrid->Add(text, 0, wxALIGN_CENTRE | wxEXPAND | wxALL, 3);

		wxComboBox *box = new wxComboBox(this, -1,
				getAxisText(i),
				wxDefaultPosition, wxSize(80, -1), NUM_AXIS,
				axisSelections,
				wxCB_READONLY);
		axes.push_back(box);
		axisGrid->Add(box, 0, wxEXPAND);
	}

	for(int i = 0; i < NUM_BUTTONS; i++) {
		wxStaticText *text = new wxStaticText(this, -1, wxString::Format(_("Button %d ->"), i+1), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		buttonGrid->Add(text, 0, wxALIGN_CENTRE | wxEXPAND | wxALL, 3);

		wxComboBox *box = new wxComboBox(this, -1,
				getButtonText(i),
				wxDefaultPosition, wxSize(90, -1), NUM_BUTTONS,
				buttonSelections,
				wxCB_READONLY);
		buttons.push_back(box);
		buttonGrid->Add(box, 0, wxEXPAND);
	}

	Fit();
}

void ReorderDialog::onDone(wxCommandEvent &evt) {
}
void ReorderDialog::onCancel(wxCommandEvent &evt) {
	EndModal(1);
}

void ReorderDialog::handleXMLError(wxString name)
{
	wxMessageDialog(this, wxString::Format(_("Error loading layout while loading \"%s\".\nPlease report this as a bug."), name.c_str()), _("Error loading layout"), wxOK | wxICON_EXCLAMATION).ShowModal();
}

