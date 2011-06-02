#ifndef DP_CONNECTION_H
#define DP_CONNECTION_H

#include <wx/socket.h>
#include <string>
#include <stdint.h>

#define CONN_BUFFER_SIZE 128

#define MODE_JS 1
#define MODE_MOUSE 2
#define MODE_SLIDE 3

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
			const ModeSetting &GetMode();
	};
};

#endif
