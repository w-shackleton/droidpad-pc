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
#ifndef _DATA_H
#define _DATA_H

#include <iostream>
#include "config.h"

#include <wx/string.h>

#define STD_TO_WX_STRING(_str) wxString(_str.c_str(), wxConvUTF8)

namespace droidpad
{
	class Data
	{
		public:
			static bool initialise();
			static wxString datadir;
			static wxString getFilePath(wxString file);

			// Custom host and custom port
			static wxString host;
			static int port;

			// static wxString serial;
			static void savePreferences();
		protected:
			static wxString confLocation;
		private:
			Data(); // To stop initialising static class

	};

	wxString stringToUpper(wxString strToConvert);
	wxString stringToLower(wxString strToConvert);
}

#endif
