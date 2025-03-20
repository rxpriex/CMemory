#include "Memory.h"
#include <stdio.h>  // For printf
#include <stdlib.h> // For exit
#include <string.h> // For memset


#ifdef _WIN32
#include <windows.h> // For VirtualAlloc, VirtualFree
#elif defined(__linux__)
#include <sys/mman.h> // For mmap, munmap
#include <unistd.h>   // For POSIX compatibility

#endif

/** @brief Maximum size of cached memory in bytes */
#define CACHE_LIMIT 256

/** @brief Head of the allocated memory list */
static _memory_block *heap = NULL;
/** @brief Head of the cached memory list */
static _memory_block *fm_cache = NULL;
/** @brief Tail of the allocated memory list */
static _memory_block *_HP = NULL;
/** @brief Tail of the cached memory list */
static _memory_block *_FMCP = NULL;

/**
 * @brief Handles critical errors by printing a message and exiting the program.
 * @param err Error code to exit with.
 */
void __attribute__((noreturn)) critical_error(short err) {
  printf("Critical error encountered, exiting with code %hd\n", err);
  exit(err);
}

/**
 * @brief Calculates the total size of memory blocks based on the given flags.
 * @param flags Flags indicating which memory pool to measure (ALLOCATED_MEMORY
 * or CACHED_MEMORY).
 * @return Total size of the memory blocks in the specified pool in bytes.
 */
uint32 __attribute__((always_inline)) get_assigned_memory_size(uint8 flags) {
  _memory_block *current = (flags & ALLOCATED_MEMORY) ? heap
                           : (flags & CACHED_MEMORY)  ? fm_cache
                                                      : NULL;
  uint32 total_size = 0;

  while (current != NULL) {
    total_size += current->block_size;
    current = current->next;
  }

  return total_size;
}

/**
 * @brief Allocates memory using platform-specific functions.
 * @param size Size of memory to allocate in bytes.
 * @return Pointer to allocated memory, or NULL on failure.
 */
void *__attribute__((malloc)) global_alloc(uint64 size) {
#ifdef _WIN32
  return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#elif defined(__linux__)
  void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (ptr == MAP_FAILED)
    return NULL;
  return ptr;
#else
  printf("Failed: Unsupported OS\n");
  critical_error(1);
#endif
}

/**
 * @brief Adds a memory block to the end of a linked list.
 * @param target_block Memory block to add.
 * @param target_pointer Pointer to the tail of the list.
 * @param target_pool Pointer to the head of the list.
 */
static void assign_memblock_to_target(_memory_block *target_block,
                                      _memory_block **target_pointer,
                                      _memory_block **target_pool) {
  target_block->next = NULL;
  target_block->prev = *target_pointer;

  if (*target_pool == NULL) {
    *target_pool = target_block;
  } else {
    (*target_pointer)->next = target_block;
  }
  *target_pointer = target_block;
}

/**
 * @brief Allocates memory and adds it to the heap list.
 * @param size_to_allocate Size of memory to allocate in bytes.
 * @param allocation_flags Flags for allocation (stored but unused).
 * @return Pointer to the usable allocated memory.
 */
void *__attribute__((malloc)) assign_memory(uint32 size_to_allocate,
                                            uint8 allocation_flags) {
  // Allocate space for both the block header and the usable memory
  void *memory = global_alloc(size_to_allocate + sizeof(_memory_block));
  if (memory == NULL) {
    critical_error(2); // Allocation failure
  }

  _memory_block *block = (_memory_block *)memory;
  block->block_size = size_to_allocate;
  block->flags = allocation_flags;

  assign_memblock_to_target(block, &_HP, &heap);
  return (void *)(block + 1); // Return pointer to memory after the header
}

/**
 * @brief Deallocates memory using platform-specific functions.
 * @param ptr Pointer to memory to deallocate (including header).
 * @note For Linux, size is unavailable here, which is a limitation.
 */
void global_dealloc(void *ptr) {
#ifdef _WIN32
  VirtualFree(ptr, 0, MEM_RELEASE);
#elif defined(__linux__)
  // TODO: Size is required for munmap but not passed.
  // As a workaround, assume the caller ensures correct size management.
  // Ideally, global_dealloc should take a size or block pointer.
  _memory_block *block = (_memory_block *)ptr;
  munmap(ptr, block->block_size + sizeof(_memory_block));
#else
  printf("Failed: Unsupported OS\n");
  critical_error(1);
#endif
}

/**
 * @brief Removes a memory block from a linked list.
 * @param target_block Memory block to remove.
 * @param target_pointer Pointer to the tail of the list.
 * @param target_pool Pointer to the head of the list.
 */
static void unassign_memblock_from_target(_memory_block *target_block,
                                          _memory_block **target_pointer,
                                          _memory_block **target_pool) {
  if (target_block->prev != NULL) {
    target_block->prev->next = target_block->next;
  } else {
    *target_pool = target_block->next;
  }

  if (target_block->next != NULL) {
    target_block->next->prev = target_block->prev;
  } else {
    *target_pointer = target_block->prev;
  }

  target_block->prev = NULL;
  target_block->next = NULL;
}

/**
 * @brief Deallocates memory and optionally caches it.
 * @param memblock_to_delete Pointer to the memory to deallocate (user pointer).
 */
void __attribute__((nonnull(1))) unassign_memory(void *memblock_to_delete) {
  // Get the block header, which is before the user pointer
  _memory_block *target_block =
      (_memory_block *)((uint8 *)memblock_to_delete - sizeof(_memory_block));

  // Clear the user-accessible memory
  memset(memblock_to_delete, 0, target_block->block_size);

  // Remove block from the heap list
  unassign_memblock_from_target(target_block, &_HP, &heap);

  // Check if block can be cached
  if ((get_assigned_memory_size(CACHED_MEMORY) + target_block->block_size) <=
      CACHE_LIMIT) {
    assign_memblock_to_target(target_block, &_FMCP, &fm_cache);
  } else {
    global_dealloc(target_block);
  }
}