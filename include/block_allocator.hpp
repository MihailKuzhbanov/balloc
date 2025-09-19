/**
 * @file block_allocator.hpp
 * @brief Header file for the blockAllocator class.
 * @author Kuzhbanov Mikhail
 */

#ifndef BLOCK_ALLOCATOR_HPP // BLOCK_ALLOCATOR_HPP
#define BLOCK_ALLOCATOR_HPP // BLOCK_ALLOCATOR_HPP
#include <iostream>
#include <sys/mman.h>
#include <mutex>

/**
 * @brief A node in the linked list of allocated blocks.
 */
class blockNode {
public:
  blockNode * next; ///< pointer on next available block
};

/**
 * @brief Class for managing a pool of memory blocks
 */
class blockAllocator {
public:
  /**
   * @brief Default constructor is deleted. Use the parameterized constructor instead.
   */
  blockAllocator() = delete;

  /**
   * @brief Copy constructor is deleted. Block allocators are not copyable.
   */
  blockAllocator( const blockAllocator & other ) = delete;

  /**
   * @brief Assignment operator is deleted. Block allocators are not assignable.
   */
  blockAllocator & operator=( const blockAllocator & other ) = delete;

  /**
   * @brief Constructs a new block allocator with the specified block size and count.
   *
   * @param req_block_size The requested size of block in bytes.
   * @param req_block_count The total number of blocks to allocate.
   */
  blockAllocator( size_t req_block_size, size_t req_block_count );

  /**
   * @brief Destructor for the block allocator. Releases all allocated memory.
   */
  ~blockAllocator();

  /**
   * @brief Allocates a single block of memory.
   *
   * @throws std::runtime_error()
   *
   * @return A pointer to the allocated block
   */
  void * allocate();

  /**
   * @brief Deallocates a previously allocated block of memory back to the pool.
   *
   * @throws std::runtime_error()
   *
   * @param block_ptr A reference to the pointer of the block to deallocate, set to nullptr after
   */
  void deallocate( void *& block_ptr );

  /**
   * @brief Soft reset the allocator, memory not munmapped
   */
  void reset();

  /**
   * @brief Template function to destroy object and deallocate memory block, set to nullptr after.
   *
   * @tparam T The type of the object being deallocated.
   * @param ptr A reference to the pointer of the object to be deallocated.
   */
  template < typename T > void destroy( T & ptr ) {
    void * p = static_cast< void * >( ptr );
    ptr.~T();
    deallocate( p );
    ptr = nullptr;
  };

  /**
   * @brief Template function to allocate and construct object on block of memory.
   *
   * @tparam T The type of object to be allocated.
   * @return A pointer to the newly allocated block, cast to the T type, not calls class constructor
   */
  template < typename T > T * create() {
    if (sizeof (T) > block_size) {
      throw std::runtime_error("sizeof class more than block_size. Cannot allocate block");
    }
    void * p   = allocate();
    T * ptr = new(p) T();

    return ptr;
  };

private:
  uint32_t block_size;  ///< block size in bytes
  uint32_t block_count; ///< allocated blocks
  uint32_t blocks_used; ///< used (unavailable to allocate) blocks

  void *      root_mem;     ///< pointer to beggining of reserved memory
  blockNode * current_node; ///< pointer to the current node in free blocks list
  std::mutex  alloc_mtx;    ///< mutex to protect on memory allocation/deallocation
};

#endif // BLOCK_ALLOCATOR_HPP