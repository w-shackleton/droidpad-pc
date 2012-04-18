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
#ifndef DP_CONNECTION_H
#define DP_CONNECTION_H

#include <wx/socket.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <stdint.h>

#include "dataDecode.hpp"

// Small buffer to allow faster reading & less overflow?
#define CONN_BUFFER_SIZE 64

namespace droidpad {
	class ModeSetting {
		public:
			bool initialised;
			int type;
			int numRawAxes;
			int numAxes;
			int numButtons;

			bool supportsBinary;

			ModeSetting();
	};

	class DPConnection : private wxSocketClient {
		public:
			DPConnection(wxString host, uint16_t port);
			virtual ~DPConnection();

			bool Start();

		private:
			wxIPV4address addr;

			std::string inData;
			char buffer[CONN_BUFFER_SIZE];

			void SendMessage(std::string message);

			wxString GetLine() throw (std::runtime_error);
			bool ParseFromNet();

			/**
			 * Returns the first character from the buffer, to check if the data is binary or text
			 */
			char PeekChar() throw (std::runtime_error);

			ModeSetting mode;
		public:
			const ModeSetting &GetMode() throw (std::runtime_error);
			const decode::DPJSData GetData() throw (std::runtime_error);
	};
};

#endif
