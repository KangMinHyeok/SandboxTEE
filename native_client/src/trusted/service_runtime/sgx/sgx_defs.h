#ifndef SGX_DEFS_H
#define SGX_DEFS_H


#define SGX_HAS_FSGSBASE (1)

#define MACBUF_SIZE ((sizeof(sgx_arch_mac_t)*2)+1) 

#define SGX_EXCEPTION_HARDWARE      3UL
#define SGX_EXCEPTION_SOFTWARE      6UL

#define SGX_EXCEPTION_VECTOR_DE     0UL  /* DIV and IDIV instructions */
#define SGX_EXCEPTION_VECTOR_DB     1UL  /* For Intel use only */
#define SGX_EXCEPTION_VECTOR_BP     3UL  /* INT 3 instruction */
#define SGX_EXCEPTION_VECTOR_BR     5UL  /* BOUND instruction */
#define SGX_EXCEPTION_VECTOR_UD     6UL  /* UD2 instruction or reserved opcodes */
#define SGX_EXCEPTION_VECTOR_MF    16UL  /* x87 FPU floating-point or WAIT/FWAIT instruction */
#define SGX_EXCEPTION_VECTOR_AC    17UL  /* Any data reference in memory */
#define SGX_EXCEPTION_VECTOR_XM    19UL  /* Any SIMD floating-point exceptions */

#define SGX_SECINFO_FLAGS_R             0x001
#define SGX_SECINFO_FLAGS_W             0x002
#define SGX_SECINFO_FLAGS_X             0x004
#define SGX_SECINFO_FLAGS_SECS          0x000
#define SGX_SECINFO_FLAGS_TCS           0x100
#define SGX_SECINFO_FLAGS_REG           0x200

#define SGX_REPORT_SIGNED_SIZE  384

#define EENTER      2
#define ERESUME     3
#define EDBGRD      4
#define EDBGWR      5

#define EREPORT     0
#define EGETKEY     1
#define EEXIT       4

#define LAUNCH_KEY          0
#define PROVISION_KEY       1
#define PROVISION_SEAL_KEY  2
#define REPORT_KEY          3
#define SEAL_KEY            4

#define KEYPOLICY_MRENCLAVE     1
#define KEYPOLICY_MRSIGNER      2

#define SGX_GPR_RAX             0x00
#define SGX_GPR_RCX             0x08
#define SGX_GPR_RDX             0x10
#define SGX_GPR_RBX             0x18
#define SGX_GPR_RSP             0x20
#define SGX_GPR_RBP             0x28
#define SGX_GPR_RSI             0x30
#define SGX_GPR_RDI             0x38
#define SGX_GPR_R8              0x40
#define SGX_GPR_R9              0x48
#define SGX_GPR_R10             0x50
#define SGX_GPR_R11             0x58
#define SGX_GPR_R12             0x60
#define SGX_GPR_R13             0x68
#define SGX_GPR_R14             0x70
#define SGX_GPR_R15             0x78
#define SGX_GPR_RFLAGS          0x80
#define SGX_GPR_RIP             0x88
#define SGX_GPR_EXITINFO        0xa0

/* update these constant according to struct enclave_tls */
#define SGX_ENCLAVE_SIZE            0x00
#define SGX_TCS_OFFSET              0x08
#define SGX_INITIAL_STACK_OFFSET    0x10
#define SGX_AEP                     0x18
#define SGX_SSA                     0x20
#define SGX_GPR                     0x28
#define SGX_EXIT_TARGET             0x30
#define SGX_FSBASE                  0x38
#define SGX_STACK                   0x40
#define SGX_USTACK_TOP              0x48
#define SGX_USTACK                  0x50
#define SGX_ISMAIN                  0x58
#define SGX_THREAD                  0x5B


#define XSAVE_SIZE  512
#define XSAVE_ALIGN 0xffffffffffffffc0

#define STACK_ALIGN 0xfffffffffffffff0

#endif
