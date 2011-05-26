#ifndef _DATA_H
#define _DATA_H

#include <iostream>
#include "config.h"

#include <wx/string.h>

#define STD_TO_WX_STRING(_str) wxString(_str.c_str(), wxConvUTF8)

namespace Misc
{
	class Data
	{
		public:
			static bool initialise();
			static wxString datadir;
			static wxString getFilePath(wxString file);

			static int ip1, ip2, ip3, ip4, port;
			static wxString serial;
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
