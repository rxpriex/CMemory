#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h> // For standard fixed-width integer types

/** @brief Flag indicating memory is allocated */
#define ALLOCATED_MEMORY 0b00000001
/** @brief Flag indicating memory is cached */
#define CACHED_MEMORY 0b00000010

/** @brief Custom type for 64-bit unsigned integer */
typedef unsigned long long int uint64;
/** @brief Custom type for 32-bit unsigned integer */
typedef unsigned long int uint32;
/** @brief Custom type for 16-bit unsigned integer */
typedef unsigned int uint16;
/** @brief Custom type for 8-bit unsigned integer (replaces 'byte') */
typedef unsigned char uint8;

/**
 * @brief Structure representing a memory block in the allocator.
 */
typedef struct _memory_block {
  uint32 block_size; /**< Size of the memory block (excluding header) */
  struct _memory_block *next; /**< Pointer to the next block in the list */
  struct _memory_block *prev; /**< Pointer to the previous block in the list */
  uint8 flags; /**< Flags indicating block status (e.g., ALLOCATED_MEMORY) */
} _memory_block;

#ifdef __cplusplus
extern "C" { // Ensure C linkage for C++ compatibility
#endif

/**
 * @brief Allocates memory using platform-specific functions.
 * @param size Size of memory to allocate in bytes.
 * @return Pointer to allocated memory, or NULL on failure.
 */
extern void *global_alloc(uint64 size) __attribute__((malloc));

/**
 * @brief Deallocates memory using platform-specific functions.
 * @param ptr Pointer to memory to deallocate.
 */
extern void global_dealloc(void *ptr);

/**
 * @brief Allocates memory and adds it to the heap list.
 * @param size Size of memory to allocate in bytes.
 * @param flags Allocation flags (currently unused).
 * @return Pointer to allocated memory usable by the caller.
 */
extern void *assign_memory(uint32 size, uint8 flags) __attribute__((malloc));

/**
 * @brief Deallocates memory, possibly caching it for reuse.
 * @param ptr Pointer to memory to deallocate (must not be NULL).
 */
extern void unassign_memory(void *ptr) __attribute__((nonnull(1)));

/**
 * @brief Calculates total size of memory blocks with specified flags.
 * @param flags Flags to filter memory pools (e.g., ALLOCATED_MEMORY).
 * @return Total size of matching memory blocks in bytes.
 */
extern uint32 get_assigned_memory_size(uint8 flags)
    __attribute__((always_inline));

#ifdef __cplusplus
}
#endif

/** @brief Macro to allocate memory with default flags */
#define rxalloc(value) assign_memory(value, 0)
/** @brief Macro to deallocate memory */
#define rxfree unassign_memory

#endif // MEMORY_H