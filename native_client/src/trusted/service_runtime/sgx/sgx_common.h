#ifndef _SGX_COMMON_
#define _SGX_COMMON_

// hmlee: commonly used macros

#define PRESET_PAGESIZE (1 << 12)
#define SGX_PAGESIZE (1 << 12)

extern unsigned long pagesize, pageshift, pagemask;

#define ALLOC_ALIGNUP(addr) \
	(pagesize ? (((unsigned long) (addr)) + pageshift) & pagemask : (unsigned long) (addr))
#define ALLOC_ALIGNDOWN(addr) \
	(pagesize ? ((unsigned long) (addr)) & pagemask : (unsigned long) (addr))
#define ALLOC_ALIGNED(addr) \
	(pagesize && ((unsigned long) (addr)) == (((unsigned long) (addr)) & pagemask))


#define ENCLAVE_HIGH_ADDRESS    (0x800000000)





#endif

