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
#include "secureConnection.hpp"

using namespace droidpad;
using namespace droidpad::decode;
using namespace std;

#include <iostream>
#include <cmath>
#include <openssl/err.h>
#include "log.hpp"
#include "hexdump.h"
#include <wx/intl.h>
#include "data.hpp"

#ifdef DEBUG
#define SSL_PRINT_ERRORS() { if(ERR_peek_error()) fprintf(stderr, "SSL Error at %s:%d:\n", __FILE__, __LINE__); ERR_print_errors_fp(stderr); }
#else
#define SSL_PRINT_ERRORS() { ERR_print_errors_fp(stderr); }
#endif

#define THROW_NULL(x, msg) if ((x)==NULL) throw runtime_error(msg);
#define THROW_ERR(err,s, msg) if ((err)==-1) { perror(s); throw runtime_error(msg); }
#define THROW_SSL(err, msg) if ((err)==-1) { SSL_PRINT_ERRORS(); throw runtime_error(msg); }

#define RETURN_NULL(x, val) if ((x)==NULL) return (val);
#define RETURN_ERR(err,s, val) if ((err)==-1) { perror(s); return (val); }
#define RETURN_SSL(err, val) if ((err)==-1) { SSL_PRINT_ERRORS(); return (val); }

SecureConnection::SecureConnection(wxString host, uint16_t port) throw (runtime_error) :
	host(host),
	port(wxString::Format(wxT("%d"), port))
{
	LOGVwx(wxString::Format(_("Connecting on %s:%d"), host.c_str(), port));
	// SSL_library_init called in droidApp
	tlsMethod = TLSv1_server_method();
	ctx = SSL_CTX_new(tlsMethod);
	THROW_NULL(ctx, "Couldn't initialise SSL");

	// Set up identity and PSK auth
	SSL_CTX_use_psk_identity_hint(ctx, Data::computerUuidString().c_str());
	SSL_CTX_set_psk_server_callback(ctx, &SecureConnection::checkPsk);

	// Connection setup
	netBio = BIO_new(BIO_s_connect());
	BIO_set_conn_hostname(netBio, host.char_str());
	BIO_set_conn_port(netBio, wxString::Format(wxT("%d"), (int)port).char_str());
}

SecureConnection::~SecureConnection() {
	Stop();
	SSL_CTX_free(ctx);
}

int SecureConnection::Start() throw (runtime_error) {
	int err;
	LOGV("SSL: Connecting");
	if(BIO_do_connect(netBio) != 1) {
		// Could not connect
		SSL_PRINT_ERRORS();
		return START_NETERROR;
	}
	LOGV("SSL: Connected");

	// Connect to BIO socket

	// Initialise SSL connection
	LOGV("SSL: Initialising");
	ssl = SSL_new(ctx);
	RETURN_NULL(ssl, START_INITERROR);
	SSL_set_bio(ssl, netBio, netBio);
	
	LOGV("SSL: Accepting");
	err = SSL_accept(ssl);
	if(err == -1) { 
		SSL_shutdown(ssl);
		SSL_free(ssl);
		ssl = NULL;
		RETURN_SSL(err, START_AUTHERROR);
	}

	LOGV("SSL: Connection created");

	try {
		StartCommunication();
	} catch (runtime_error e) {
		cout << e.what() << endl;
		Stop();
		return START_HANDSHAKEERROR;
	}
}

// Stops the connection, whatever stage it is at. If the connection is currently open, will send a stop message, then disconnect.
void SecureConnection::Stop() throw (std::runtime_error) {
	if(ssl) {
		LOGV("Sending stop message");
		BinaryServerMessage stop;
		stop.sig.setCmd();
		stop.msg = CMD_STOP;
		SSL_write(ssl, &stop, sizeof(BinaryServerMessage));
		LOGV("Stop message sent");

		SSL_shutdown(ssl);
		SSL_free(ssl);
		ssl = NULL;
		SSL_PRINT_ERRORS();
	}
	if(netBio) {
		// BIO is closed by SSL_free
		netBio = NULL;
	}
}

void SecureConnection::StartCommunication() throw(std::runtime_error) {
	GetMode();
}

const ModeSetting &SecureConnection::GetMode() throw (std::runtime_error) {
	if(mode.initialised) return mode;
	decode::BinarySignature sig = getSignature();
	if(!sig.isConnectionInfo())
		throw runtime_error("Received a mode which didn't start with DINF header");
	// Read header, reconstructing from two pieces
	if(!ssl) throw runtime_error("SSL connection lost");
	const size_t amt = sizeof(decode::BinaryConnectionInfo);
	char buf[amt];
	memcpy(buf, &sig, sizeof(decode::BinarySignature));
	if(SSL_read(ssl, buf + sizeof(decode::BinarySignature), amt - sizeof(BinarySignature)) < 1)
		throw runtime_error("Failed to read full info from stream");

	const decode::BinaryConnectionInfo info = decode::getBinaryConnectionInfo(buf);

	// Set mode
	mode.type = info.modeType;
	mode.numRawAxes = info.rawDevices;
	mode.numAxes = info.axes;
	mode.numButtons = info.buttons;
	mode.supportsBinary = true;
	mode.initialised = true;
	return mode;
}
const decode::DPJSData SecureConnection::GetData() throw (std::runtime_error) {
	decode::BinarySignature sig = getSignature();
	if(!sig.isBinaryHeader())
		return DPJSData();
	char *headerBuf = (char*)malloc(sizeof(RawBinaryHeader));
	// Copy header in place
	memcpy(headerBuf, &sig, sizeof(BinarySignature));

	// Read rest of header
	if(!ssl) throw runtime_error("SSL connection lost");
	if(SSL_read(ssl, headerBuf + sizeof(BinarySignature),
				sizeof(RawBinaryHeader) - sizeof(BinarySignature)) < 1)
		throw runtime_error("Failed to read full header from stream");

	RawBinaryHeader header = getBinaryHeader(headerBuf);
	free(headerBuf);

	size_t elementsSize = sizeof(RawBinaryElement) * header.numElements;
	char *elementsBuf = (char*)malloc(elementsSize);
	if(SSL_read(ssl, elementsBuf, elementsSize) < 1)
		throw runtime_error("Failed to read elements from stream");

	vector<RawBinaryElement> elems;
	for(const char *elem = elementsBuf; elem < elementsBuf + elementsSize; elem += sizeof(RawBinaryElement)) {
		elems.push_back(getBinaryElement(elem));
	}

	free(elementsBuf);

	return getBinaryData(header, elems);
}

decode::BinarySignature SecureConnection::getSignature() throw(std::runtime_error) {
	decode::BinarySignature sig;
	if(ssl) { // Read header
		int read;
		if((read = SSL_read(ssl, &sig, sizeof(decode::BinarySignature))) < 1) {
			LOGW("WARNING: Signature not read from stream");
			// TODO: Should I stop here?
			Stop();
		}
	} else throw runtime_error("SSL not open");
	return sig;
}

// Checks to see if a set of creds exists, and sends the PSK to SSL if it does.
// Sends a fake PSK if not.
unsigned int SecureConnection::checkPsk(SSL *ssl, const char *identity, unsigned char *psk, unsigned int max_psk_len) {
	for(vector<Credentials>::iterator it = CredentialStore::begin();
			it != CredentialStore::end(); ++it) {
		if(it->deviceIdString().compare((string) identity) == 0) {
			// Known connection found
			memcpy(psk, it->psk.c_str(), std::min((int)max_psk_len, (int)it->psk.size()));
			return it->psk.size();
		}
	}
	LOGV("Failed to authenticate PSK");
	return 0; // Indicates failure
}
