
typedef struct {
    int lock;
//#ifdef DEBUG_SPINLOCKS_SHIM
    unsigned int owner;
//#endif // DEBUG_SPINLOCKS_SHIM
} spinlock_t;

#define SPINLOCK_UNLOCKED            0
#define SPINLOCK_LOCKED              1

static inline void spinlock_lock(spinlock_t* lock);
static inline void cpu_pause(void);
static inline void debug_spinlock_giveup_ownership(spinlock_t* lock);


static inline void spinlock_lock(spinlock_t* lock) {
    int val;

    /* First check if lock is already free. */
    if (__atomic_exchange_n(&lock->lock, SPINLOCK_LOCKED, __ATOMIC_ACQUIRE) == SPINLOCK_UNLOCKED) {
       // goto out;
    }

    do {
        /* This check imposes no inter-thread ordering, thus does not slow other threads. */
        while (__atomic_load_n(&lock->lock, __ATOMIC_RELAXED) != SPINLOCK_UNLOCKED)
            cpu_pause();
        /* Seen lock as free, check if it still is, this time with acquire semantics (but only
         * if we really take it). */
        val = SPINLOCK_UNLOCKED;
    } while (!__atomic_compare_exchange_n(&lock->lock, &val, SPINLOCK_LOCKED, /*weak=*/false,
                                          __ATOMIC_ACQUIRE, __ATOMIC_RELAXED));
   // out:
   // debug_spinlock_take_ownership(lock);
}

static inline void spinlock_unlock(spinlock_t* lock) {
    debug_spinlock_giveup_ownership(lock);
    __atomic_store_n(&lock->lock, SPINLOCK_UNLOCKED, __ATOMIC_RELEASE);
}

static inline void cpu_pause(void) {
    __asm__ volatile("pause");
}

/*static inline void debug_spinlock_take_ownership(spinlock_t* lock) {
    __atomic_store_n(&lock->owner, get_cur_tid(), __ATOMIC_RELAXED);
}    */

static inline void debug_spinlock_giveup_ownership(spinlock_t* lock) {
    __atomic_store_n(&lock->owner, SPINLOCK_UNLOCKED, __ATOMIC_RELAXED);
}
