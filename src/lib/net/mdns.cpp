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
#include "mdns.hpp"

#include <wx/object.h>
#include <wx/thread.h>
#include <wx/intl.h>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <iostream>

#ifdef OS_WIN32
#include <windows.h>
#include <ws2tcpip.h>
#else
// unix socket includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "1035.h"

#include <b64/base64.hpp>
#include "log.hpp"

using namespace std;
using namespace droidpad::mdns;

// Converts a wxWidgets string of either wchar* or char*
#define CSTR_TO_WSTR(_cstr) ((const wxChar*) wxString(_cstr, wxConvUTF8).c_str())

MDNS::MDNS(const wxString& what, int type) :
	callbacks(NULL)
{
	w = what;
	t = type;
}

void MDNS::start()
{
	mdnsd d;
	struct message m;
	unsigned long int ip;
	unsigned short int port;
	struct timeval *tv;
	fd_set fds;
	SOCKET s;
	exit = false;

	d = mdnsd_new(1,1000);

	if((s = msock()) == 0) 
	{ 
		wxLogError(_("mDNS: Can't create socket: %s\n"), CSTR_TO_WSTR(strerror(errno)));
		exit = true;
	}

	// register query(w,t) at mdnsd d, submit our address for callback ans()
	mdnsd_query(d, w.char_str(), t, ans, this);

	bool unexpectedExit = false;

	while(!exit)
	{
		if(callbacks != NULL) callbacks->cycle();
		tv = mdnsd_sleep(d);
		FD_ZERO(&fds);
		FD_SET(s,&fds);
		select(s+1,&fds,0,0,tv);

		// receive
		if(FD_ISSET(s,&fds))
		{
			while(recvm(&m, s, &ip, &port) > 0)
				mdnsd_in(d, &m, ip, port);
		}

		// send
		while(mdnsd_out(d,&m,&ip,&port))
			if(!sendm(&m, s, ip, port))
			{
				exit = true;
				unexpectedExit = true;
				break;
			}
	}

	mdnsd_shutdown(d);

	if(unexpectedExit) {
		exit = false;
		while(!exit)
#ifdef OS_WIN32
			Sleep(100);
#else // Unix / any other sensible OS?
			usleep(100*1000);
#endif
	}

	mdnsd_free(d);

#ifdef OS_WIN32
	closesocket(s);
#else
	close(s);
#endif 
}




bool MDNS::sendm(struct message* m, SOCKET s, unsigned long int ip, unsigned short int port)
{
	struct sockaddr_in to;

	memset(&to, '\0', sizeof(to));

	to.sin_family = AF_INET;
	to.sin_port = port;
	to.sin_addr.s_addr = ip;

	if(sendto(s, (char*)message_packet(m), message_packet_len(m), 0,(struct sockaddr *)&to,sizeof(struct sockaddr_in)) != message_packet_len(m))  
	{ 
		wxLogError(_("mDNS: Can't write to socket: %s\n"), CSTR_TO_WSTR(strerror(errno)));
		return false;
	}

	return true;
}





int MDNS::recvm(struct message* m, SOCKET s, unsigned long int *ip, unsigned short int *port) 
{
	struct sockaddr_in from;
	int bsize;
	static unsigned char buf[MAX_PACKET_LEN];
#ifdef __WIN32__
	int ssize  = sizeof(struct sockaddr_in);
#else
	socklen_t ssize  = sizeof(struct sockaddr_in);
#endif


	if((bsize = recvfrom(s, (char*)buf, MAX_PACKET_LEN, 0, (struct sockaddr*)&from, &ssize)) > 0)
	{
		memset(m, '\0', sizeof(struct message));
		message_parse(m,buf);
		*ip = (unsigned long int)from.sin_addr.s_addr;
		*port = from.sin_port;
		return bsize;
	}

#ifdef __WIN32__
	if(bsize < 0 && WSAGetLastError() != WSAEWOULDBLOCK) 
#else
		if(bsize < 0 && errno != EAGAIN)
#endif
		{
			wxLogMessage(_("mDNS: Can't read from socket: %s\n"), CSTR_TO_WSTR(strerror(errno)));
			return bsize;
		}

	return 0;
}







int MDNS::ans(mdnsda a, void *arg)
{
	MDNS *moi = (MDNS*)arg;

	moi->processResult(a);

	return 1;
}



