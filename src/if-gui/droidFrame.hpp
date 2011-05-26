#ifndef _DROIDFRAME_H
#define _DROIDFRAME_H

#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/button.h>

class DroidFrame : public wxFrame
{
	public:
		DroidFrame ();
		~DroidFrame ();
		DECLARE_EVENT_TABLE()
	public:
	protected:
		wxPanel *panel;
		wxMenuBar *menuBar;

		wxButton *buttonStart, *buttonStop, *buttonDevicesRefresh;

		enum
		{

		};

	private:
		void init();

		void handleXMLError(wxString name);

		void OnDevicesRefresh(wxCommandEvent& event);
		void OnStart(wxCommandEvent& event);
		void OnStop(wxCommandEvent& event);
};

#endif
