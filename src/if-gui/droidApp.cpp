#include "droidApp.hpp"

#include <wx/xrc/xmlres.h>
#include <wx/log.h>
#include <wx/image.h>
#include <wx/msgdlg.h>

#include <iostream>
using namespace std;

#include "droidFrame.hpp"
#include "data.hpp"

IMPLEMENT_APP(DroidApp)

bool DroidApp::OnInit()
{
#ifndef __WXMSW__
	setenv("UBUNTU_MENUPROXY", "0", 1); // Ubuntu 10.10 fix
#endif

	if(!wxApp::OnInit())
		return false;
	wxLog *logger = new wxLogStream(&cerr); // TODO: Change this to different for MSW
	wxLog::SetActiveTarget(logger);
	SetAppName(_T("droidpad"));
	wxInitAllImageHandlers();

	if(!Misc::Data::initialise())
	{
		wxMessageDialog(NULL, _("Could not find application data,\npossibly because application was installed incorrectly?"), _("Error finding data"), wxOK | wxICON_EXCLAMATION).ShowModal();
		return false;
	}

	wxString layoutPath = Misc::Data::getFilePath(wxT("layout.xrc"));
	wxXmlResource::Get()->InitAllHandlers();
	if(!wxXmlResource::Get()->Load(layoutPath))
	{
		cout  << "Could not load resource file " << layoutPath << endl;
		return false;
	}

	DroidFrame *frame = new DroidFrame;
	frame->Show(true);
	SetTopWindow(frame);
	return true;
}
