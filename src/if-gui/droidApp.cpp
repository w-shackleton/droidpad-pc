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
#include "droidApp.hpp"

#include <wx/xrc/xmlres.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>

#include <iostream>
using namespace std;

#include "droidFrame.hpp"
#include "data.hpp"
#include "log.hpp"
#include "setup.hpp"

#ifdef OS_LINUX
#include "output/linux/dpinput.h"
#endif

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
#ifdef DEBUG
	wxLog::SetVerbose(true);
#endif
	wxLog::SetActiveTarget(logger);
#elif OS_WIN32
	// Log errors to a log file.
	wxString confLocation = wxStandardPaths::Get().GetUserDataDir();
	if(!wxDirExists(confLocation)) wxMkdir(confLocation);
	wxString logFile = confLocation + wxT("/log.txt");

	logOut.open(logFile.mb_str(), ios::out | ios::app);
	logOut << endl;
	logOut << endl;
	logger = new wxLogStream(&logOut);

	wxLog::SetActiveTarget(logger);
#ifdef DEBUG // Log to UI as well.
	wxLog* extraLogger = new wxLogWindow(NULL, _("DroidPad debug log output"));
	wxLog::SetVerbose(true);
#endif
#endif
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

	if(runSetup) {
		return dpSetup(*this);
	}
	if(runRemove) {
		return dpRemove(*this);
	}

	DroidFrame *frame = new DroidFrame;
	frame->Show(true);
	SetTopWindow(frame);
	return true;
}

int DroidApp::OnExit() {
	logOut.close();
	return 0;
}

void DroidApp::onDFFinish() {
	ExitMainLoop();
}

void DroidApp::OnInitCmdLine(wxCmdLineParser& parser) {
	parser.SetDesc(dp_cmdLineDesc);
	parser.SetSwitchChars(wxT("-"));
}
bool DroidApp::OnCmdLineParsed(wxCmdLineParser& parser) {
	runSetup = parser.Found(wxT("s"));
	runRemove = parser.Found(wxT("u"));

	if(!parser.Found(wxT("n"))) { // If user didn't request no root, check.
		requestNecessaryPermissions();
	}

	return true;
}

void DroidApp::requestNecessaryPermissions() {
#ifdef OS_LINUX
	LOGV("Checking if permissions needed...");
	if(geteuid() == 0) return; // We should be fine if sudo / su.
	if(dpinput_checkUInput() == CHECKRESULT_PERMS) { // Checking permissions
		LOGV("Getting elevated permissions...");
		// Execute correct SU process - guessing it
		wxChar* session = wxGetenv(wxT("DESKTOP_SESSION"));
		wxString suCmd;
		if(session == NULL) suCmd = wxT("gksu");
		else {
			wxString sSession = session;
			if(sSession == wxT("ubuntu") || sSession.find(wxT("gnome"))) {
				suCmd = wxT("gksu");
			} else if(sSession.find(wxT("kde")) || sSession.find(wxT("kwin"))) {
				suCmd = wxT("kdesu");
			}
		}

		wxString thisProcess = argv[0];

		if(suCmd == wxT("gksu")) {
			execlp(
					suCmd.mb_str(),
					(const char *)suCmd.mb_str(),
					"-m",
					"DroidPad must be run with elevated privileges.",
					(const char *)thisProcess.mb_str(),
					(char *)0);
		}
	}
#endif // No windows probably.
}
