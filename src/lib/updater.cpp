#include "updater.hpp"

#include <wx/url.h>
#include <wx/stream.h>
#include <wx/sstream.h>
#include <wx/tokenzr.h>

#include <vector>

#include "deviceManager.hpp"
#include "log.hpp"
#include "data.hpp"

#ifdef OS_WIN32
#define UPDATE_FILE wxT("http://digitalsquid.co.uk/download/droidpad/win-update.upd")
#endif

using namespace droidpad::threads;
using namespace std;

Updater::Updater(DeviceManager &parent, bool userRequest) :
	parent(parent),
	userRequest(userRequest)
{
}

void *Updater::Entry() {
	// Check update file.
	// TODO: Check type of errors thrown here.
	wxURL url(UPDATE_FILE);
	if(url.GetError() == wxURL_NOERR) {
		wxString updateData;
		wxInputStream *in = url.GetInputStream();
		if(in && in->IsOk()) {
			wxStringOutputStream updateStream(&updateData);
			in->Read(updateStream);
			parseUpdates(updateData);
		} else {
			LOGE("Couldn't download update file; error downloading.");
			delete in;
			return NULL;
		}
		delete in;
	} else {
		LOGE("Couldn't download update file; error parsing.");
		return NULL;
	}
}

void Updater::parseUpdates(wxString updates) {
	LOGV("Parsing updates.");
	wxStringTokenizer lines(updates);
	vector<UpdateInfo> availableVersions;
	while(lines.HasMoreTokens()) {
		wxStringTokenizer sections(lines.GetNextToken());
		UpdateInfo version;
		if(sections.CountTokens() < 6) {
			LOGW("WARNING: Incorrectly formatted update line.");
			continue;
		}
		long code;
		sections.GetNextToken().ToLong(&code);
		version.versionCode = code;
		version.versionName = sections.GetNextToken();
		version.url = sections.GetNextToken();
		version.name = sections.GetNextToken();
		version.comment = sections.GetNextToken();
		version.md5 = sections.GetNextToken();
		availableVersions.push_back(version);
	}
}
