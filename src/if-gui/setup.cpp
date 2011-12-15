#include "setup.hpp"

#include "droidApp.hpp"
#include "data.hpp"
#include "log.hpp"

#include <wx/xrc/xmlres.h>
#include <wx/icon.h>
#include <wx/msgdlg.h>

#include <iostream>
using namespace std;
using namespace droidpad;

#ifdef OS_WIN32
BEGIN_EVENT_TABLE(WinSetupFrame, wxFrame)
	EVT_SETUP(SETUP_INITIALISED, WinSetupFrame::OnSetupInitialised)
	EVT_SETUP(REMOVE_INITIALISED, WinSetupFrame::OnRemoveInitialised)

	EVT_SETUP(SETUP_FINISHED, WinSetupFrame::OnSetupFinished)

	EVT_CLOSE(WinSetupFrame::OnClose)
END_EVENT_TABLE()

#define FRAME_TITLE "DroidPad - Setup"

#ifdef __WXMSW__
#define _FRAME_ICON wxT("icon.xpm")
#else
#define _FRAME_ICON wxT("iconlarge.xpm")
#endif

#define LOADXRC(_xml, _name, _type) _name = XRCCTRL(*this, #_xml, _type); \
					    if(_name == NULL) handleXMLError(wxT(#_xml));

WinSetupFrame::WinSetupFrame(int mode) :
	mode(mode),
	wxFrame(NULL, -1, _(FRAME_TITLE), wxDefaultPosition, wxDefaultSize)
{
	SetIcon(wxIcon(wxString(Data::getFilePath(_FRAME_ICON).c_str(), wxConvUTF8), wxBITMAP_TYPE_XPM));

	parent = new wxPanel(this);
	parentSizer = new PageSizer;
	parent->SetSizer(parentSizer);

	// TEXT
	textPanel = new wxPanel(parent);
	wxBoxSizer *textPanelSizer = new wxBoxSizer(wxVERTICAL);
	textPanel->SetSizer(textPanelSizer);

	textPanel_text = new wxStaticText(textPanel, -1, _("Loading..."), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	textPanelSizer->Add(textPanel_text, 1, wxEXPAND | wxALIGN_CENTER_HORIZONTAL);

	parentSizer->Add(textPanel, 1, wxEXPAND | wxALL, 5);
	
	// TESTING
	otherPanel = new wxPanel(parent);
	wxBoxSizer *otherPanelSizer = new wxBoxSizer(wxVERTICAL);
	otherPanel->SetSizer(otherPanelSizer);

	wxButton *button = new wxButton(otherPanel, -1, _("Test"));
	otherPanelSizer->Add(button, 1, wxEXPAND | wxALIGN_CENTER_HORIZONTAL);

	parentSizer->Add(otherPanel, 1, wxEXPAND | wxALL, 5);

	parentSizer->HidePages();
	activateView(VIEW_TEXT);

	setup = new SetupThread(*this, mode);
	setup->Create();
	setup->Run();
	running = true;
}

WinSetupFrame::~WinSetupFrame() {
}

void WinSetupFrame::OnClose(wxCloseEvent& event) {
#ifdef DEBUG // If debugging, allow force closing of this.
	LOGW("WARNING: Force closing DroidPad.");
	if(running) setup->Delete();
	Destroy();
#endif
}

void WinSetupFrame::OnSetupInitialised(SetupEvent& event) {
	LOGV("Setup initialised");
}

void WinSetupFrame::OnRemoveInitialised(SetupEvent& event) {
	LOGV("Removing initialised");
}

void WinSetupFrame::OnSetupFinished(SetupEvent& event) {
	LOGV("Setup finished");
	running = false;
}

void WinSetupFrame::activateView(int view) {
	LOGV("Activating view");
	switch(view) {
		case VIEW_TEXT:
			parentSizer->SetCurrent(0);
			break;
		case VIEW_OTHER:
			parentSizer->SetCurrent(1);
			break;
	}
}

void WinSetupFrame::handleXMLError(wxString name) {
	wxMessageDialog(this, wxString::Format(_("Error loading layout while loading \"%s\".\nPlease report this as a bug."), name.c_str()), _("Error loading layout"), wxOK | wxICON_EXCLAMATION).ShowModal();
}
#endif


bool dpSetup(DroidApp& app) {
#ifdef OS_LINUX
	cout << " *** No setup is currently required for DroidPad on Linux." << endl << endl;
	return false;
#elif OS_WIN32
	WinSetupFrame *frame = new WinSetupFrame(MODE_SETUP);
	frame->Show(true);
	app.SetTopWindow(frame);
	return true;
#endif
}

bool dpRemove(DroidApp& app) {
#ifdef OS_LINUX
	// Nothing
	return false;
#elif OS_WIN32
	WinSetupFrame *frame = new WinSetupFrame(MODE_REMOVE);
	frame->Show(true);
	app.SetTopWindow(frame);
	return true;
#endif
}
