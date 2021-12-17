
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "sgx_utils.h"
#include "sgx_report.h"

#include "native_client/src/shared/platform/nacl_log.h"

//#include "native_client/src/trusted/wolfssl/settings.h"
//#include "native_client/src/trusted/wolfssl/options.h"
#include "native_client/src/trusted/wolfssl/ssl.h"
#include "native_client/src/trusted/wolfssl/asn.h"
#include "native_client/src/trusted/wolfssl/asn_public.h"
#include "native_client/src/trusted/wolfssl/sha256.h"
#include "native_client/src/trusted/wolfssl/ecc.h"
#include "native_client/src/trusted/wolfssl/error-crypt.h"
#include "native_client/src/trusted/wolfssl/coding.h"
#include "native_client/src/trusted/wolfssl/settings.h"

#include "native_client/src/trusted/xcall/ias_sign_ca_cert.h"
#include "native_client/src/trusted/xcall/ra.h"
#include "native_client/src/trusted/xcall/enclave_ocalls.h"
#include "native_client/src/trusted/xcall/wolfssl-ra.h"



static const char pem_marker_begin[] = "-----BEGIN CERTIFICATE-----";
static const char pem_marker_end[] = "-----END CERTIFICATE-----";


void *my_memchr(const void *bigptr, int ch, size_t length) {
	const char *big = (const char *)bigptr;
	size_t n;
	for (n = 0; n < length; n++)
		if (big[n] == ch)
			return (void *)&big[n];
	return NULL;
}


void *my_memmem(const void *haystack, size_t haystacklen, const void *needle, size_t needlelen) {
   char *bf = (char*) haystack, *pt = (char*) needle, *p = bf;
 
   while (needlelen <= (haystacklen - (p - bf)))
   {
      if (NULL != (p = my_memchr(p, (int)(*pt), haystacklen - (p - bf))))
      {
         if (0 == memcmp(p, needle, needlelen))
            return p;
         else
            ++p;
      }
      else
         break;
   }
   return NULL;
}


void do_remote_attestation(
 sgx_quote_t *quote, // out in sgxsdk header
 uint32_t *quote_size, // out
 sgx_report_data_t* report_data, // in
 const struct ra_tls_options* opts // in
) {
	unsigned char rdata[32] __attribute__ ((aligned (512)));

	NaClLog(4, "do_remote_attestation in\n");
	sgx_status_t status = 1;

	sgx_arch_targetinfo_t target_info = {0, };
	sgx_arch_report_t report = {0, };

	// uint8_t mrenclave[32] = {0xfd, 0xed , 0xd3 , 0xe6 , 0x75 , 0x2f , 0xc1 , 0xde , 0x7a , 0x36 , 0x97 , 0x20 , 0x91 , 0xf6 , 0x3d , 0x99 , 0x1b , 0xdf , 0xb7 , 0x8c , 0x35 , 0x5e , 0xcb , 0x5f , 0x1e , 0x8b , 0xba , 0xaa , 0xca , 0x1a , 0x20 , 0xf1};
	status = ocall_sgx_init_quote((sgx_target_info_t*)&target_info);
	assert(status == 0);
	NaClLog(4, "ocall_sgx_init_quote done\n");

	memset(rdata, 0, 32);
	memcpy(rdata, report_data->d, 32);
	
	sgx_report(&target_info, rdata, &report);
	NaClLog(4, "sgx_report done\n");


	status = my_sgx_verify_report(&report);
	NaClLog(4, "my_sgx_verify_report done\n");

	if (status)
		printf("ERROR: do_remote_attestation\n");

	status = ocall_remote_attestation(quote, quote_size, (sgx_report_t*)&report, opts);
	NaClLog(4, "ocall_remote_attestation done\n");

}

static void base64_encode(uint8_t *in, uint32_t in_len, uint8_t* out, uint32_t* out_len) {
	int ret;
	assert(*out_len >= (in_len + 3 - 1) / 3 * 4 + 1);
	ret = Base64_Encode_NoNl(in, in_len, out, out_len);
	assert(ret == 0);

}

void make_report_message(char *json, const sgx_quote_t* quote, const uint32_t quote_size) {
	const char json_template[] = "{\"isvEnclaveQuote\":\"";
	const char json_template2[] = "\"}";
	unsigned char quote_base64[1500];
	uint32_t quote_base64_len = sizeof(quote_base64);
	char *p = json;

	char *str = "POST /sgx/dev/attestation/v4/report HTTP/1.1\r\n"
		"Ocp-Apim-Subscription-Key: 86cfa3605c974cfea995bc296389cd34\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: 1510\r\n"
		"Host: api.trustedservices.intel.com\r\n"
		"\r\n";

	memset(quote_base64, 0, 1500);

	base64_encode((uint8_t*) quote, quote_size, quote_base64, &quote_base64_len);

	memcpy(p, str, strlen(str));
	p += strlen(str);
	memcpy(p, json_template, strlen(json_template));
	p += strlen(json_template);
	memcpy(p, quote_base64, quote_base64_len);
	p += quote_base64_len;
	memcpy(p, json_template2, strlen(json_template2));
	p += strlen(json_template);
	*p = '\0';
}

