#include"droidFrame.hpp"

#include <wx/xrc/xmlres.h>
#include <wx/icon.h>
#include <wx/msgdlg.h>
		void handleXMLError(wxString name);

BEGIN_EVENT_TABLE(DroidFrame, wxFrame)
	EVT_BUTTON(XRCID("devicesRefresh"), DroidFrame::OnDevicesRefresh)
	EVT_BUTTON(XRCID("buttonStart"), DroidFrame::OnStart)
	EVT_BUTTON(XRCID("buttonStop"), DroidFrame::OnStop)
END_EVENT_TABLE()

#include <iostream>
using namespace std;

#include "data.hpp"

#ifdef __WXMSW__
#define _FRAME_ICON wxT("icon.xpm")
#else
#define _FRAME_ICON wxT("iconlarge.xpm")
#endif

#define FRAME_TITLE "DroidPad"

#define LOADXRC(_xml, _name, _type) _name = XRCCTRL(*this, #_xml, _type); \
					    if(_name == NULL) handleXMLError(wxT(#_xml));

DroidFrame::DroidFrame() :
	wxFrame(NULL, -1, _(FRAME_TITLE), wxDefaultPosition, wxSize(640, 480))
{
	init();
}

void DroidFrame::init()
{
	cout << "Loading" << endl;
	SetIcon(wxIcon(wxString(Misc::Data::getFilePath(_FRAME_ICON).c_str(), wxConvUTF8), wxBITMAP_TYPE_XPM));

	// Load XML
	SetMenuBar(wxXmlResource::Get()->LoadMenuBar(this, wxT("menu")));

	panel = wxXmlResource::Get()->LoadPanel(this, wxT("mainPanel"));
	panel->SetSizerAndFit(panel->GetSizer());
	wxSize sz;
	sz.SetHeight(200);
	sz.SetWidth(500);
	SetSize(sz);

	LOADXRC(buttonStart,	buttonStart,		wxButton);
	LOADXRC(buttonStop,	buttonStop,		wxButton);
	LOADXRC(devicesRefresh,	buttonDevicesRefresh,	wxButton);
}

void DroidFrame::handleXMLError(wxString name)
{
	wxMessageDialog(this, wxString::Format(_("Error loading layout while loading \"%s\".\nPlease report this as a bug."), name.c_str()), _("Error loading layout"), wxOK | wxICON_EXCLAMATION).ShowModal();
}

DroidFrame::~DroidFrame()
{
}

void DroidFrame::OnDevicesRefresh(wxCommandEvent& event)
{
	cout << "Refreshing devices" << endl;
}

void DroidFrame::OnStart(wxCommandEvent& event)
{
	cout << "Starting DroidPad" << endl;
}

void DroidFrame::OnStop(wxCommandEvent& event)
{
	cout << "Stopping DroidPad" << endl;
}

