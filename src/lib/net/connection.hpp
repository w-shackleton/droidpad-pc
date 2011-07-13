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

#include "types.hpp"

#define CONN_BUFFER_SIZE 128

namespace droidpad {
	class ModeSetting {
		public:
			bool initialised;
			int type;
			int numRawAxes;
			int numAxes;
			int numButtons;

			ModeSetting();
	};

	/**
	 * Raw data returned from connection. Is castable to the other data types,
	 * which contain data from it.
	 */
	class DPJSData {
		public:
			std::vector<int> axes;
			std::vector<bool> buttons;
	};

	class DPMouseData {
		public:
			DPMouseData(const DPJSData& rawData);

			/**
			 * scrollDelta: down is positive.
			 */
			int x, y, scrollDelta;
			bool bLeft, bMiddle, bRight;
	};

	class DPSlideData {
		public:
			DPSlideData(const DPJSData& rawData);
	};

	class DPConnection : private wxSocketClient {
		public:
			DPConnection(wxString host, uint16_t port);
			virtual ~DPConnection();

			bool Start();

		private:
			wxIPV4address addr;

			wxString inData;
			char buffer[CONN_BUFFER_SIZE];

			void SendMessage(std::string message);

			wxString GetLine();
			void ParseFromNet();

			ModeSetting mode;

		public:
			const ModeSetting &GetMode() throw (std::runtime_error);
			const DPJSData GetData();
	};
};

#endif
