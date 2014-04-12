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

#include "tlsTest.h"

#include <stdio.h>

#include <openssl/crypto.h>
#include <openssl/err.h>

#define RETURN_NULL(x) if ((x)==NULL) exit(1)
#define RETURN_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define RETURN_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(1); }

int main(int argc, char **argv) {
	if(argc < 3) {
		printf("Usage: %s <ip> <port>\n", argv[0]);
		return;
	}

	int err;

	// One-time setup
	SSL_library_init();
	SSL_load_error_strings();

	const SSL_METHOD *method = TLSv1_server_method();

	SSL_CTX *ctx = SSL_CTX_new(method);
	if(!ctx) {
		ERR_print_errors_fp(stderr);
		exit(1);
	};

	// Set up SSL stuff
	SSL_CTX_use_psk_identity_hint(ctx, "Server identity");
	SSL_CTX_set_psk_server_callback(ctx, &checkPsk);

	// Connection setup
	BIO *netBio = BIO_new(BIO_s_connect());
	BIO_set_conn_hostname(netBio, argv[1]);
	BIO_set_conn_port(netBio, argv[2]);

	if(BIO_do_connect(netBio) != 1) {
		// Could not connect
		ERR_print_errors_fp(stderr);
		exit(1);
	}

	// SSL instance
	SSL *ssl = SSL_new(ctx);
	RETURN_NULL(ssl);
	SSL_set_bio(ssl, netBio, netBio);
	
	err = SSL_accept(ssl);
	RETURN_SSL(err);

	printf("Connected via TLS using %s\n", SSL_get_cipher(ssl));
	char buf[1024];
	int read = SSL_read(ssl, buf, 1024);
	printf("Received: %.*s\n", read, buf);

	err = SSL_shutdown(ssl);
	RETURN_SSL(err);
	SSL_free(ssl);

	// One-time shutdown
	SSL_CTX_free(ctx);
	return 0;
}

unsigned int checkPsk(SSL *ssl, const char *identity, unsigned char *psk, unsigned int max_psk_len) {
	psk[0] = 'P';
	psk[1] = 'A';
	psk[2] = 'S';
	psk[3] = 'S';
	psk[4] = 'W';
	psk[5] = 'O';
	psk[6] = 'R';
	psk[7] = 'D';
	printf("Authentication being checked:\n"
			"Identity: %s\n"
			, identity);
	// Return length of PSK
	return 8;
}
