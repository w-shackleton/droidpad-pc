#include "updater.hpp"

#include <wx/url.h>
#include <wx/stream.h>
#include <wx/sstream.h>
#include <wx/tokenzr.h>

#include <vector>

#include "deviceManager.hpp"
#include "log.hpp"
#include "data.hpp"
#include "events.hpp"

#include <wx/wfstream.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

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
	wxStringTokenizer lines(updates, wxT("\r\n"));
	vector<UpdateInfo> availableVersions;
	while(lines.HasMoreTokens()) {
		wxStringTokenizer sections(lines.GetNextToken(), wxT(";"));
		UpdateInfo version;
		if(sections.CountTokens() < 6) {
			LOGW("Incorrectly formatted update line.");
			LOGWwx(wxString::Format(wxT("%d"), sections.CountTokens()));
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

	vector<UpdateInfo> updateToInstall;

	UpdateInfo highestUpdate;
	for(vector<UpdateInfo>::iterator it = availableVersions.begin(); it != availableVersions.end(); it++) {
		UpdateInfo update = *it;
		if(update.versionCode > highestUpdate.versionCode)
			highestUpdate = update;
	}

	// If newer version
	if(highestUpdate.versionCode > VERSION_CODE) {
		updateToInstall.push_back(highestUpdate);
	}

	UpdatesNotification notif(availableVersions, updateToInstall, userRequest);
	parent.AddPendingEvent(notif);
}


UpdateDl::UpdateDl(DeviceManager &parent, UpdateInfo &info) :
	parent(parent),
	info(info),
	running(true)
{
}

void *UpdateDl::Entry() {
	LOGV("Starting DroidPad download...");
	LOGVwx(wxString::Format(wxT("Downloading from %s"), (const wxChar *)info.url.c_str()));
	wxString tempDir = wxStandardPaths::Get().GetTempDir();
	wxFileName fileName = wxFileName(tempDir, wxT("droidpad-update.exe"));
	LOGVwx(wxString::Format(wxT("Saving to %s"), (const wxChar *) fileName.GetFullPath().c_str()));

	wxFileOutputStream *out = new wxFileOutputStream(fileName.GetFullPath());

	{
		DlStatus status(dpDL_STARTED);
		parent.AddPendingEvent(status);
	}

	// If this is set to false then the downloaded file will be assumed invalid.
	bool fileOk = true;

	// Begin DL
	wxURL url(info.url);
	if(url.GetError() == wxURL_NOERR) {
		wxInputStream *in = url.GetInputStream();
		if(in && in->IsOk()) {
#define BUFFER_SIZE 65536
			char *buffer = new char[BUFFER_SIZE];
			size_t byteCount;
			const size_t totalBytes = in->GetSize();
			while(running && !in->Eof()) { // Main DL loop
				if(!(in->IsOk() && out->IsOk())) {
					LOGW("I/O Not reporting OK in DL loop");
					fileOk = false;
					break;
				}
				size_t bytesRead = in->Read(buffer, BUFFER_SIZE).LastRead();
				byteCount += bytesRead;
				size_t bytesWritten = 0;
				while(bytesWritten < bytesRead) { // Write the remaining data.
					bytesWritten += out->Write(buffer + bytesWritten, bytesRead).LastWrite();
				}
				{
					DlStatus status(dpDL_PROGRESS, byteCount, totalBytes);
					parent.AddPendingEvent(status);
				}
			}
			delete buffer;
		} else {
			DlStatus status(dpDL_FAILED);
			parent.AddPendingEvent(status);
			LOGW("Failed to create download stream");
			fileOk = false;
		}
		if(in) delete in;
	} else {
		DlStatus status(dpDL_FAILED);
		parent.AddPendingEvent(status);
		LOGW("Failed to parse URL");
		fileOk = false;
	}

	if(out) delete out;
}