// create a multicast 224.0.0.251:5353 socket, windows or unix style
SOCKET MDNS::msock() const
{
	SOCKET s;
	int flag = 1;
	char ttl = 255; // Used to set multicast TTL
	int ittl = 255;

	// this is our local address
	struct sockaddr_in addrLocal;
	memset(&addrLocal, '\0', sizeof(addrLocal));
	addrLocal.sin_family = AF_INET;
	addrLocal.sin_port = htons(5353);
	addrLocal.sin_addr.s_addr = 0;	

	// and this the multicast destination
	struct ip_mreq	ipmr;
	ipmr.imr_multiaddr.s_addr = inet_addr("224.0.0.251");
	ipmr.imr_interface.s_addr = htonl(INADDR_ANY);

#ifdef __WIN32__
	// winsock startup
	WORD			wVersionRequested;
	WSADATA			wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	if(WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		WSACleanup();
		wxLogError(_("mDNS: Failed to start winsock"));
		return 0;
	}
#endif


	// Create a new socket
#ifdef __WIN32__
	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
#else
	if((s = socket(AF_INET,SOCK_DGRAM, IPPROTO_UDP)) < 0)
#endif
		return 0;


	// set to reuse address
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(flag));

	// Bind socket to port, returns 0 on success
	if(bind(s, (struct sockaddr*) &addrLocal, sizeof(addrLocal))) 
	{ 
#ifdef __WIN32__
		closesocket(s);
#else
		close(s);
#endif 
		return 0;
	}

	// Set the multicast ttl
	setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
	setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ittl, sizeof(ittl));

	// Add socket to be a member of the multicast group
	setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&ipmr, sizeof(ipmr));

	// set to nonblock
#ifdef __WIN32__
	unsigned long block=1;
	ioctlsocket(s, FIONBIO, &block);
#else
	flag =  fcntl(s, F_GETFL, 0);
	flag |= O_NONBLOCK;
	fcntl(s, F_SETFL, flag);
#endif

	// whooaa, that's it
	return s;
}

DeviceFinder::DeviceFinder(Callbacks *callbacks) :
	MDNS(wxT("_droidpad._tcp.local."), QTYPE_PTR)
{
	this->callbacks = callbacks;
}

int DeviceFinder::processResult(mdnsda a)
{
	if(a->rdname == NULL) return 0; // Error?
	wxString fullName = wxString((char*)a->rdname, wxConvUTF8); 
	string charFullName = string((char*)a->rdname);

	if(a->ttl == 0)
	{
		// entry was expired
		devices.erase(fullName);
		return 1;
	}

	InfoFinder infoFinder(fullName);
	infoFinder.start();
	IPFinder ipFinder(infoFinder.result);
	ipFinder.start();

	// Save stuff

	Device device;

	device.fullName = fullName;

	int beginPos = charFullName.find(':');
	if(beginPos == string::npos) beginPos = 0;
	int endPos = charFullName.find('.');
	if(endPos == string::npos) endPos = fullName.size() - 1;
	string b64 = charFullName.substr(beginPos + 1, endPos - beginPos - 1);

	device.deviceDescription = wxString(base64_decode(b64).c_str(), wxConvUTF8);
	size_t pos;
	if((pos = device.deviceDescription.find(wxT("secure:"))) == string::npos) {
		device.secureSupported = true;
		device.deviceDescription.Remove(pos, 7);
	}

	struct in_addr ip;
	ip.s_addr =  ntohl(ipFinder.result);
	device.ip = wxString(inet_ntoa(ip), wxConvUTF8); 
	device.port = infoFinder.port;
	// For the time being use the next port along.
	// TODO: Perhaps change once worked out how to receive mDNS properties
	device.securePort = device.port + 1;

	devices[fullName] = device;
	if(callbacks != NULL) callbacks->onData();
}


InfoFinder::InfoFinder(wxString fullName) :
	MDNS(fullName, QTYPE_SRV)
{
}

int InfoFinder::processResult(mdnsda a)
{
	exit = true;
	result = wxString((char*)a->rdname, wxConvUTF8);
	port = a->srv.port;
}


IPFinder::IPFinder(wxString fullName) :
	MDNS(fullName, QTYPE_A)
{
}

int IPFinder::processResult(mdnsda a)
{
	exit = true;
	result = a->ip;
}

