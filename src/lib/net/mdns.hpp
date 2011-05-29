#ifndef DP_MDNS_H
#define DP_MDNS_H

// Work based off that of wxServDisc

#include <wx/event.h>
#include <wx/string.h>
#include <wx/hashmap.h>
#include <map>
#include <stdint.h>

#include "mdnsd.h"

#ifdef OS_WIN32
// mingw socket includes
#include <winsock2.h>
#endif

// just for convenience
// SOCKET is a unsigned int in win32!!
// but in unix we expect signed ints!!
#ifndef OS_WIN32
typedef int SOCKET;
#endif

namespace droidpad {
	namespace mdns {

		class Callbacks {
			public:
				virtual void cycle() = 0;
				virtual void onData() = 0;
		};

		/*
		   Generic scanner - C++ implementation of mDNSd
		 */
		class MDNS
		{
			private:
				wxString w;   // query what?
				int t;        // query type

				// create a multicast 224.0.0.251:5353 socket, windows or unix style
				SOCKET msock() const; 
				// send/receive message m
				bool sendm(struct message *m, SOCKET s, unsigned long int ip, unsigned short int port);
				int recvm(struct message *m, SOCKET s, unsigned long int *ip, unsigned short int *port);

				static int ans(mdnsda a, void *caller);

			protected:
				virtual int processResult(mdnsda a) = 0;

				Callbacks *callbacks;

				bool exit;
			public:
				MDNS(const wxString& what, int type);

				// execution starts here
				void start();
		};

		class Device
		{
			public:
				wxString fullName;
				wxString deviceDescription;
				wxString ip;
				uint16_t port;
		};

		class DeviceFinder : public MDNS
		{
			private:
				int processResult(mdnsda a);

			public:
				DeviceFinder(Callbacks *callbacks);

				inline void stop()
				{
					exit = true;
				}

				std::map<wxString, Device> devices;
		};

		class InfoFinder : public MDNS
		{
			private:
				int processResult(mdnsda a);

			public:
				InfoFinder(wxString fullName);

				wxString result;
				uint16_t port;
		};

		class IPFinder : public MDNS
		{
			private:
				int processResult(mdnsda a);

			public:
				IPFinder(wxString deviceName);

				int result;
		};
	}
}

#endif
