#include "sgx_interface.h"
#include "sgx/sgx_arch.h"

#define GSGX_FILE "/dev/gsgx"
#define ISGX_FILE "/dev/isgx"

unsigned long pagesize  = PRESET_PAGESIZE;
unsigned long pageshift = PRESET_PAGESIZE - 1;
unsigned long pagemask  = ~(PRESET_PAGESIZE - 1);

int gsgx_device = -1;
int isgx_device = -1;

void * zero_page;


int open_gsgx(void) {

	gsgx_device = open(GSGX_FILE, O_RDWR, 0); // /dev/gsgx
	if (gsgx_device < 0) {
		printf("Cannot open device " GSGX_FILE ". Please make sure the"
				" \'graphene_sgx\' kernel module is loaded.\n");
		return gsgx_device;
	}

	isgx_device = open(ISGX_FILE, O_RDWR, 0); // /dev/isgx
	if (isgx_device < 0) {
		printf("Cannot open device " ISGX_FILE ". Please make sure the"
				" Intel SGX kernel module is loaded.\n");
		return isgx_device;
	}
	return 0;
}

static inline void cpuid(uint32_t leaf, uint32_t subleaf, uint32_t info[4])
{
    __asm__ volatile("cpuid"
                 : "=a"(info[0]),
                   "=b"(info[1]),
                   "=c"(info[2]),
                   "=d"(info[3])
                 : "a"(leaf),
                   "c"(subleaf));
}

static size_t get_ssaframesize (uint64_t xfrm)
{
    uint32_t cpuinfo[4];
    uint64_t xfrm_ex;
    uint32_t xsave_size = 0;

    cpuid(SE_LEAF, 1, cpuinfo);
    xfrm_ex = ((uint64_t) cpuinfo[3] << 32) + cpuinfo[2];

    for (int i = 2; i < 64; i++)
        if ((xfrm & (1ULL << i)) || (xfrm_ex & (1ULL << i))) {
            cpuid(0xd, i, cpuinfo);
            if (cpuinfo[0] + cpuinfo[1] > xsave_size)
                xsave_size = cpuinfo[0] + cpuinfo[1];
        }

    return ALLOC_ALIGNUP(xsave_size + sizeof(sgx_arch_gpr_t) + 1);
}


int read_enclave_sigstruct(int sigfile, sgx_arch_sigstruct_t* sig) {

	struct stat stat;
	int ret, bytes;
	ret = fstat(sigfile, &stat);
	if (ret < 0)
		return ret;

	if (stat.st_size < (long int)sizeof(sgx_arch_sigstruct_t)) {
		printf("size of sigstruct size does not match\n");
		return -EINVAL;
	}

	bytes = read(sigfile, sig, sizeof(sgx_arch_sigstruct_t));
	if (bytes < 0)
		return bytes;

	return 0;
}


int read_enclave_token(int token_file, sgx_arch_token_t* token) {
	struct stat stat;
	int ret, bytes;
	ret = fstat(token_file, &stat);
	if (ret < 0)
		return ret;

	if (stat.st_size != sizeof(sgx_arch_token_t)) {
		printf("size of token size does not match\n");
		return -EINVAL;
	}

	bytes = read(token_file, token, sizeof(sgx_arch_token_t));
	if (bytes < 0)
		return bytes;

	return 0;
}


