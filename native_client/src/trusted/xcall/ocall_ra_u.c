
#include <stdio.h>
#include "assert.h"
#include "string.h"
#include "stdlib.h"

#include "sgx_quote.h"
#include "sgx_uae_epid.h"

#include "native_client/src/include/nacl_compiler_annotations.h"
#include "native_client/src/trusted/xcall/ocall_types.h"


int sgx_ocall_sgx_init_quote(void * pms) {
	ms_ocall_sgx_init_quote_t * ms = (ms_ocall_sgx_init_quote_t *)pms;
	sgx_epid_group_id_t gid = {0, };
	int status = 0;

	status = sgx_init_quote(ms->ms_target_info, &gid);

	UNREFERENCED_PARAMETER(gid);

	return status;
}

int sgx_ocall_remote_attestation(void * pms) {

	ms_ocall_remote_attestation_t* ms = (ms_ocall_remote_attestation_t*) pms;

	int status = 0;

	status = sgx_calc_quote_size(NULL, 0, &ms->ms_quote_size);
	assert(SGX_SUCCESS == status);

	ms->ms_quote = (sgx_quote_t*)malloc(ms->ms_quote_size);
	memset(ms->ms_quote, 0, 1*ms->ms_quote_size);
	status = sgx_get_quote(ms->ms_report, ms->ms_quote_type, &ms->ms_spid, NULL, NULL, 0, NULL, ms->ms_quote, ms->ms_quote_size);
	assert(status == 0);

	ms->attestation_report = malloc(sizeof(attestation_verification_report_t));
	return status;
}
