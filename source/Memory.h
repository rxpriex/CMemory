#ifndef MEMORY
#define MEMORY

#define assign_memory rxalloc
#define unassign_memory rxfree

typedef unsigned long long int uint64;

#ifdef __cplusplus
extern "C" {
#endif

    extern void* __attribute__((malloc)) global_alloc(uint64);
    extern void __attribute__((always_inline)) global_dealloc(void*);

    extern void* __attribute__((malloc)) assign_memory(uint64);
    extern void __attribute__((nonnull(1))) unassign_memory(void*);

#ifdef __cplusplus
}
#endif

#endif