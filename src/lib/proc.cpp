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
#include "proc.hpp"

#include <string>
#include <iostream>

#ifdef OS_UNIX
#include <stdio.h>
#include <unistd.h>

#include <wx/utils.h>
#include <wx/string.h>
#elif OS_WIN32
#include <windows.h>
#include "adminCheck.h"
#endif

#include "md5/md5.h"

using namespace droidpad;
using namespace std;

#define BUFFER_SIZE 128

#ifdef OS_UNIX
// I still can't believe how infinitely much easier this is in Unix.
string droidpad::runProcess(string cmd, string args) {
	string result;
	runProcess(result, cmd, args);
	return result;
}
int droidpad::runProcess(std::string &result, std::string cmd, std::string args = "") {
	FILE* pipe = popen((cmd + " " + args).c_str(), "r");
	if (!pipe) throw "Couldn't run adb";
	char buffer[BUFFER_SIZE];
	while(!feof(pipe)) {
		if(fgets(buffer, BUFFER_SIZE, pipe) != NULL)
			result += buffer;
	}
	return pclose(pipe);
}

#elif OS_WIN32
// http://ubuntuforums.org/showthread.php?t=610271
string droidpad::runProcess(string cmd, string args) {
	string result;
	runProcess(result, cmd, args);
	return result;
}
int droidpad::runProcess(string &output, string cmd, string args) {
	string errorToThrow = "Unknown Error";

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sattr;
	HANDLE readfh, writefh;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

	// Uncomment this if you want to hide the other app's
	// DOS window while it runs
	//si.wShowWindow = SW_HIDE;

	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

	// Initialize security attributes to allow the launched app to
	// inherit the caller's STDOUT, STDIN, and STDERR
	sattr.nLength = sizeof(SECURITY_ATTRIBUTES);
	sattr.lpSecurityDescriptor = 0;
	sattr.bInheritHandle = TRUE;

	// Get a pipe from which we read
	// output from the launched app
	if (!CreatePipe(&readfh, &si.hStdOutput, &sattr, 0))
	{
		// Error opening the pipe
		errorToThrow = "CreatePipe failed: " + GetLastError();
bad1:
		throw errorToThrow;
	}

	string cmdLine = "\"" + cmd + "\" " + args;
	const char *constC = cmdLine.c_str();
	char c[strlen(constC)];
	strcpy(c, constC);

	// Start the child process.
	if(!CreateProcess(	NULL,		// No module name (use command line)
				c,		// Command line
				NULL,		// Process handle not inheritable
				NULL,		// Thread handle not inheritable
				FALSE,		// Set handle inheritance to FALSE
				0,		// No creation flags
				NULL,		// Use parent's environment block
				NULL,		// Use parent's starting directory
				&si,		// Pointer to STARTUPINFO structure
				&pi)		// Pointer to PROCESS_INFORMATION structure
	  )
	{
		errorToThrow = "CreateProcess failed: " + GetLastError();
bad2:
		CloseHandle(readfh);
		CloseHandle(si.hStdOutput);
		goto bad1;
	}

	// Don't need the read access to these pipes
	CloseHandle(si.hStdInput);
	CloseHandle(si.hStdOutput);

	// We haven't yet read app's output
	si.dwFlags = 0;

	
	char buffer[BUFFER_SIZE];
	while(readfh) {
		DWORD numBytesRead = 0;
		if (!ReadFile(readfh, buffer, BUFFER_SIZE, &numBytesRead, 0) || !numBytesRead)
		{
			// If we aborted for any reason other than that the
			// app has closed that pipe, it's an
			// error. Otherwise, the program has finished its
			// output apparently
			if (GetLastError() != ERROR_BROKEN_PIPE && numBytesRead)
			{
				// An error reading the pipe
				printf("ReadFile failed for process output pipe (%d).\n", GetLastError());
			}

			// Close the pipe
			CloseHandle(readfh);
			readfh = 0;
		} else {
			output += buffer;
		}
	}

	// Close output pipe
	if (readfh) CloseHandle(readfh);

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	int result = -1;
	if(GetExitCodeProcess(pi.hProcess, (LPDWORD)&result)) {
		printf("GetExitCodeProcess failed (%d)\n", GetLastError());
	}

	// Close process and thread handles.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return result;
}

#endif

#ifdef OS_LINUX

#define BROWSER "x-www-browser"

void droidpad::openWebpage(string url) {
	// Good ol' fork and exec
	if(fork() == 0) {
		// TODO: Get this running as original user, not root!
		execlp(BROWSER, BROWSER, url.c_str(), (char *)0);
	}
}

void droidpad::forkProcess(string cmd) {
	if(fork() == 0) {
		execlp(cmd.c_str(), cmd.c_str(), (char *)0);
	}
}

#elif OS_WIN32
void droidpad::openWebpage(string url) {
	ShellExecute(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void droidpad::forkProcess(string cmd) {
	ShellExecute(NULL, "open", cmd.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
#endif


bool droidpad::md5check(std::string filePath, std::string checksum) {
	FILE *file;
	MD5_CTX context;
	int len;
	unsigned char buffer[1024], digest[16];

	if ((file = fopen(filePath.c_str(), "rb")) == NULL)
		return false;
	else {
		MD5Init (&context);
		while (len = fread (buffer, 1, 1024, file))
			MD5Update (&context, buffer, len);
		MD5Final (digest, &context);

		// Print out computed hash, compare to given hash.
		char md5String[33];
		sprintf(md5String, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
				digest[0], digest[1],
				digest[2], digest[3],
				digest[4], digest[5],
				digest[6], digest[7],
				digest[8], digest[9],
				digest[10], digest[11],
				digest[12], digest[13],
				digest[14], digest[15]); // Can't be bothered to do this properly
		return checksum == md5String;

		fclose (file);
	}
	return false;
}

#ifdef OS_LINUX
bool droidpad::isAdmin() {
	return geteuid() == 0; // 0 is root on linux. Effective ID so su / sudo work
}

void droidpad::runAsAdminAndExit(string cmd, string args) {
	// Execute correct SU process - guessing it
	wxChar* session = wxGetenv(wxT("DESKTOP_SESSION"));
	wxString suCmd;
	if(session == NULL) suCmd = wxT("gksu");
	else {
		wxString sSession = session;
		if(sSession == wxT("ubuntu") || sSession.find(wxT("gnome"))) {
			suCmd = wxT("gksu");
		} else if(sSession.find(wxT("kde")) || sSession.find(wxT("kwin"))) {
			suCmd = wxT("kdesu");
		}
	}

	string runCommand;
	if(args == "")
		runCommand = cmd;
	else
		runCommand = cmd + " " + args;

	if(suCmd == wxT("gksu")) {
		execlp(
				suCmd.mb_str(),
				(const char *)suCmd.mb_str(),
				"-m",
				"DroidPad must be run with elevated privileges.",
				(const char *)runCommand.c_str(),
				(char *)0);
	} else {
		execlp(
				suCmd.mb_str(),
				(const char *)suCmd.mb_str(),
				(const char *)runCommand.c_str(),
				(char *)0);
	}

}
#elif OS_WIN32
bool droidpad::isAdmin() {
	return isCurrentUserLocalAdministrator() == TRUE;
}

// NOTE / TODO: On MSW this doesn't stop, but this is handled when this is called.
void droidpad::runAsAdminAndExit(string cmd, string args) {
	ShellExecute(NULL, "runas", cmd.c_str(), args.c_str(), NULL, SW_SHOWNORMAL);
}
#endif
