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

#include "qrPanel.hpp"

#include <wx/dcclient.h>

BEGIN_EVENT_TABLE(qrPanel, wxPanel)
	EVT_PAINT(qrPanel::paintEvent)
END_EVENT_TABLE()

#include "log.hpp"
#include "ext/hexdump.h"

#define SIZE_PER_DOT 10
#define PADDING 10

qrPanel::qrPanel(wxWindow *parent, wxString text) :
	wxPanel(parent)
{
	code = createQrData(text);
	if(!code) return;
	LOGV("Successfully created qr data");
	wxSize size(
			code->width * SIZE_PER_DOT + 2 * PADDING,
			code->width * SIZE_PER_DOT + 2 * PADDING);
	SetMaxSize(size);
	SetMinSize(size);
}

qrPanel::~qrPanel() {
	QRcode_free(code);
}

void qrPanel::paintEvent(wxPaintEvent & evt)
{
	// depending on your system you may need to look at double-buffered dcs
	wxPaintDC dc(this);
	render(dc);
}

void qrPanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void qrPanel::render(wxDC& dc)
{
	if(!code) return;
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();
	dc.SetBrush(*wxBLACK_BRUSH);
	for(int x = 0; x < code->width; x++) {
		for(int y = 0; y < code->width; y++) {
			if(code->data[y*code->width+x] & 0x1)
				dc.DrawRectangle(
						x * SIZE_PER_DOT + PADDING,
						y * SIZE_PER_DOT + PADDING,
						SIZE_PER_DOT,
						SIZE_PER_DOT);
		}
	}
}

QRcode *qrPanel::createQrData(wxString text) {
	return QRcode_encodeString(text.mb_str(), 0, QR_ECLEVEL_M, QR_MODE_8, 1);
}

void qrPanel::setContent(wxString text) {
	if(code) QRcode_free(code);

	code = createQrData(text);
	if(!code) return;
	LOGV("Successfully created qr data (2)");
	wxSize size(
			code->width * SIZE_PER_DOT + 2 * PADDING,
			code->width * SIZE_PER_DOT + 2 * PADDING);
	SetMaxSize(size);
	SetMinSize(size);
	SetSize(size);
	paintNow();
}