int create_enclave(sgx_arch_secs_t* secs, unsigned long baseaddr, unsigned long size, sgx_arch_token_t* token) {

    int flags = MAP_SHARED;
	int ret;
	uint64_t addr;
    struct sgx_enclave_create param;


    if (!zero_page) {
        zero_page = (void *) mmap(NULL, pagesize, PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS|MAP_POPULATE, -1, 0);
        if ((signed long)zero_page == -1) {
            return -ENOMEM;
		}
    }

    memset(secs, 0, sizeof(sgx_arch_secs_t));
    secs->size = pagesize;
    while (secs->size < size)
        secs->size <<= 1;
    secs->ssaframesize = get_ssaframesize(token->attributes.xfrm) / pagesize;
    secs->miscselect = token->miscselect_mask;
    memcpy(secs->mrenclave, token->mrenclave, sizeof(sgx_arch_hash_t));
    memcpy(&secs->attributes, &token->attributes,
           sizeof(sgx_arch_attributes_t));
    /* Do not initialize secs->mrsigner and secs->mrenclave here as they are
     * not used by ECREATE to populate the internal SECS. SECS's mrenclave is
     * computed dynamically and SECS's mrsigner is populated based on the
     * SIGSTRUCT during EINIT (see pp21 for ECREATE and pp34 for
     * EINIT in https://software.intel.com/sites/default/files/managed/48/88/329298-002.pdf). */

    if (baseaddr) {
        secs->baseaddr = (uint64_t) baseaddr & ~(secs->size - 1);
    } else {
        secs->baseaddr = ENCLAVE_HIGH_ADDRESS;
    }

    secs->baseaddr = 0;
    
    // isgx_device -> baseaddr
    addr = (uint64_t) mmap((void*)(secs->baseaddr), secs->size, PROT_READ|PROT_WRITE|PROT_EXEC, flags|MAP_FIXED|MAP_POPULATE, isgx_device, 0);

    if ((unsigned long)(addr) >= (unsigned long)-4095L) {
        if ((long)addr == -1 && (flags | MAP_FIXED))
            printf("Permission denied on mapping enclave. "
                       "You may need to set sysctl vm.mmap_min_addr to zero\n");

        perror("enclave ECREATE failed in allocating EPC memory");
        return -errno;
    }

    secs->baseaddr = addr;

    // here
    //struct sgx_enclave_create param = {
    //    .src = (uint64_t) secs,
    //};
    param.src = (uint64_t) secs;
    ret = ioctl(isgx_device, SGX_IOC_ENCLAVE_CREATE, &param);

    if (ret < 0) {
        perror("enclave ECREATE failed in enclave creation ioctl");
        return ret;
    }

    if (ret) {
        printf( "enclave ECREATE failed - %d\n", ret);
        return -EPERM;
    }

    secs->attributes.flags |= SGX_FLAGS_INITTED;

    printf( "enclave created:\n");
    printf( "    base:         0x%016lx\n", secs->baseaddr);
    printf( "    size:         0x%016lx\n", secs->size);
    

//#define SE_LEAF 0x12printf( "    attr:         0x%016lx\n", secs->attributes.flags);
    printf( "    xfrm:         0x%016lx\n", secs->attributes.xfrm);
    printf( "    ssaframesize: %d\n",      secs->ssaframesize);
    printf( "    isvprodid:    0x%08x\n",   secs->isvprodid);
    printf( "    isvsvn:       0x%08x\n",   secs->isvsvn);

    return 0;

}


