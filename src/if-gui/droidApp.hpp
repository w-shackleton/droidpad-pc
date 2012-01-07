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
#ifndef _DROIDAPP_H
#define _DROIDAPP_H

#include <fstream>
#include <wx/app.h>
#include <wx/log.h>

#include <wx/wxprec.h>
#include <wx/cmdline.h>

class DroidApp : public wxApp
{
	        virtual bool OnInit();

		virtual int OnExit();

		wxLog *logger;
		std::ofstream logOut;

		bool runSetup;
		bool runRemove;

		void requestNecessaryPermissions();

	public:
		virtual void OnInitCmdLine(wxCmdLineParser& parser);
		virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

		void onDFFinish();
};

DECLARE_APP(DroidApp)

static const wxCmdLineEntryDesc dp_cmdLineDesc [] =
{
	{ wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), wxT("displays help on the command line parameters"),
		wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
	{ wxCMD_LINE_SWITCH, wxT("s"), wxT("setup"), wxT("sets up DroidPad components"),
		wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL  },
	{ wxCMD_LINE_SWITCH, wxT("u"), wxT("remove"), wxT("removes DroidPad components"),
		wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL  },
	{ wxCMD_LINE_SWITCH, wxT("n"), wxT("no-root"), wxT("don't ask for root privileges"),
		wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL  },

	{ wxCMD_LINE_NONE }
};

#endif