void parse_response_header(const char* header, size_t header_len, unsigned char* signature, const size_t signature_max_size, uint32_t* signature_size) {
	const char sig_tag[] = "X-IASReport-Signature: ";
	char* sig_begin = my_memmem(header, header_len, sig_tag, strlen(sig_tag));
	assert(sig_begin != NULL);
	sig_begin += strlen(sig_tag);
	char* sig_end = my_memmem(sig_begin, header_len - (sig_begin - header),	"\r\n",	strlen("\r\n"));
	assert(sig_end);

	assert((size_t) (sig_end - sig_begin) <= signature_max_size);
	memcpy(signature, sig_begin, sig_end - sig_begin);
	*signature_size = sig_end - sig_begin;
}


char misxdigit (unsigned char c) {
	if ( ( c >= (unsigned char)'0' && c <= (unsigned char)'9') ||
			( c >= (unsigned char)'a' && c <= (unsigned char)'f') ||
			( c >= (unsigned char)'A' && c <= (unsigned char)'F') )
		return 1;
	return 0;
}


void my_urldecode(char *dst, const char *src, int src_len, int *dst_len) {
	char a, b;
	while (src_len>0) {
		if ((*src == '%') &&
				((a = src[1]) && (b = src[2])) &&
				(misxdigit(a) && misxdigit(b))) {
			if (a >= 'a')
				a -= 'a'-'A';
			if (a >= 'A')
				a -= ('A' - 10);
			else
				a -= '0';
			if (b >= 'a')
				b -= 'a'-'A';
			if (b >= 'A')
				b -= ('A' - 10);
			else
				b -= '0';
			*dst++ = 16*a+b;
			src+=3;
			src_len -= 3;
			*dst_len += 1;
		} else if (*src == '+') {
			*dst++ = ' ';
			src++;
			src_len -= 1;
			*dst_len += 1;
		} else {
			*dst++ = *src++;
			src_len -= 1;
			*dst_len += 1;
		}
	}
	*dst++ = '\0';
}

/*
static void pem_to_base64_der(const char* pem, uint32_t pem_len, char* der, uint32_t* der_len, uint32_t der_max_len) {
	assert(XSTRNCMP((char*) pem, pem_marker_begin, strlen(pem_marker_begin)) == 0);
	assert(XSTRNCMP((char*) pem + pem_len - strlen(pem_marker_end),
				pem_marker_end, strlen(pem_marker_end)) == 0);

	uint32_t out_len = 0;
	const char* p = pem + strlen(pem_marker_begin);
	for (uint32_t i = 0;
			i < pem_len - strlen(pem_marker_begin) - strlen(pem_marker_end);
			++i) {
		if (p[i] == '\n') continue;
		assert(out_len <= der_max_len);
		der[out_len] = p[i];
		out_len++;
	}
	*der_len = out_len;
}
*/

