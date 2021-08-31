#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "native_client/src/shared/platform/nacl_log.h"
#include "native_client/src/shared/platform/sgx/cert.h"
#include "native_client/src/trusted/wolfssl/options.h"
#include "native_client/src/trusted/wolfssl/ssl.h"
#include "native_client/src/trusted/wolfssl/types.h"
#include "native_client/src/trusted/wolfssl/settings.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"
#include "native_client/src/trusted/wolfssl/coding.h"


int my_inet_pton(const char *src, struct in_addr *dst) {
	static const char digits[] = "0123456789";
	int saw_digit, octets, ch;
	u_char tmp[4], *tp;
	saw_digit = 0;
	octets = 0;
	*(tp = tmp) = 0;
	while ((ch = *src++) != '\0') {
		const char *pch;
		if ((pch = strchr(digits, ch)) != NULL) {
			u_int new = *tp * 10 + (pch - digits);
			if (new > 255)
				return (0);
			*tp = new;
			if (! saw_digit) {
				if (++octets > 4)
					return (0);
				saw_digit = 1;
			}
		} else if (ch == '.' && saw_digit) {
			if (octets == 4)
				return (0);
			*++tp = 0;
			saw_digit = 0;
		} else
			return (0);
	}
	if (octets < 4)
		return (0);
	memcpy(dst, tmp, 4);
	return (1);
}

int cert_verify_callback(int preverify, WOLFSSL_X509_STORE_CTX* store) {
	//TODO just pass
	return 1;
}


WOLFSSL *ssl_handshake(char *ip, int port, WOLFSSL_CTX* ctx) {

	int sockfd;
	int ret = 0;
    //WOLFSSL *ssl = NULL;
    
	struct sockaddr_in servAddr;

	WOLFSSL *ssl;
	//WOLFSSL_CTX* ctx;
	WOLFSSL_METHOD* method;

  NaClLog(3, "ssl_handshake");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;             
	servAddr.sin_port   = (((port & 0xff) << 8) | (port >> 8)); 

	if (my_inet_pton(ip, &servAddr.sin_addr) != 1) {
		printf("ERROR: invalid address\n");
		ret = -1;
		goto end;
	}

	if ((ret = wolfSSL_Init()) != WOLFSSL_SUCCESS) {
		printf("ERROR: Failed to initialize the library\n");
		goto end;
	}

	method = wolfTLSv1_2_client_method();
	if (method == NULL) {
		ret = -1;
		goto end;
	}

	if ((ctx = wolfSSL_CTX_new(method)) == NULL) {
		printf("ERROR: wolfSSL_CTXS_new failed\n");
		ret = -1;
		goto end;
	}

	if ((ret = wolfSSL_CTX_load_verify_buffer(ctx, ca_cert, sizeof(ca_cert), SSL_FILETYPE_ASN1)) != SSL_SUCCESS) {
		printf("ERROR: failed to load cert, please check the file.\n");
		goto ctx_cleanup;
	}

	wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, cert_verify_callback);

	if ((ssl = wolfSSL_new(ctx)) == NULL) {
		printf("ERROR: failed to create WOLFSSL object\n");
		ret = -1;
		goto ctx_cleanup;
	}
	
	/* Connect to the server */
	if ((sockfd = ocall_sock_connect(AF_INET, SOCK_STREAM, 0, (struct sockaddr*) &servAddr, sizeof(servAddr), NULL, NULL)) < 0) {
		printf("ERROR: failed to connect\n");
		ret = -1;
		goto cleanup;
	}

	if ((ret = wolfSSL_set_fd(ssl, sockfd)) != WOLFSSL_SUCCESS) {
		printf("ERROR: Failed to set the file descriptor\n");
		goto cleanup;
	}
	
	if ((ret = wolfSSL_connect(ssl)) != SSL_SUCCESS) {
		printf("ERROR: failed to connect to wolfSSL\n");
		goto cleanup;
	}

	return ssl;

cleanup:
	wolfSSL_free(ssl);
ctx_cleanup:
	wolfSSL_CTX_free(ctx);
	wolfSSL_Cleanup();
end:
	return ssl;
}
