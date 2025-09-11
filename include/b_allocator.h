/**
 * @file b_allocator.h
 * @brief Header file for the blockAllocator class.
 * @author Kuzhbanov Mikhail
 */

#ifndef B_ALLOCATOR_H // BLOCK_ALLOCATOR_H
#define B_ALLOCATOR_H // BLOCK_ALLOCATOR_H

#include <stddef.h>
#include <pthread.h>

typedef struct bNode bNode;

/**
 * @brief A node in the linked list of allocated blocks.
 */

typedef struct bNode {
  bNode * next; ///< Pointer to the next block node in the free block list
} bNode;

/**
 * @brief blockAllocator structure description.
 */
typedef struct bAllocator {
  int             block_size;   ///< Block size in bytes.
  int             block_count;  ///< Number of allocated blocks.
  int             blocks_used;  ///< Number of already used blocks.
  void *          root_mem;     ///< Pointer to the beginning of the memory pool.
  bNode *         current_node; ///< Pointer to the current node in free blocks list.
  pthread_mutex_t alloc_mtx;    ///< Mutex, used to protect access on allocate and deallocate
} bAllocator;

/**
 * @brief Initializes a blockAllocator instance.
 *
 * @param allocator A pointer to the blockAllocator to initialize.
 * @param req_block_size The requested block size in bytes.
 * @param req_block_count The total number of blocks to allocate.
 *
 * @return 0 on success
 *
 * EINVAL on wrong req_block_size or req_block_count value
 *
 * ENOMEM on memory allocation failure
 */
int bAllocatorInitialize( bAllocator * allocator, size_t req_block_size, size_t req_block_count );

/**
 * @brief Deinitializes a blockAllocator instance, munmap memory.
 *
 * @param allocator A pointer to the blockAllocator to deinitialize.
 *
 * @return 0 on success
 *
 * EINVAL on allocator not exist
 */
int bAllocatorDeinitialize( bAllocator * allocator );

/**
 * @brief Allocates a single block of memory from the blockAllocator.
 *
 * @param allocator A pointer to the blockAllocator.
 *
 * @return A pointer to the allocated block, or nullptr if no blocks are available.
 */
void * bAllocatorAllocate( bAllocator * allocator );

/**
 * @brief Deallocates a previously allocated block back to the blockAllocator.
 *
 * @param allocator A pointer to the blockAllocator.
 * @param block_ptr A pointer to the block to deallocate.  Must have been previously allocated by `bAllocatorAllocate`.
 */
void bAllocatorDeallocate( bAllocator * allocator, void * block_ptr );

#endif // B_ALLOCATOR_H
