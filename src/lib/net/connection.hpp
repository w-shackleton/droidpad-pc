#ifndef DP_CONNECTION_H
#define DP_CONNECTION_H

#include <wx/socket.h>
#include <stdint.h>

#define CONN_BUFFER_SIZE 128

namespace droidpad {
	class DPConnection : private wxSocketClient {
		public:
			DPConnection(wxString host, uint16_t port);
			~DPConnection();

			bool Start();
			void GetData();

		private:
			wxIPV4address addr;

			wxString inData;
			char buffer[CONN_BUFFER_SIZE];

			void ParseFromNet();
	};
};

#endif
