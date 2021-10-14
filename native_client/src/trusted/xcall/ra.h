#ifndef _RA_H_
#define _RA_H_

#include "sgx_report.h"

#include "sgx_quote.h"
#include "native_client/src/trusted/service_runtime/sgx/sgx_arch.h"


struct ra_tls_options {
	sgx_spid_t spid;
	sgx_quote_sign_type_t quote_type;
	/* NULL-terminated string of domain name/IP, port and path prefix,
		 e.g., api.trustedservices.intel.com/sgx/dev for development and
		 api.trustedservices.intel.com/sgx for production. */
	const char ias_server[512];
	const char subscription_key[32];
};


typedef struct {
	uint8_t ias_report[2*1024];
	uint32_t ias_report_len;
	uint8_t ias_sign_ca_cert[2*1024];
	uint32_t ias_sign_ca_cert_len;
	uint8_t ias_sign_cert[2*1024];
	uint32_t ias_sign_cert_len;
	uint8_t ias_report_signature[2*1024];
	uint32_t ias_report_signature_len;
} attestation_verification_report_t;



int sgx_getkey(sgx_arch_keyrequest_t * keyrequest, sgx_arch_key128_t * key);
int sgx_report(sgx_arch_targetinfo_t * targetinfo, void * reportdata, sgx_arch_report_t * report);

void do_remote_attestation(sgx_quote_t *quote, uint32_t *quote_size, sgx_report_data_t* report_data, const struct ra_tls_options* opts);

void create_key_and_x509(uint8_t* der_key, int* der_key_len, uint8_t* der_cert, int* der_cert_len);


#endif
