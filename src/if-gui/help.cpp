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
#include "help.hpp"

BEGIN_EVENT_TABLE(Help, wxFrame)
	EVT_CLOSE(Help::OnClose)
END_EVENT_TABLE()

#include "data.hpp"
#include "proc.hpp"
#include "log.hpp"

#ifdef __WXMSW__
#define _FRAME_ICON wxT("icon.xpm")
#else
#define _FRAME_ICON wxT("iconlarge.xpm")
#endif

#include <wx/icon.h>
#include <wx/button.h>
#include <wx/msgdlg.h>

using namespace droidpad;
using namespace std;

HelpWindow *currentWindow;

#define FRAME_TITLE "DroidPad - Getting Started"

Help::Help() :
	wxFrame(NULL, -1, _(FRAME_TITLE), wxDefaultPosition, wxSize(640, 480))
{
	SetIcon(wxIcon(wxString(Data::getFilePath(_FRAME_ICON).c_str(), wxConvUTF8), wxBITMAP_TYPE_XPM));

	wxBoxSizer *parentSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(parentSizer);

	web = new HelpWindow(this);
	parentSizer->Add(web, 1, wxEXPAND | wxALL);
	web->LoadPage(Data::getFilePath(wxT("docs/intro.zip")) + wxT("#zip:intro/start.html"));
}

void Help::OnClose(wxCloseEvent& event) {
	Destroy();
}

wxHtmlOpeningStatus HelpWindow::OnOpeningURL(wxHtmlURLType type,const wxString& url, wxString *redirect) const {
	if(url == wxT("back:///")) {
		LOGV("User clicked back");
		currentWindow->HistoryBack();
		return wxHTML_BLOCK;
	} else if(url.StartsWith(wxT("http://")) || url.StartsWith(wxT("https://"))) {
		LOGV("User clicked webpage");
		openWebpage(string(url.mb_str()));
		return wxHTML_BLOCK;
	}
	cout << "Opening " << url.mb_str() << endl;
	return wxHTML_OPEN;
}
