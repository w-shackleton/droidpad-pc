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
#include"droidFrame.hpp"

#include "droidApp.hpp"

#include <wx/xrc/xmlres.h>
#include <wx/icon.h>
#include <wx/msgdlg.h>

#include "customHost.hpp"
#include "reorderDialog.hpp"
#include "devicePair.hpp"
#include "axisTweak.hpp"
#include "about.hpp"
#include "log.hpp"
#ifdef OS_WIN32
#include "updater.hpp"
#endif

BEGIN_EVENT_TABLE(DroidFrame, wxFrame)
	EVT_BUTTON(XRCID("buttonStart"), DroidFrame::OnStart)
	EVT_BUTTON(XRCID("buttonStop"), DroidFrame::OnStop)

	EVT_MENU(XRCID("menuFileStart"), DroidFrame::OnStart)
	EVT_MENU(XRCID("menuFileStop"), DroidFrame::OnStop)

	EVT_MENU(XRCID("menuFilePair"), DroidFrame::OnPairDevice)
	EVT_MENU(XRCID("menuFileQuit"), DroidFrame::OnMenuClose)

	EVT_MENU(XRCID("menuAdjustReorder"), DroidFrame::ReorderAxes)
	EVT_MENU(XRCID("menuAdjustTweak"), DroidFrame::TweakAxes)

	EVT_MENU(XRCID("menuHelpAbout"), DroidFrame::OnAbout)
	EVT_MENU(XRCID("menuHelpGettingStarted"), DroidFrame::OnGettingStarted)

	EVT_LISTBOX(XRCID("devicesList"), DroidFrame::OnListBox)
	EVT_LISTBOX_DCLICK(XRCID("devicesList"), DroidFrame::OnStart)

	EVT_CLOSE(DroidFrame::OnClose)
END_EVENT_TABLE()

using namespace droidpad;
using namespace droidpad::threads;

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
	LOADXRC(statusText,	statusText,	wxStaticText);
	LOADXRC(devicesList,	devListBox,	wxListBox);

	buttonStop->Disable();

	devices = new DeviceManager(*this);
	PushEventHandler(devices);
	devices->RequestUpdates();
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

void DroidFrame::OnMenuClose(wxCommandEvent& event)
{
	wxCloseEvent e;
	OnClose(e);
}

void DroidFrame::OnStart(wxCommandEvent& event)
{
	wxLogInfo(wxT("Starting DP"));
	int deviceNum = listToDeviceMap[devListBox->GetSelection()];
	devices->Start(deviceNum);
	buttonStart->Disable();
}

void DroidFrame::OnStop(wxCommandEvent& event)
{
	wxLogInfo(wxT("Stopping DP"));
	buttonStop->Disable();
	devices->Stop();
}

void DroidFrame::ReorderAxes(wxCommandEvent& event) {
	ReorderDialog dlg(this);
	dlg.ShowModal();
}

void DroidFrame::TweakAxes(wxCommandEvent& event) {
	AxisTweak dlg(this);
	int y = dlg.GetSize().GetHeight();
	dlg.SetSizeHints(400, y);
	dlg.ShowModal();
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
	for(int i = 0; i < list.size(); i++) {
		wxString label;
		switch(list[i].type) {
			case DEVICE_CUSTOMHOST:
				label = list[i].usbId;
				break;
			default:
				label = list[i].usbId + wxT(": ") + list[i].name;
				break;
		}
		if(devListBox->FindString(label) == wxNOT_FOUND) {
			AndroidDevice *clientData = new AndroidDevice(list[i]);
			int listPos = devListBox->Append(label, clientData); // Does wx take ownership here? Hope so.
			listToDeviceMap[listPos] = i;
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

void DroidFrame::threadStarted()
{
	buttonStop->Enable();
	buttonStart->Disable();
}

void DroidFrame::threadError(wxString failReason)
{
	wxMessageBox(_("DroidPad couldn't start - ") + failReason);
}

void DroidFrame::setStatusText(wxString text) {
	statusText->SetLabel(text);
}

void DroidFrame::threadStopped()
{
	buttonStart->Enable();
	buttonStop->Disable();
}


bool DroidFrame::customiseDevice(AndroidDevice *device) {
	CustomHost dlg(this, device);
	switch(device->type) {
		case DEVICE_CUSTOMHOST:
			if(dlg.ShowModal() != wxID_OK) {
				return false;
			}
			break;
		default:
			break;
	}
	return true;
}

void DroidFrame::OnAbout(wxCommandEvent& event) {
	About dlg(this);
	dlg.ShowModal();
}

void DroidFrame::OnGettingStarted(wxCommandEvent& event) {
	help = new Help;
	help->Show(true);
}

void DroidFrame::OnPairDevice(wxCommandEvent& event) {
	DevicePair pair(this, Data::computerUuid);
	pair.ShowModal();
}

// Update code
#ifdef OS_WIN32
void DroidFrame::updatesAvailable(std::vector<UpdateInfo> updates, std::vector<UpdateInfo> latest, bool userRequest) {
	LOGV("Checking for new DP version..");
	if(latest.size() == 1 && // New update available
		wxMessageBox(_("There is a new version of DroidPad available.\nWould you like to download it?"), _("New version available"), wxYES_NO, this) == wxYES) {
		devices->StartUpdate(latest[0]);
	}
}

void DroidFrame::updateStarted() {
	if(!updater) updater = new UpdateDisplay(this, *devices);
	updater->Show();
}
void DroidFrame::updateProgress(int bytesDone, int bytesTotal) {
	if(updater) updater->SetProgress(bytesDone, bytesTotal);
}
void DroidFrame::updateFailed() {
	if(updater) updater->Close();
	wxMessageDialog dlg(this, _("Failed to download DroidPad update.\nSee log files for more information."), _("Download failed"), wxOK);
	dlg.ShowModal();
}
void DroidFrame::updateCompleted(bool wasCancel) {
	if(updater) updater->Close();
	updater = NULL;
	if(!wasCancel) {
		wxCloseEvent e;
		OnClose(e);
	}
}
#else
// Dummy implementations for OSes which don't need this updater.
void DroidFrame::updatesAvailable(std::vector<UpdateInfo> updates, std::vector<UpdateInfo> latest, bool userRequest) { }

void DroidFrame::updateStarted() { }
void DroidFrame::updateProgress(int bytesDone, int bytesTotal) { }
void DroidFrame::updateFailed() { }
void DroidFrame::updateCompleted(bool wasCancel) { }
#endif
