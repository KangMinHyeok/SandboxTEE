#include "native_client/src/trusted/stdlib/memory.h"
#include "native_client/src/include/build_config.h"
#include "native_client/src/trusted/stdlib/api.h"

#include "native_client/src/trusted/stdlib/stddef.h"
#include "native_client/src/trusted/stdlib/errno.h"
//#include "native_client/src/trusted/stdlib/assert.h"
#include <assert.h>
#include <stdint.h>

#ifndef UNIT
#define UNIT 16
#endif
#ifndef IB
#define IB 4
#endif

struct group {
	struct meta *meta;
	unsigned char active_idx:5;
	char pad[UNIT - sizeof(struct meta *) - 1];
	unsigned char storage[];
};

struct meta {
	struct meta *prev, *next;
	struct group *mem;
	volatile int avail_mask, freed_mask;
	uintptr_t last_idx:5;
	uintptr_t freeable:1;
	uintptr_t sizeclass:6;
	uintptr_t maplen:8*sizeof(uintptr_t)-12;
};

struct meta_area {
	uint64_t check;
	struct meta_area *next;
	int nslots;
	struct meta slots[];
};

struct malloc_context {
	uint64_t secret;
#ifndef PAGESIZE
	size_t pagesize;
#endif
	int init_done;
	unsigned mmap_counter;
	struct meta *free_meta_head;
	struct meta *avail_meta;
	size_t avail_meta_count, avail_meta_area_count, meta_alloc_shift;
	struct meta_area *meta_area_head, *meta_area_tail;
	unsigned char *avail_meta_areas;
	struct meta *active[48];
	size_t usage_by_class[48];
	uint8_t unmap_seq[32], bounces[32];
	uint8_t seq;
	uintptr_t brk;
};


const uint16_t size_classes[] = {
	1, 2, 3, 4, 5, 6, 7, 8,
	9, 10, 12, 15,
	18, 20, 25, 31,
	36, 42, 50, 63,
	72, 84, 102, 127,
	146, 170, 204, 255,
	292, 340, 409, 511,
	584, 682, 818, 1023,
	1169, 1364, 1637, 2047,
	2340, 2730, 3276, 4095,
	4680, 5460, 6552, 8191,
};

static inline size_t get_stride(const struct meta *g)
{
	if (!g->last_idx && g->maplen) {
		return g->maplen*4096UL - UNIT;
	} else {
		return UNIT*size_classes[g->sizeclass];
	}
}

static inline void set_size(unsigned char *p, unsigned char *end, size_t n)
{
	int reserved = end-p-n;
	if (reserved) end[-reserved] = 0;
	if (reserved >= 5) {
		*(uint32_t *)(end-4) = reserved;
		end[-5] = 0;
		reserved = 5;
	}
	p[-3] = (p[-3]&31) + (reserved<<5);
}


static inline int get_slot_index(const unsigned char *p)
{
	return p[-3] & 31;
}

static inline struct meta *get_meta(const unsigned char *p)
{
	assert(!((uintptr_t)p & 15));
	int offset = *(const uint16_t *)(p - 2);
	int index = get_slot_index(p);
	if (p[-4]) {
		assert(!offset);
		offset = *(uint32_t *)(p - 8);
		assert(offset > 0xffff);
	}
	const struct group *base = (const void *)(p - UNIT*offset - UNIT);
	const struct meta *meta = base->meta;
	assert(meta->mem == base);
	assert(index <= meta->last_idx);
	assert(!(meta->avail_mask & (1u<<index)));
	assert(!(meta->freed_mask & (1u<<index)));
	//const struct meta_area *area = (void *)((uintptr_t)meta & -4096);
	//assert(area->check == ctx.secret);
	if (meta->sizeclass < 48) {
		assert(offset >= size_classes[meta->sizeclass]*index);
		assert(offset < size_classes[meta->sizeclass]*(index+1));
	} else {
		assert(meta->sizeclass == 63);
	}
	if (meta->maplen) {
		assert((long unsigned int)offset <= (long unsigned int)meta->maplen*4096UL/UNIT - 1);
	}
	return (struct meta *)meta;
}

void *aligned_alloc(size_t align, size_t len){
	if ((align & -align) != align) {
		errno = EINVAL;
		return 0;
	}

	if (len > SIZE_MAX - align || align >= (1ULL<<31)*UNIT) {
		errno = ENOMEM;
		return 0;
	}
	/*
	if (DISABLE_ALIGNED_ALLOC) {
		errno = ENOMEM;
		return 0;
	}
	*/

	if (align <= UNIT) align = UNIT;

	unsigned char *p = (unsigned char *)malloc(len + align - UNIT);
	struct meta *g = get_meta(p);
	int idx = get_slot_index(p);
	size_t stride = get_stride(g);
	unsigned char *start = g->mem->storage + stride*idx;
	unsigned char *end = g->mem->storage + stride*(idx+1) - IB;
	size_t adj = -(uintptr_t)p & (align-1);

	if (!adj) {
		set_size(p, end, len);
		return p;
	}
	p += adj;
	uint32_t offset = (size_t)(p-g->mem->storage)/UNIT;
	if (offset <= 0xffff) {
		*(uint16_t *)(p-2) = offset;
		p[-4] = 0;
	} else {
		// use a 32-bit offset if 16-bit doesn't fit. for this,
		// 16-bit field must be zero, [-4] byte nonzero.
		*(uint16_t *)(p-2) = 0;
		*(uint32_t *)(p-8) = offset;
		p[-4] = 1;
	}
	p[-3] = idx;
	set_size(p, end, len);
	// store offset to aligned enframing. this facilitates cycling
	// offset and also iteration of heap for debugging/measurement.
	// for extreme overalignment it won't fit but these are classless
	// allocations anyway.
	*(uint16_t *)(start - 2) = (size_t)(p-start)/UNIT;
	start[-3] = 7<<5;
	return p;
}

void *memalign(size_t align, size_t len){
    return aligned_alloc(align, len);
}

