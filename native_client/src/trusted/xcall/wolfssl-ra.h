#ifndef _WOLFSSL_RA_H_
#define _WOLFSSL_RA_H

#include "native_client/src/trusted/wolfssl/asn.h"
#include "native_client/src/trusted/wolfssl/error-crypt.h"
#include "native_client/src/trusted/wolfssl/sha256.h"
#include "native_client/src/trusted/wolfssl/ecc.h"
#include "native_client/src/trusted/wolfssl/cmac.h"

#include "native_client/src/trusted/xcall/ra.h"
#include "native_client/src/trusted/service_runtime/sgx/sgx_arch.h"

#include "sgx_report.h"



void sha256_ecc_pubkey(unsigned char hash[SHA256_DIGEST_SIZE], ecc_key* key);
int my_sgx_verify_report(sgx_arch_report_t * report);
int my_wc_MakeSelfCert(Cert* cert, byte* buffer, word32 buffSz, ecc_key* key, WC_RNG* rng);

#endif
