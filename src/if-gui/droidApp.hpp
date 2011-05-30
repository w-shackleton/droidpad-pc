#ifndef _DROIDAPP_H
#define _DROIDAPP_H

#include <wx/app.h>
#include <wx/log.h>

class DroidApp : public wxApp
{
	        virtual bool OnInit();

		wxLog *logger;

	public:

		void onDFFinish();
};

DECLARE_APP(DroidApp)

#endif
