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
#elif OS_WIN32
#include <windows.h>
#endif

using namespace droidpad;
using namespace std;

#define BUFFER_SIZE 128

#ifdef OS_UNIX
// I still can't believe how infinitely much easier this is in Unix.
string droidpad::runProcess(string cmd, string args) {
	FILE* pipe = popen((cmd + " " + args).c_str(), "r");
	if (!pipe) throw "Couldn't run adb";
	char buffer[BUFFER_SIZE];
	string result = "";
	while(!feof(pipe)) {
		if(fgets(buffer, BUFFER_SIZE, pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);
	return result;
}

#elif OS_WIN32
// http://ubuntuforums.org/showthread.php?t=610271
string droidpad::runProcess(string cmd, string args) {
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

	
	string output = "";
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

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return output;
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
#elif OS_WIN32
void droidpad::openWebpage(string url) {
	// TODO: Test
	ShellExecute(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
#endif
