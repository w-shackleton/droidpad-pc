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

#include "bootConf.hpp"

#include <wx/tokenzr.h>

#include "proc.hpp"
#include "log.hpp"

#define CMD "bcdedit.exe"

using namespace std;
using namespace droidpad;
using namespace droidpad::bootconf;

BootConfig bootconf::getCurrentConfig() {
	string _bcdOutput;
	int exitCode = runProcess(_bcdOutput, CMD, "");
	wxString bcdOutput(_bcdOutput.c_str(), wxConvUTF8);
	if(exitCode != 0) { // Something went wrong
		BootConfig conf;
		conf.success = false;
		return conf;
	}

	wxStringTokenizer lines(bcdOutput, wxT("\r\n"));

	// Find various lines
	BootConfig conf;
	conf.success = true;
	/**
	 * Counts how many times testsigning is found.
	 * if > 1, something odd is going on; report it.
	 */
	int testsignlines = 0;
	while(lines.HasMoreTokens()) {
		wxString line = lines.GetNextToken().Lower();
		if(line.Find(wxT("testsigning"))) {
			if(line.EndsWith(wxT("on")))
				conf.testsigning = true;
			testsignlines++;
		}
	}
	if(testsignlines > 1)
		LOGWwx(wxString::Format(wxT("Number of entries for testsigning was %d!"), testsignlines));

	return conf;
}
