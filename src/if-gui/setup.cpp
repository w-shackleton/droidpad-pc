#include "setup.hpp"

#include "droidApp.hpp"
#include "data.hpp"
#include "log.hpp"

#ifdef OS_64BIT
#include "driverChoice.hpp"
#endif

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
	EVT_SETUP(SETUP_EXIT, WinSetupFrame::OnSetupExit)
	EVT_SETUP(SETUP_ERROR, WinSetupFrame::OnSetupError)

	EVT_SETUP(REMOVE_REMOVING, WinSetupFrame::OnRemoveRemoving)

	EVT_SETUP(SETUP_SHOW_DRIVERCHOICE, WinSetupFrame::ShowDriverChoice)

	EVT_SETUP(SETUP_REMOVING_OLD, WinSetupFrame::OnSetupRemovingOld)
	EVT_SETUP(SETUP_INSTALLING_NEW, WinSetupFrame::OnSetupInstallingNew)

	EVT_BUTTON(ID_ERROR_OK, WinSetupFrame::OnErrorOkClick)

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
	wxFrame(NULL, -1, _(FRAME_TITLE), wxDefaultPosition, wxSize(180, 100))
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
	errorPanel = new wxPanel(parent);
	wxBoxSizer *errorPanelSizer = new wxBoxSizer(wxVERTICAL);
	errorPanel->SetSizer(errorPanelSizer);

	wxStaticText *errorText = new wxStaticText(errorPanel, -1, _("Something went wrong while installing DroidPad."), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	errorPanel_text = new wxStaticText(errorPanel, -1, _(""), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	wxButton *button = new wxButton(errorPanel, ID_ERROR_OK, _("OK"));

	errorPanelSizer->Add(errorText, 0, wxEXPAND | wxALIGN_CENTER_HORIZONTAL);
	errorPanelSizer->Add(errorPanel_text, 0, wxEXPAND | wxALIGN_CENTER_HORIZONTAL);
	errorPanelSizer->Add(button, 0, wxALIGN_CENTER_HORIZONTAL);

	parentSizer->Add(errorPanel, 1, wxEXPAND | wxALL, 5);

	parentSizer->HidePages();
	activateView(VIEW_TEXT);

	setup = new SetupThread(*this, mode);
	setup->Create();
	setup->Run();
	running = true;
}

WinSetupFrame::~WinSetupFrame() {
	wxGetApp().onDFFinish();
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
	activateView(VIEW_TEXT);
	textPanel_text->SetLabel(_("Installing DroidPad drivers..."));
}

void WinSetupFrame::OnRemoveInitialised(SetupEvent& event) {
	LOGV("Removing initialised");
}

void WinSetupFrame::OnSetupRemovingOld(SetupEvent& event) {
	activateView(VIEW_TEXT);
	textPanel_text->SetLabel(_("Removing old DroidPad drivers..."));
}
void WinSetupFrame::OnSetupInstallingNew(SetupEvent& event) {
	activateView(VIEW_TEXT);
	textPanel_text->SetLabel(_("Installing new DroidPad drivers..."));
}

void WinSetupFrame::OnRemoveRemoving(SetupEvent& event) {
	textPanel_text->SetLabel(_("Removing DroidPad drivers..."));
	activateView(VIEW_TEXT);
}

void WinSetupFrame::ShowDriverChoice(droidpad::SetupEvent& event) {
#ifdef OS_64BIT
	DriverChoice *choice = new DriverChoice(this);
	int result = choice->ShowModal();
	delete choice;
	setup->SetUserDriverResponse(result);
#endif
}

void WinSetupFrame::OnSetupFinished(SetupEvent& event) {
	LOGV("Setup finished");
	activateView(VIEW_TEXT);
	textPanel_text->SetLabel(_("Done!"));
}
void WinSetupFrame::OnSetupExit(SetupEvent& event) {
	running = false;
	Destroy();
}

void WinSetupFrame::OnSetupError(SetupEvent& event) {
	running = false;

	errorPanel_text->SetLabel(event.GetMessage());
	activateView(VIEW_ERROR);
}

void WinSetupFrame::OnErrorOkClick(wxCommandEvent& event) {
	Destroy();
}

void WinSetupFrame::activateView(int view) {
	switch(view) {
		case VIEW_TEXT:
			parentSizer->SetCurrent(0);
			break;
		case VIEW_ERROR:
			parentSizer->SetCurrent(1);
			break;
	}
	Update();
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
