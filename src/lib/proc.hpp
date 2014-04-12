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
#ifndef DP_PROC_H
#define DP_PROC_H

// This file does process starting and other things related to processes and executables.

#include <string>

namespace droidpad {
	std::string runProcess(std::string cmd, std::string args = "");

	int runProcess(std::string &result, std::string cmd, std::string args);

	void forkProcess(std::string cmd);

	// Returns true on success
	bool openWebpage(std::string url);

	/**
	 * verifies an md5. Returns true on success.
	 */
	bool md5check(std::string filePath, std::string checksum);

	// Returns true if the current user is an admin. Note that admin isn't actually required
	// on all platforms
	bool isAdmin();
	void runAsAdminAndExit(std::string cmd, std::string args = "");

#ifdef OS_LINUX
	// Tries to work out the UID of the user who originally started DroidPad.
	// This is used to start the browser etc, and the ID is guessed from some
	// environment variables.
	//
	// Returns the ID on success, or 0 on failure.
	// Failure can mean DP was run as root (id of 0), or couldn't find UID.
	int getOriginalUserID();
#endif
}

#endif
