#ifndef _DROIDFRAME_H
#define _DROIDFRAME_H

#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/listbox.h>

#include <map>

#include "lib/deviceManager.hpp"
#include "lib/droidpadCallbacks.hpp"

class DroidFrame : public wxFrame, public droidpad::DroidPadCallbacks
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
		
		wxListBox *devListBox;

		wxStaticText *statusText;

		enum
		{

		};

	private:
		void init();

		void handleXMLError(wxString name);

		void OnDevicesRefresh(wxCommandEvent& event);
		void OnStart(wxCommandEvent& event);
		void OnStop(wxCommandEvent& event);

		void OnClose(wxCloseEvent& event);
		void OnClose(wxCommandEvent& event);

		void OnListBox(wxCommandEvent& event);

		droidpad::DeviceManager *devices;

		std::map<int, int>listToDeviceMap;

	// Callbacks
	public:
		void dpInitComplete(bool complete);
		void dpCloseComplete();

		void dpNewDeviceList(droidpad::AndroidDeviceList &list);

		virtual void threadStarted();
		virtual void threadError(wxString failReason);
		virtual void threadStopped();

};

#endif
