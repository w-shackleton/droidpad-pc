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

#define SSL_NULL(x) if ((x)==NULL) ERR_print_errors_fp(stderr);
#define SSL_ERROR(err) if ((err)==-1) { ERR_print_errors_fp(stderr); }

SecureConnection::SecureConnection(wxString host, uint16_t port) throw (runtime_error) :
	host(host),
	port(wxString::Format(wxT("%d"), port))
{
	tlsMethod = TLSv1_server_method();
	ctx = SSL_CTX_new(tlsMethod);
	SSL_NULL(ctx);
	if(ctx == NULL) throw runtime_error("Couldn't initialise SSL");
}

SecureConnection::~SecureConnection() {
}

bool SecureConnection::Start() throw (runtime_error) {
}
