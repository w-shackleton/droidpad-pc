#include "connection.hpp"

using namespace droidpad;
using namespace std;

DPConnection::DPConnection(wxString host, uint16_t port) :
	wxSocketClient(wxSOCKET_BLOCK)
{
	addr.Hostname(host);
	addr.Service(port);
	
	SetTimeout(10);
}

DPConnection::~DPConnection() {
	Close();
}

bool DPConnection::Start()
{
	return Connect(addr, true);
}

wxString DPConnection::GetLine()
{
	while(inData.Find('\n') == wxNOT_FOUND) {
		ParseFromNet();
	}
	return inData.Left(inData.Find('\n'));
}

void DPConnection::ParseFromNet()
{
	Read(buffer, CONN_BUFFER_SIZE);
	inData += wxString(buffer, wxConvUTF8);
}
