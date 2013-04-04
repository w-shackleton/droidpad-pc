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
#ifndef DP_SECURE_CONNECTION_H
#define DP_SECURE_CONNECTION_H

#include <wx/string.h>
#include <openssl/ssl.h>
#include <stdint.h>
#include <stdexcept>

#include "net/connection.hpp"

namespace droidpad {
	class SecureConnection : public Connection {
		public:
			SecureConnection(AndroidDevice &device) throw (std::runtime_error);
			virtual ~SecureConnection();

			/**
			 * Starts the full handshake process, including TLS
			 */
			int Start() throw (std::runtime_error);
			void Stop() throw (std::runtime_error);

			virtual const ModeSetting &GetMode() throw (std::runtime_error);
			virtual const decode::DPJSData GetData() throw (std::runtime_error);

			// In this mode, binary comms is used all the time, so
			// no need to request it.
			inline virtual void RequestBinary() throw (std::runtime_error) { }

		private:
			wxString host, port, name;

			/**
			 * Once a TLS connection has been made,
			 * start the DroidPad loop etc.
			 */
			void StartCommunication() throw(std::runtime_error);

			/**
			 * Reads the signature from the input stream
			 */
			decode::BinarySignature getSignature() throw(std::runtime_error);

			// SSL stuff
			const SSL_METHOD *tlsMethod;
			SSL_CTX *ctx;
			SSL *ssl;
			BIO *netBio;

			static unsigned int checkPsk(SSL *ssl, const char *identity, unsigned char *psk, unsigned int max_psk_len);

			/**
			 * Initialises the static components of this class
			 */
			static inline void staticInitialise() {
				if(staticInitialised) return;
				staticInitialised = true;
				thisReferenceId = SSL_get_ex_new_index(0, (void*)"SecureConnection this reference", NULL, NULL, NULL);
			}
			static int thisReferenceId;
			static bool staticInitialised;
	};
}

#endif
