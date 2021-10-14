
#include "stdio.h"
#include "string.h"

#include "sgx_error.h"

#include "native_client/src/trusted/xcall/ocall_types.h"
#include "native_client/src/trusted/xcall/enclave_framework.h"


int ocall_sgx_init_quote(sgx_target_info_t *target_info) { //, sgx_report_data_t *report_data, sgx_report_t *report) {
	sgx_status_t status = SGX_SUCCESS;
	ms_ocall_sgx_init_quote_t * ms = NULL;
	void * old_ustack = sgx_prepare_ustack();

	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	if (target_info == NULL) {
		ms->ms_target_info = NULL;
	}

	ms->ms_target_info = sgx_alloc_on_ustack_aligned(sizeof(sgx_target_info_t), alignof(sgx_target_info_t));

	status = sgx_ocall(OCALL_SGX_INIT_QUOTE, ms);

	if (status == SGX_SUCCESS) {
		if (target_info) {
			sgx_copy_to_enclave(target_info, sizeof(sgx_target_info_t), ms->ms_target_info, sizeof(sgx_target_info_t));
		}
	}
	sgx_reset_ustack(old_ustack);
	return status;
}

int ocall_remote_attestation(sgx_quote_t *quote, uint32_t *quote_size, sgx_report_t* report, const struct ra_tls_options* opts) {

	sgx_status_t status = SGX_SUCCESS;
	ms_ocall_remote_attestation_t * ms;
	void * old_ustack = sgx_prepare_ustack();

	ms = sgx_alloc_on_ustack_aligned(sizeof(*ms), alignof(*ms));

	memcpy(&ms->ms_spid, &opts->spid, sizeof(sgx_spid_t));
	memcpy(&ms->ms_quote_type, &opts->quote_type, sizeof(sgx_quote_sign_type_t));

	ms->ms_report = sgx_copy_to_ustack(report, sizeof(sgx_report_t));

	status = sgx_ocall(OCALL_REMOTE_ATTESTATION, ms);
	*quote_size = ms->ms_quote_size;

	sgx_copy_to_enclave(quote, *quote_size, ms->ms_quote, *quote_size);
	sgx_reset_ustack(old_ustack);

	return status;
}

