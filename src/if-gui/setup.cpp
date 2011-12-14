#include "setup.hpp"

#include "droidApp.hpp"
#include "data.hpp"

#include <wx/xrc/xmlres.h>
#include <wx/icon.h>
#include <wx/msgdlg.h>

#include <iostream>
using namespace std;
using namespace droidpad;

BEGIN_EVENT_TABLE(WinSetupFrame, wxFrame)
	EVT_SETUP(SETUP_INITIALISED, WinSetupFrame::OnSetupInitialised)
	EVT_SETUP(REMOVE_INITIALISED, WinSetupFrame::OnRemoveInitialised)

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

	textPanel = wxXmlResource::Get()->LoadPanel(this, wxT("setupTextView"));
	textPanel->SetSizerAndFit(textPanel->GetSizer());
	wxSize sz;
	sz.SetHeight(200);
	sz.SetWidth(500);
	SetSize(sz);

	LOADXRC(setupSimpleText, textPanel_text, wxStaticText);

	setup = new SetupThread(*this, mode);
	setup->Create();
	setup->Run();
}

WinSetupFrame::~WinSetupFrame() {
}

void WinSetupFrame::OnClose(wxCloseEvent& event) {
}

void WinSetupFrame::OnSetupInitialised(SetupEvent& event) {
}

void WinSetupFrame::OnRemoveInitialised(SetupEvent& event) {
}

void WinSetupFrame::handleXMLError(wxString name) {
	wxMessageDialog(this, wxString::Format(_("Error loading layout while loading \"%s\".\nPlease report this as a bug."), name.c_str()), _("Error loading layout"), wxOK | wxICON_EXCLAMATION).ShowModal();
}


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
