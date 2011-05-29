#include "mdns.hpp"

#include <wx/object.h>
#include <wx/thread.h>
#include <wx/intl.h>

#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <iostream>

#ifdef OS_WIN32
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

using namespace std;
using namespace droidpad::mdns;

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
		printf(wxString(_("Can't create socket: %s\n")).mb_str(), strerror(errno));
		exit = true;
	}

	// register query(w,t) at mdnsd d, submit our address for callback ans()
	mdnsd_query(d, w.char_str(), t, ans, this);

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
				break;
			}
	}

	mdnsd_shutdown(d);
	mdnsd_free(d);

#ifdef DEBUG
	cerr << "scanthread querying " << w.char_str() << " Entry() end.";
#endif

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
		printf(wxString(_("Can't write to socket: %s\n")).mb_str(), strerror(errno));
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
			printf("Can't read from socket %d: %s\n", errno,strerror(errno));
			return bsize;
		}

	return 0;
}







int MDNS::ans(mdnsda a, void *arg)
{
	MDNS *moi = (MDNS*)arg;

	moi->processResult(a);

#ifdef DEBUG
	cerr << "--->" << endl;
	cerr << "key is: " << key.char_str() << endl; 
	cerr << moi->p->results[key].name.char_str() << endl;
	cerr << inet_ntoa((in_addr&) moi->p->results[key].ip) << endl;
	cerr << moi->p->results[key].port << endl;
	cerr << "<---" << endl;
#endif

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
		printf("Failed to start winsock\r\n");
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
#ifdef DEBUG
	cout << "Device found: " << device.deviceDescription.mb_str() << endl;
#endif

	struct in_addr ip;
	ip.s_addr =  ntohl(ipFinder.result);
	device.ip = wxString(inet_ntoa(ip), wxConvUTF8); 

	device.port = a->srv.port;

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

