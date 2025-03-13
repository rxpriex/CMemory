#include "Memory.h"

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

typedef struct _memory_block{
    uint64 block_size;
    struct _memory_block* next;
    struct _memory_block* prev;
} _memory_block;

_memory_block* heap,reserve;
_memory_block* _HP,_RP;

void __attribute__((noreturn)) critical_error(short err) { exit(err); }

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
    _memory_block* target_block,
    _memory_block* target_pointer,
    _memory_block* target_pool){
        if(target_pool == 0){
            target_pool = target_block;
            target_pointer = target_block;
        }else{
            target_pointer->next = target_block;
            target_block->prev = target_pointer;
            target_pointer = target_block;
        }
}

void* __attribute__((malloc)) assign_memory(uint64 size_to_allocate){
    void* memory = global_alloc(size_to_allocate+sizeof(_memory_block));
    _memory_block* n_block = (_memory_block*)memory;
    n_block->block_size = size_to_allocate;
    assign_memblock_to_target(n_block,heap,_HP);

    return (void*)(n_block+sizeof (_memory_block));
}

void global_dealloc(void* target_addr){
#ifdef _WIN32
	VirtualFree(target_addr, 0, MEM_RELEASE);
#elif defined(__linux__)
	munmap(target_addr, sizeof(memory));
#else
	printf("Failed: Unsupported OS");
	critical_error(1);
#endif
}

void __attribute__((nonnull(1))) unassign_memory(void* memblock_to_delete){
    _memory_block* target_block = (_memory_block*)(memblock_to_delete-sizeof(_memory_block));
    printf("sizeof block %llu",target_block->block_size);
}