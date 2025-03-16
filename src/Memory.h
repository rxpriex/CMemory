#ifndef MEMORY
#define MEMORY

#define ALLOCATED_MEMORY 0b00000001
#define CACHED_MEMORY 0b00000010

#define rxalloc(value) assign_memory(value,0);
#define rxfree unassign_memory

typedef unsigned long long int uint64;
typedef unsigned long int uint32;
typedef unsigned int uint16;
typedef unsigned char uint8;

#ifdef __cplusplus
extern "C" {
#endif

    extern void* __attribute__((malloc)) global_alloc(uint64);
    extern void __attribute__((always_inline)) global_dealloc(void*);

    extern void* __attribute__((malloc)) assign_memory(uint32, uint8);
    extern void __attribute__((nonnull(1))) unassign_memory(void*);

    extern uint32 __attribute__((always_inline)) get_assigned_memory_size(uint8);
    extern void __attribute__((always_inline)) get_debug_outputs();

#ifdef __cplusplus
}
#endif

#endif