void extract_certificates_from_response_header(const char* header, size_t header_len, attestation_verification_report_t* attn_report) {
	// Locate x-iasreport-signature HTTP header field in the response.
	const char response_header_name[] = "X-IASReport-Signing-Certificate: ";
	char *field_begin = my_memmem(header, header_len, response_header_name, strlen(response_header_name));
	assert(field_begin != NULL);
	field_begin += strlen(response_header_name);
	const char http_line_break[] = "\r\n";
	char *field_end = my_memmem(field_begin, header_len - (field_begin - header), http_line_break, strlen(http_line_break));
	size_t field_len = field_end - field_begin;

	// Remove urlencoding from x-iasreport-signing-certificate field.
	int unescaped_len = 0;
	char *unescaped = malloc(field_len);

	my_urldecode(unescaped, field_begin, field_len, &unescaped_len);
	
	unescaped_len = field_len;

	char* cert_begin = my_memmem(unescaped, unescaped_len, pem_marker_begin, strlen(pem_marker_begin));
	assert(cert_begin != NULL);
	char* cert_end = my_memmem(unescaped, unescaped_len, pem_marker_end, strlen(pem_marker_end));
	assert(cert_end != NULL);
	uint32_t cert_len = cert_end - cert_begin + strlen(pem_marker_end);

	assert(cert_len <= sizeof(attn_report->ias_sign_cert));
	memcpy(attn_report->ias_sign_cert, cert_begin, cert_len);
	attn_report->ias_sign_cert_len = cert_len;

	//pem_to_base64_der(cert_begin, cert_len,
	//		(char*) attn_report->ias_sign_cert,
	//		&attn_report->ias_sign_cert_len,
	//		sizeof(attn_report->ias_sign_cert));

	cert_begin = my_memmem(cert_end, unescaped_len - (cert_end - unescaped), pem_marker_begin, strlen(pem_marker_begin));
	assert(cert_begin != NULL);
	cert_end = my_memmem(cert_begin, unescaped_len - (cert_begin - unescaped), pem_marker_end, strlen(pem_marker_end));
	assert(cert_end != NULL);
	cert_len = cert_end - cert_begin + strlen(pem_marker_end);

	assert(cert_len <= sizeof(attn_report->ias_sign_ca_cert));
	memcpy((char*) attn_report->ias_sign_ca_cert, cert_begin, cert_len);
	attn_report->ias_sign_ca_cert_len = cert_len;
	
	//pem_to_base64_der(cert_begin, cert_len,
	//		(char*) attn_report->ias_sign_ca_cert,
	//		&attn_report->ias_sign_ca_cert_len,
	//		sizeof(attn_report->ias_sign_ca_cert));

	unescaped = NULL;
}




int cert_verify_callback_ra(int preverify, WOLFSSL_X509_STORE_CTX* store) {
	return 1;
}

void obtain_attestation_verification_report(const sgx_quote_t* quote, const uint32_t quote_size, attestation_verification_report_t* attn_report) {
	char json[2000];
	unsigned int ip[4];
	unsigned long my_inet_addr;
	struct sockaddr_in servAddr;
	int sockfd;
	int bytes, sent, received, total;//, message_size;
	char response[9128];
	char *message = json;
	int portno = 47873;
	int ret;
	const char *body_needle = "{\"id\":";
	char *body;
	int header_len;


	WOLFSSL_CTX* ctx;
	WOLFSSL* ssl;
	WOLFSSL_METHOD* method;


	ip[0] = 40;
	ip[1] = 87;
	ip[2] = 90;
	ip[3] = 88;

	my_inet_addr = ((ip[3]<<24) + (ip[2]<<16) + (ip[1]<<8) + ip[0]);

	make_report_message(json, quote, quote_size);
	NaClLog(4, "make_report_message done\n");
	printf("Report msg: %s\n", json);

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

  if ((ret = wolfSSL_CTX_load_verify_buffer(ctx, ias_ca_cert, sizeof(ias_ca_cert), SSL_FILETYPE_ASN1)) != SSL_SUCCESS) {
    printf("ERROR: failed to load cert, please check the file.\n");
    goto ctx_cleanup;
  }
  wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, cert_verify_callback_ra);

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = portno;
	servAddr.sin_addr.s_addr = my_inet_addr;

	sockfd = ocall_sock_connect(AF_INET,SOCK_STREAM,0,(struct sockaddr*)&servAddr,sizeof(servAddr),NULL,NULL);
	if (sockfd < 0){
		NaClLog(5, "sockfd returns negative");
		goto ctx_cleanup;
	}

	if ((ssl = wolfSSL_new(ctx)) == NULL) {
		goto cleanup;
	}

	wolfSSL_set_fd(ssl, sockfd);
		
	//ret = wolfSSL_set_cipher_list(ssl, "ECDHE-RSA-AES128-GCM-SHA256");

	printf("%s %d\n", __func__, __LINE__);
	
	
	NaClLog(4, "wolfSSL_connect\n");
	if ((ret = wolfSSL_connect(ssl)) < 0) {
		printf("ERROR - failed to wolfssl connect\n");
		goto cleanup;
	}
	NaClLog(4, "wolfSSL_connect done\n");

	total = strlen(message);
	sent = 0;
	do {
		bytes = wolfSSL_write(ssl, message+sent, total-sent);
		if (bytes < 0) {
			break;
		}
		if (bytes == 0)
			break;
		sent += bytes;
	} while(sent < total);


	memset(response, 0, sizeof(response));
	total = sizeof(response) - 1;
	received = 0;

	do {
		bytes = wolfSSL_read(ssl, response, sizeof(response)-1);
		if (bytes < 0) {
			break;
		}
		if (bytes == 0)
			break;
		received += bytes;
	} while(0); // received < total);
	if (received == total)
		goto cleanup;


	body = my_memmem(response, received, body_needle, strlen(body_needle));
	header_len = body - response;

	parse_response_header(response, header_len,	attn_report->ias_report_signature, sizeof(attn_report->ias_report_signature), &attn_report->ias_report_signature_len);
	attn_report->ias_report_len = sizeof(attn_report->ias_report);

	memcpy(attn_report->ias_report, body, received - header_len);
	attn_report->ias_report_len = received - header_len;

	//base64_encode((uint8_t*) body, received - header_len,	attn_report->ias_report, &attn_report->ias_report_len);
	extract_certificates_from_response_header(response, header_len, attn_report);


