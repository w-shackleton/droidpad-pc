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

#include "data.hpp"
#include "qrPanel.hpp"

#include <wx/icon.h>
#include <wx/sizer.h>

using namespace boost::uuids;
using namespace droidpad;

BEGIN_EVENT_TABLE(DevicePair, wxDialog)
END_EVENT_TABLE()

#ifdef __WXMSW__
#define _FRAME_ICON wxT("icon.xpm")
#else
#define _FRAME_ICON wxT("iconlarge.xpm")
#endif

#define FRAME_TITLE "Pair new device"

DevicePair::DevicePair(wxWindow *parent, uuid id) :
	wxDialog(parent, -1, _(FRAME_TITLE), wxDefaultPosition)
{
	SetIcon(wxIcon(Data::getFilePath(_FRAME_ICON), wxBITMAP_TYPE_XPM));

	wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(panelSizer);

	// Set up QR code part
	qrPanel *qrCode = new qrPanel(this, wxT("QR CONTENT TEXT TEST"));
	panelSizer->Add(qrCode, 0, wxALIGN_CENTRE);

	panelSizer->SetSizeHints(this);
	Fit();
}
