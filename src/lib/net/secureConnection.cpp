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
using namespace std;

#include <iostream>
#include <cmath>
#include <openssl/err.h>
#include "log.hpp"
#include <wx/intl.h>
#include "data.hpp"

#define THROW_NULL(x, msg) if ((x)==NULL) throw runtime_error(msg);
#define THROW_ERR(err,s, msg) if ((err)==-1) { perror(s); throw runtime_error(msg); }
#define THROW_SSL(err, msg) if ((err)==-1) { ERR_print_errors_fp(stderr); throw runtime_error(msg); }

#define RETURN_NULL(x, val) if ((x)==NULL) return (val);
#define RETURN_ERR(err,s, val) if ((err)==-1) { perror(s); return (val); }
#define RETURN_SSL(err, val) if ((err)==-1) { ERR_print_errors_fp(stderr); return (val); }

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
	if(netBio) BIO_free(netBio);
	SSL_CTX_free(ctx);
}

int SecureConnection::Start() throw (runtime_error) {
	int err;
	LOGV("SSL: Connecting");
	if(BIO_do_connect(netBio) != 1) {
		// Could not connect
		ERR_print_errors_fp(stderr);
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
	}
	RETURN_SSL(err, START_AUTHERROR);

	LOGV("SSL: Connection created");
}

// Stops the connection, whatever stage it is at. If the connection is currently open, will send a stop message, then disconnect.
void SecureConnection::Stop() throw (std::runtime_error) {
	if(ssl) {
		// TODO: Send STOP message
		SSL_shutdown(ssl);
		SSL_free(ssl);
		ssl = NULL;
		ERR_print_errors_fp(stderr);
	}
}

const ModeSetting &SecureConnection::GetMode() throw (std::runtime_error) {
}
const decode::DPJSData SecureConnection::GetData() throw (std::runtime_error) {
}

// Checks to see if a set of creds exists, and sends the PSK to SSL if it does.
// Sends a fake PSK if not.
unsigned int SecureConnection::checkPsk(SSL *ssl, const char *identity, unsigned char *psk, unsigned int max_psk_len) {
	for(vector<Credentials>::iterator it = CredentialStore::begin();
			it != CredentialStore::end(); ++it) {
		if((string) identity == it->deviceIdString()) {
			// Known connection found
			memcpy(psk, it->psk.c_str(), std::min((int)max_psk_len, (int)it->psk.size()));
		}
	}
	return 0; // Indicates failure
}
