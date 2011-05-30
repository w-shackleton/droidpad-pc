#include "droidApp.hpp"

#include <wx/xrc/xmlres.h>
#include <wx/image.h>
#include <wx/msgdlg.h>

#include <iostream>
using namespace std;

#include "droidFrame.hpp"
#include "data.hpp"

using namespace droidpad;

IMPLEMENT_APP(DroidApp)

bool DroidApp::OnInit()
{
#ifdef OS_LINUX
	setenv("UBUNTU_MENUPROXY", "0", 1); // Ubuntu 10.10 fix
#endif

	if(!wxApp::OnInit())
		return false;
#ifdef OS_LINUX
	logger = new wxLogStream(&cerr);
#elif OS_WIN32
#ifdef DEBUG
	logger = new wxLogWindow(NULL, _("DroidPad debug log output"));
	wxLog::SetVerbose(true);
#else
	logger = new wxLogGui();
#endif
#endif
	wxLog::SetActiveTarget(logger);
	SetAppName(_T("droidpad"));
	wxInitAllImageHandlers();

	if(!Data::initialise())
	{
		wxMessageDialog(NULL, _("Could not find application data,\npossibly because application was installed incorrectly?"), _("Error finding data"), wxOK | wxICON_EXCLAMATION).ShowModal();
		return false;
	}

	wxString layoutPath = Data::getFilePath(wxT("layout.xrc"));
	wxXmlResource::Get()->InitAllHandlers();
	if(!wxXmlResource::Get()->Load(layoutPath))
	{
		wxMessageDialog(NULL, _("Could not load application data,\npossibly because application was installed incorrectly?"), _("Error finding data"), wxOK | wxICON_EXCLAMATION).ShowModal();
		return false;
	}

	DroidFrame *frame = new DroidFrame;
	frame->Show(true);
	SetTopWindow(frame);
	return true;
}

void DroidApp::onDFFinish() {
	ExitMainLoop();
}
