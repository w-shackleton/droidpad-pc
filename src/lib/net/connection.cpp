#include "connection.hpp"

using namespace droidpad;

DPConnection::DPConnection(wxString host, uint16_t port) :
	wxSocketClient(wxSOCKET_BLOCK)
{
	addr.Hostname(host);
	addr.Service(port);
}

DPConnection::~DPConnection() {
	Close();
}

bool DPConnection::Start()
{
	return Connect(addr, true);
}

void DPConnection::GetData()
{
	while(inData.Find('\n') == wxNOT_FOUND) {
		ParseFromNet();
	}

	// Parse a new command
}

void DPConnection::ParseFromNet()
{
	Read(buffer, CONN_BUFFER_SIZE);
	inData += wxString(buffer, wxConvUTF8);
}