int add_pages_to_enclave(sgx_arch_secs_t * secs,
                         void * addr, void * user_addr,
                         unsigned long size,
                         enum sgx_page_type type, int prot,
                         bool skip_eextend,
                         const char * comment) {
    sgx_arch_secinfo_t secinfo;
    int ret;
    char p[4] = "---";
    const char * t;
    const char * m;
    struct sgx_enclave_add_page param;
	uint64_t added_size;

    memset(&secinfo, 0, sizeof(sgx_arch_secinfo_t));

    switch (type) {
        case SGX_PAGE_SECS:
            return -EPERM;
        case SGX_PAGE_TCS:
            secinfo.flags |= SGX_SECINFO_FLAGS_TCS;
            break;
        case SGX_PAGE_REG:
            secinfo.flags |= SGX_SECINFO_FLAGS_REG;
            if (prot & PROT_READ)
                secinfo.flags |= SGX_SECINFO_FLAGS_R;
            if (prot & PROT_WRITE)
                secinfo.flags |= SGX_SECINFO_FLAGS_W;
            if (prot & PROT_EXEC)
                secinfo.flags |= SGX_SECINFO_FLAGS_X;
            break;
    }

    t = (type == SGX_PAGE_TCS) ? "TCS" : "REG";
    m = skip_eextend ? "" : " measured";

    if (type == SGX_PAGE_REG) {
        if (prot & PROT_READ)
            p[0] = 'R';
        if (prot & PROT_WRITE)
            p[1] = 'W';
        if (prot & PROT_EXEC)
            p[2] = 'X';
    }

    if (size == (long unsigned int) pagesize)
        printf( "adding page  to enclave: %016lx [%s:%s] (%s)%s\n",
                (long unsigned int)addr, t, p, comment, m);
    else
        printf( "adding pages to enclave: %016lx-%016lx [%s:%s] (%s)%s\n",
                (long unsigned int)addr, (unsigned long) addr + size, t, p, comment, m);


    param.addr = secs->baseaddr + (uint64_t) addr;
    //param.src = (uint64_t) (user_addr ? : zero_page);
    if (user_addr != 0)
    	param.src = (uint64_t) user_addr;
	else
		param.src = (uint64_t) zero_page;
    param.secinfo = (uint64_t) &secinfo;
    //param.mrmask = skip_eextend ? 0 : (uint16_t) -1;
    if (skip_eextend != 0)
    	param.mrmask = 0;
	else
		param.mrmask = (uint16_t)-1;

    added_size = 0;
    while (added_size < size) {
        ret = ioctl(isgx_device, SGX_IOC_ENCLAVE_ADD_PAGE, &param);
        if (ret < 0) {
            printf( "Enclave add page returned %d\n", ret);
            return ret;
        }

        param.addr += pagesize;
        if (param.src != (uint64_t) zero_page) param.src += pagesize;
        added_size += pagesize;
    }

    return 0;
}



int init_enclave(struct NaClApp *nap) {

	//int enclave_image;
	int sigfile, tokenfile, exec_image;
	int ret;
	size_t enclave_size;
	unsigned long baseaddr;
	sgx_arch_sigstruct_t enclave_sigstruct;
    sgx_arch_token_t enclave_token;

	if (open_gsgx() < 0)
		return -1;

	//enclave_image = open((nap->sgx).enclave_img, O_RDONLY);
    sigfile = open((nap->sgx).sig_file, O_RDONLY);

    if (sigfile < 0) {
    	printf("error while reading sigfile\n");
    	return -1;
	}

    tokenfile = open((nap->sgx).token_file, O_RDONLY);
	if (tokenfile < 0) {
		printf("error while reading tokenfile\n");
		return -1;
	}

	exec_image = open((nap->sgx).nacl_file, O_RDONLY);
	if (exec_image < 0) {
		printf("error while reading exe image\n");
		return -1;
	}

	enclave_size = (size_t) 1024*1024*1024*64;
	baseaddr = 0;
	
	ret = read_enclave_sigstruct(sigfile, &enclave_sigstruct);
	if (ret < 0) {
		printf("error while reading sigstruct\n");
		return -1;
	}

	ret = read_enclave_token(tokenfile, &enclave_token);
	if (ret < 0) {
		printf("error while reading token\n");
		return -1;
	}

	ret = create_enclave(&((nap->sgx).enclave_secs), baseaddr, enclave_size, (sgx_arch_token_t *) &enclave_token);
	if (ret < 0) {
		printf("error while creating enclave\n");
		return -1;
	}

	return 0;
}

int destroy_enclave(void * base_addr, size_t length)
{
	int ret;
    printf( "destroying enclave...\n");

    ret = munmap(base_addr, length);

    if (ret < 0) {
        printf( "enclave EDESTROY failed\n");
        return ret;
    }

    return 0;
}
