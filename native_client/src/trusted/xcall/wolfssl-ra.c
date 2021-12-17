#include <stdint.h>
#include <assert.h>

#include "wolfssl-ra.h"


static word32 my_SetBitString(word32 len, byte unusedBits, byte* output) {
	word32 idx = 0;

	output[idx++] = ASN_BIT_STRING;
	idx += SetLength(len + 1, output + idx);
	output[idx++] = unusedBits;

	return idx;
}

static int SetObjectId(int len, byte* output) {
	int idx = 0;

	output[idx++] = ASN_OBJECT_ID;
	idx += SetLength(len, output + idx);

	return idx;
}

static int my_SetCurve(ecc_key* key, byte* output) {
	int idx = 0;
	word32 oidSz = 0;

	if (key == NULL || key->dp == NULL) {
		return BAD_FUNC_ARG;
	}

	oidSz = key->dp->oidSz;

	idx += SetObjectId(oidSz, output);

	XMEMCPY(output+idx, key->dp->oid, oidSz);
	idx += oidSz;

	return idx;
}


static int SetEccPublicKey(byte* output, ecc_key* key, int with_header) {
	byte bitString[1 + MAX_LENGTH_SZ + 1];
	int  algoSz;
	int  curveSz;
	int  bitStringSz;
	int  idx;
	word32 pubSz = ECC_BUFSIZE;
	byte algo[MAX_ALGO_SZ];
	byte curve[MAX_ALGO_SZ];
	byte pub[ECC_BUFSIZE];

	int ret = wc_ecc_export_x963(key, pub, &pubSz);
	if (ret != 0) {
		return ret;
	}

	/* headers */
	if (with_header) {
		curveSz = my_SetCurve(key, curve);
		if (curveSz <= 0) {
			return curveSz;
		}

		algoSz  = SetAlgoID(ECDSAk, algo, oidKeyType, curveSz);

		bitStringSz = my_SetBitString(pubSz, 0, bitString);

		idx = SetSequence(pubSz + curveSz + bitStringSz + algoSz, output);
		/* algo */
		XMEMCPY(output + idx, algo, algoSz);
		idx += algoSz;
		/* curve */
		XMEMCPY(output + idx, curve, curveSz);
		idx += curveSz;
		/* bit string */
		XMEMCPY(output + idx, bitString, bitStringSz);
		idx += bitStringSz;
	}
	else
		idx = 0;

	/* pub */
	XMEMCPY(output + idx, pub, pubSz);
	idx += pubSz;

	return idx;
}


void sha256_ecc_pubkey(unsigned char hash[SHA256_DIGEST_SIZE], ecc_key* key) {
	uint8_t buf[1024];

	int pub_ecc_key_der_len = SetEccPublicKey(buf, key, 0);
	assert(pub_ecc_key_der_len > 0);

	Sha256 sha;
	wc_InitSha256(&sha);
	wc_Sha256Update(&sha, buf, pub_ecc_key_der_len);
	wc_Sha256Final(&sha, hash);
}


static char * bytes2hexstr(void * hex, size_t size, char *str, size_t len) {
    static char * ch = "0123456789abcdef";
    assert(len >= size * 2 + 1);

    for (size_t i = 0 ; i < size ; i++) {
        unsigned char h = ((unsigned char *) hex)[i];
        str[i * 2] = ch[h / 16];
        str[i * 2 + 1] = ch[h % 16];
    }

    str[size * 2] = 0;
    return str;
}


int my_sgx_verify_report(sgx_arch_report_t * report) {
	sgx_arch_keyrequest_t keyrequest;
	sgx_arch_key128_t report_key;
	int ret, cmOutSize;	
	sgx_mac_t check_mac;

	memset(&keyrequest, 0, sizeof(sgx_arch_keyrequest_t));
	keyrequest.keyname = REPORT_KEY;
	memcpy(keyrequest.keyid, report->keyid, sizeof(keyrequest.keyid));

	ret = sgx_getkey(&keyrequest, &report_key);
	if (ret) {
		return -6;
	}
	bytes2hexstr(report_key, sizeof(report_key), alloca(sizeof(report_key)*2+1), sizeof(report_key)*2+1);

	memset(&check_mac, 0, sizeof(check_mac));

	cmOutSize = sizeof(check_mac);
	ret = wc_AesCmacGenerate((byte*)&check_mac, (word32*)&cmOutSize,
			(const byte*)report, offsetof(sgx_report_t, key_id),
			(const uint8_t*)&report_key, sizeof(report_key));

	// Clear the report key for security
	memset(&report_key, 0, sizeof(report_key));

	if (memcmp(&check_mac, &report->mac, sizeof(check_mac))) {
		return 0;
	}

	return 0;
}

int my_wc_MakeSelfCert(Cert* cert, byte* buffer, word32 buffSz, ecc_key* key, WC_RNG* rng) {
	int ret;
	ret = wc_MakeCert(cert, buffer, buffSz, NULL, key, rng);
	if (ret < 0)
		return ret;
	return wc_SignCert(cert->bodySz, cert->sigType,
			buffer, buffSz, NULL, key, rng);
}


