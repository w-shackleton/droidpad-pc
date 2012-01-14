#include "wxImagePanel.hpp"

#include <wx/dcclient.h>

BEGIN_EVENT_TABLE(wxImagePanel, wxPanel)
	EVT_PAINT(wxImagePanel::paintEvent)
END_EVENT_TABLE()

wxImagePanel::wxImagePanel(wxWindow *parent, wxString file, wxBitmapType format) :
	wxPanel(parent)
{
	image.LoadFile(file, format);

	SetMaxSize(wxSize(image.GetWidth(), image.GetHeight()));
	SetMinSize(wxSize(image.GetWidth(), image.GetHeight()));
}

void wxImagePanel::paintEvent(wxPaintEvent & evt)
{
	// depending on your system you may need to look at double-buffered dcs
	wxPaintDC dc(this);
	render(dc);
}

void wxImagePanel::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void wxImagePanel::render(wxDC&  dc)
{
	dc.DrawBitmap(image, 0, 0, true);
}
