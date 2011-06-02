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

	class DPData {
		public:
			std::vector<uint16_t> axes;
			std::vector<bool> buttons;
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
			const DPData GetData();
	};
};

#endif
