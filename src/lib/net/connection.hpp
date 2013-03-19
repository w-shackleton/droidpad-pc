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

	// Interface for a connection of some type
	class Connection {
		public:
			virtual int Start() = 0;
			inline virtual ~Connection() { }

			virtual const ModeSetting &GetMode() throw (std::runtime_error) = 0;
			virtual const decode::DPJSData GetData() throw (std::runtime_error) = 0;

			virtual void RequestBinary() throw (std::runtime_error) = 0;

			enum {
				START_SUCCESS = 0,
				START_NETERROR,
				START_INITERROR,
				START_AUTHERROR,
			};
	};

	class DPConnection : private wxSocketClient, public Connection {
		public:
			DPConnection(wxString host, uint16_t port);
			virtual ~DPConnection();

			virtual int Start();

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

			/**
			 * Returns a string containing n bytes of data from the network
			 */
			std::string GetBytes(size_t n);

			ModeSetting mode;
		public:
			virtual const ModeSetting &GetMode() throw (std::runtime_error);
			virtual const decode::DPJSData GetData() throw (std::runtime_error);

			virtual void RequestBinary() throw (std::runtime_error);
	};
};

#endif