cleanup:
  wolfSSL_free(ssl);
ctx_cleanup:
  wolfSSL_CTX_free(ctx);
  wolfSSL_Cleanup();
end:
	return;
}

static void generate_x509(ecc_key* key, uint8_t* der_crt, int* der_crt_len, const attestation_verification_report_t* attn_report) {
	Cert crt;
	int certSz;
	RNG rng;

	wc_InitCert(&crt);
	NaClLog(4, "wc_InitCert done\n");

	strncpy(crt.subject.country, "KR", CTC_NAME_SIZE);
	strncpy(crt.subject.state, "", CTC_NAME_SIZE);
	strncpy(crt.subject.locality, "", CTC_NAME_SIZE);
	strncpy(crt.subject.org, "MMLAB", CTC_NAME_SIZE);
	strncpy(crt.subject.unit, "MMLAB", CTC_NAME_SIZE);
	strncpy(crt.subject.commonName, "SGX rocks!", CTC_NAME_SIZE);
	strncpy(crt.subject.email, "", CTC_NAME_SIZE);

	memcpy(crt.iasAttestationReport, attn_report->ias_report,
			attn_report->ias_report_len);
	crt.iasAttestationReportSz = attn_report->ias_report_len;

	memcpy(crt.iasSigCACert, attn_report->ias_sign_ca_cert,
			attn_report->ias_sign_ca_cert_len);
	crt.iasSigCACertSz = attn_report->ias_sign_ca_cert_len;

	memcpy(crt.iasSigCert, attn_report->ias_sign_cert,
			attn_report->ias_sign_cert_len);
	crt.iasSigCertSz = attn_report->ias_sign_cert_len;

	memcpy(crt.iasSig, attn_report->ias_report_signature,
			attn_report->ias_report_signature_len);
	crt.iasSigSz = attn_report->ias_report_signature_len;

	wc_InitRng(&rng);
	NaClLog(4, "wc_InitRng done\n");

	certSz = my_wc_MakeSelfCert(&crt, der_crt, *der_crt_len, key, &rng);
	NaClLog(4, "my_wc_MakeSelfCert done\n");
	assert(certSz > 0);
	*der_crt_len = certSz;
}

void create_key_and_x509(uint8_t* der_key, int* der_key_len, uint8_t* der_cert, int* der_cert_len) {
  ecc_key gen_key;
  RNG rng;
  int ret;
  int key_size = 32;
  uint8_t der[4096];
  int der_sz;

  uint32_t quote_size;
  sgx_quote_t *quote;
  attestation_verification_report_t attestation_report;
  struct ra_tls_options my_ra_tls_options = {
  	.spid = {{0x66,0x84,0x61,0x95,
		0x06,0x54,0x69,0x7A,
		0xFB,0x61,0x1A,0xF1,
		0x70,0x83,0xAA,0x69 }},
		.quote_type = SGX_UNLINKABLE_SIGNATURE,
		.ias_server = "api.trustedservices.intel.com/sgx/dev",
		.subscription_key = "0123456789abcdef0123456789abcdef"
	};

  NaClLog(3, "create_key_and_x509");

  wc_InitRng(&rng);
  ret  = wc_ecc_init(&gen_key);
  assert(ret == 0);

  ret = wc_ecc_make_key(&rng, key_size, &gen_key);
  assert(ret == 0);

  der_sz = wc_EccKeyToDer(&gen_key, der, sizeof(der));
  assert(der_sz >= 0);
  assert(der_sz <= (int) *der_key_len);

  *der_key_len = der_sz;
  memcpy(der_key, der, der_sz);
  NaClLog(4, "ECC key created\n");

  sgx_report_data_t report_data = {0, };
  sha256_ecc_pubkey(report_data.d, &gen_key);
  NaClLog(4, "sha_ecc_pubkey done\n");

  quote = malloc(1024);

  do_remote_attestation(quote, &quote_size, &report_data, &my_ra_tls_options);
  NaClLog(4, "attestation_verification_report_t done\n");

	obtain_attestation_verification_report (quote, quote_size, &attestation_report);
	NaClLog(4, "obtain_attestation_verification_report done\n");

	generate_x509(&gen_key, der_cert, der_cert_len, &attestation_report);
		
	NaClLog(4, "generate_x509 done\n");

}

