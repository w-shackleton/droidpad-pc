#include "axisTweak.hpp"

#include <wx/xrc/xmlres.h>
#include <wx/msgdlg.h>
#include "log.hpp"
// #include <wx/msgdlg.h>

using namespace droidpad;
using namespace std;

#ifdef __WXMSW__
#define _FRAME_ICON wxT("icon.xpm")
#else
#define _FRAME_ICON wxT("iconlarge.xpm")
#endif

#define FRAME_TITLE "Tweak Axes"

#define LOADXRC(_xml, _name, _type) _name = XRCCTRL(*this, #_xml, _type); \
					    if(_name == NULL) handleXMLError(wxT(#_xml));

template <class T>
inline void trim(T &num, T min, T max) {
	if(num < min) num = min;
	if(num > max) num = max;
}

BEGIN_EVENT_TABLE(AxisTweak, wxDialog)
	EVT_BUTTON(XRCID("okButton"), AxisTweak::onDone)
	EVT_BUTTON(XRCID("cancelButton"), AxisTweak::onCancel)

	EVT_COMBOBOX(XRCID("tiltSelection"), AxisTweak::updateFields)

	EVT_COMMAND_SCROLL(XRCID("tiltAngleSlider"), AxisTweak::onTiltUpdate)
	EVT_COMMAND_SCROLL(XRCID("tiltGammaSlider"), AxisTweak::onTiltUpdate)
END_EVENT_TABLE()


AxisTweak::AxisTweak(wxWindow *parent) {
	if(!wxXmlResource::Get()->LoadDialog(this, parent, wxT("axisTweak"))) {
		LOGE("Failed to load dialog XML.");
	}

	SetIcon(wxIcon(Data::getFilePath(_FRAME_ICON), wxBITMAP_TYPE_XPM));

	LOADXRC(tiltSelection, tiltSelection, wxComboBox);
	LOADXRC(tiltAngleSlider, tiltAngleSlider, wxSlider);
	LOADXRC(tiltGammaSlider, tiltGammaSlider, wxSlider);

	tweaks = Data::tweaks;

	wxCommandEvent tmp;
	updateFields(tmp);
}

void AxisTweak::handleXMLError(wxString name)
{
	wxMessageDialog(this, wxString::Format(_("Error loading layout while loading \"%s\".\nPlease report this as a bug."), name.c_str()), _("Error loading layout"), wxOK | wxICON_EXCLAMATION).ShowModal();
}

void AxisTweak::onCancel(wxCommandEvent &evt) {
	EndModal(0);
}

void AxisTweak::onDone(wxCommandEvent &evt) {
	EndModal(0);
}

void AxisTweak::updateFields(wxCommandEvent &evt) {
	// Update tilt tweaks
	int tiltIx = tiltSelection->GetCurrentSelection();
	trim(tiltIx, 0, 1);
	tiltAngleSlider->SetValue(tweaks.tilt[tiltIx].totalAngle);
	tiltGammaSlider->SetValue(tweaks.tilt[tiltIx].gamma);
}

void AxisTweak::onTiltUpdate(wxScrollEvent &evt) {
}
