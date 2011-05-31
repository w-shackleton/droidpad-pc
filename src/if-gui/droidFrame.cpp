#include"droidFrame.hpp"

#include "droidApp.hpp"

#include <wx/xrc/xmlres.h>
#include <wx/icon.h>
#include <wx/msgdlg.h>

BEGIN_EVENT_TABLE(DroidFrame, wxFrame)
	EVT_BUTTON(XRCID("devicesRefresh"), DroidFrame::OnDevicesRefresh)
	EVT_BUTTON(XRCID("buttonStart"), DroidFrame::OnStart)
	EVT_BUTTON(XRCID("buttonStop"), DroidFrame::OnStop)

	EVT_MENU(XRCID("menuFileStart"), DroidFrame::OnStart)
	EVT_MENU(XRCID("menuFileStop"), DroidFrame::OnStop)

	EVT_LISTBOX(XRCID("devicesList"), DroidFrame::OnListBox)
	EVT_LISTBOX_DCLICK(XRCID("devicesList"), DroidFrame::OnStart)

	EVT_CLOSE(DroidFrame::OnClose)
END_EVENT_TABLE()

using namespace droidpad;

#include <wx/log.h>

#include "data.hpp"

#ifdef __WXMSW__
#define _FRAME_ICON wxT("icon.xpm")
#else
#define _FRAME_ICON wxT("iconlarge.xpm")
#endif

#define FRAME_TITLE "DroidPad"

#define LOADXRC(_xml, _name, _type) _name = XRCCTRL(*this, #_xml, _type); \
					    if(_name == NULL) handleXMLError(wxT(#_xml));

DroidFrame::DroidFrame() :
	wxFrame(NULL, -1, _(FRAME_TITLE), wxDefaultPosition, wxSize(640, 480))
{
	init();
}

void DroidFrame::init()
{
	SetIcon(wxIcon(wxString(Data::getFilePath(_FRAME_ICON).c_str(), wxConvUTF8), wxBITMAP_TYPE_XPM));

	// Load XML
	SetMenuBar(wxXmlResource::Get()->LoadMenuBar(this, wxT("menu")));

	panel = wxXmlResource::Get()->LoadPanel(this, wxT("mainPanel"));
	panel->SetSizerAndFit(panel->GetSizer());
	wxSize sz;
	sz.SetHeight(200);
	sz.SetWidth(500);
	SetSize(sz);

	LOADXRC(buttonStart,	buttonStart,		wxButton);
	LOADXRC(buttonStop,	buttonStop,		wxButton);
	LOADXRC(devicesRefresh,	buttonDevicesRefresh,	wxButton);
	LOADXRC(statusText,	statusText,	wxStaticText);
	LOADXRC(devicesList,	devListBox,	wxListBox);

	devices = new DeviceManager(*this);
	PushEventHandler(devices);
}

void DroidFrame::handleXMLError(wxString name)
{
	wxMessageDialog(this, wxString::Format(_("Error loading layout while loading \"%s\".\nPlease report this as a bug."), name.c_str()), _("Error loading layout"), wxOK | wxICON_EXCLAMATION).ShowModal();
}

DroidFrame::~DroidFrame()
{
	delete devices;
	wxGetApp().onDFFinish();
}

void DroidFrame::OnClose(wxCloseEvent& event)
{
	devices->Close();
	panel->Disable();
}

void DroidFrame::OnDevicesRefresh(wxCommandEvent& event)
{
	wxLogInfo(wxT("Refreshing devices"));
}

void DroidFrame::OnStart(wxCommandEvent& event)
{
	wxLogInfo(wxT("Starting DP"));
}

void DroidFrame::OnStop(wxCommandEvent& event)
{
	wxLogInfo(wxT("Stopping DP"));
}

void DroidFrame::dpInitComplete(bool complete)
{
	if(!complete)
	{
		wxMessageDialog(this, _("Couldn't start DroidPad. Please check that it was installed correctly, or see log for more information"), _("Couldn't start DroidPad"), wxOK | wxICON_EXCLAMATION).ShowModal();
		Destroy();
		return;
	}
	devListBox->Enable();
	devListBox->Clear();
}

void DroidFrame::dpCloseComplete()
{
	RemoveEventHandler(devices);
	Destroy();
}

void DroidFrame::dpNewDeviceList(AndroidDeviceList &list)
{
	wxLogInfo(wxT("Devices found: %d"), list.size());
	for(int i = 0; i < list.size(); i++) {
		wxString label = list[i].usbId + wxT(": ") + list[i].name;
		if(devListBox->FindString(label) == wxNOT_FOUND) {
			AndroidDevice *clientData = new AndroidDevice(list[i]);
			devListBox->Append(label, clientData); // Does wx take ownership here? Hope so.
		}
	}

	for(int i = 0; i < devListBox->GetCount(); i++) {
		AndroidDevice *clientData = (AndroidDevice*) devListBox->GetClientData(i);
		bool matches = false;
		for(int j = 0; j < list.size(); j++) {
			if(list[j] == *clientData) matches = true;
		}
		if(!matches) devListBox->Delete(i);
	}
}

void DroidFrame::OnListBox(wxCommandEvent& event)
{
	if(devices->getState() == DP_STATE_STOPPED) {
		buttonStart->Enable(event.IsSelection());
	}
}

