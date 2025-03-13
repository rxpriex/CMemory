#include "Memory.h"

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

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

void* __attribute__((malloc)) assign_memory(uint64 size_to_allocate){
    return global_alloc(size_to_allocate);
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
    global_dealloc(memblock_to_delete);
}