#include "Memory.h"

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

#define CACHE_LIMIT 256

typedef struct _memory_block {
    uint32 block_size;

    struct _memory_block *next;
    struct _memory_block *prev;

    byte flags;
} _memory_block;

_memory_block *heap, *fm_cache;
_memory_block *_HP, *_FMCP;

void __attribute__((noreturn)) critical_error(short err) {
    printf("Critical error encountered, exiting with code %hd", err);
    exit(err);
}

void print_allocated(_memory_block *inpt) {
    while (inpt != NULL) {
        printf("MemoryBlock:[%lubit] at location:[%p] with flags:[%hhu]\n", inpt->block_size * 8, inpt, inpt->flags);
        inpt = inpt->next;
    }
}

uint32 __attribute__((always_inline)) get_assigned_memory_size(uint8 flags) {
    _memory_block *start_iteration = (flags & ALLOCATED_MEMORY) == 1 ? heap
                             : (flags & CACHED_MEMORY) == 1 ? fm_cache : NULL;
    uint32 size = 0;
    while (start_iteration != NULL) {
        size+=start_iteration->block_size;
        start_iteration = start_iteration->next;
    }

    return size;
}

void __attribute__((always_inline)) get_debug_outputs() {
    printf("----Heap----\n");
    print_allocated(heap);
    printf("----Cache----\n");
    print_allocated(fm_cache);
}

void *__attribute__((malloc)) global_alloc(uint64 size) {
#ifdef _WIN32
    return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#elif defined(__linux__)
    return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#else
    printf("Failed: Unsupported OS");
    critical_error(1);
#endif
}

void assign_memblock_to_target(
    _memory_block *target_block,
    _memory_block **target_pointer,
    _memory_block **target_pool) {
    if (*target_pool == 0) {
        *target_pool = target_block;
        *target_pointer = target_block;
    } else {
        (*target_pointer)->next = target_block;
        target_block->prev = *target_pointer;
        *target_pointer = target_block;
    }
}

void *__attribute__((malloc)) assign_memory(uint32 size_to_allocate, uint8 allocation_flags) {
    void *memory = global_alloc(size_to_allocate + sizeof(_memory_block));
    _memory_block *n_block = (_memory_block *)memory;
    n_block->block_size = size_to_allocate;
    assign_memblock_to_target(n_block, &_HP, &heap);

    return (void *)(n_block + 1);
}

void global_dealloc(void *target_addr) {
#ifdef _WIN32
    VirtualFree(target_addr, 0, MEM_RELEASE);
#elif defined(__linux__)
    munmap(target_addr, sizeof(memory));
#else
    printf("Failed: Unsupported OS");
    critical_error(1);
#endif
}

void unassign_memblock_from_target(
    _memory_block *target_block,
    _memory_block **target_pool) {
    _memory_block *temp_block = *target_pool;
    while (temp_block != NULL) {
        if (temp_block == target_block) {
            if (temp_block == *target_pool) {
                if (temp_block->next != NULL) {
                    *target_pool = (*target_pool)->next;
                } else {
                    *target_pool = NULL;
                }
            } else {
                temp_block->prev->next = temp_block->next;
                if (temp_block->next != NULL) {
                    temp_block->next->prev = temp_block->prev;
                }
            }
            target_block->prev = NULL;
            target_block->next = NULL;
            return;
        }
        temp_block = temp_block->next;
    }
    critical_error(5);
}

void __attribute__((nonnull(1))) unassign_memory(void *memblock_to_delete) {
    _memory_block *target_block = (_memory_block *)((byte *)memblock_to_delete - sizeof(_memory_block));
    memset((void *)((byte *)memblock_to_delete + sizeof(_memory_block)), 0, target_block->block_size);

    unassign_memblock_from_target(target_block, &heap);
    if((get_assigned_memory_size(CACHED_MEMORY) + target_block->block_size) <= CACHE_LIMIT){
        assign_memblock_to_target(target_block, &_FMCP, &fm_cache);
    }else{
        global_dealloc(target_block);
    }
}