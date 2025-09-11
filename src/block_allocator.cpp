#include "block_allocator.hpp"

#include <iostream>
#include <sys/mman.h>
#include <mutex>

blockAllocator::blockAllocator( size_t req_block_size, size_t req_block_count )
    : block_size( req_block_size ), block_count( req_block_count ) {

  if ( req_block_size == 0 || block_count == 0 ) {
    throw std::runtime_error( "Failed to initialize allocator: block size and block count cannot be null" );
  }

  size_t pointer_size = sizeof( void * );
  if ( block_size < pointer_size ) {
    block_size = pointer_size;
  }

  size_t aligned_size = ( block_size + sizeof( void * ) - 1 ) & ~( sizeof( void * ) - 1 );
  if ( aligned_size != block_size ) {
    block_size = aligned_size;
  }

  root_mem = mmap( NULL, block_size * block_count, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0 );
  if ( root_mem == MAP_FAILED ) {
    throw std::runtime_error( "Failed to initialize allocator: mmap failed (MAP_FAILED)" );
  }

  current_node = ( blockNode * )root_mem;
  blocks_used  = 0;

  for ( size_t i = 0; i < block_count; i++ ) {
    size_t      offset = i * block_size;
    blockNode * block  = ( blockNode * )( ( char * )root_mem + offset );
    block->next        = current_node;
    current_node       = block;
  }
};

blockAllocator::~blockAllocator() {
  if ( root_mem != nullptr ) {
    size_t memory_size = block_size * block_count;
    munmap( root_mem, memory_size );
    root_mem = nullptr;
  }
};

void * blockAllocator::allocate() {
  std::lock_guard< std::mutex > alloc_guard( alloc_mtx );

  if ( !current_node ) {
    throw std::runtime_error( "Failed to allocate memory: allocator internal error" );
  }

  if ( blocks_used >= block_count ) {
    throw std::runtime_error( "Failed to allocate memory: all blocks used" );
  }

  void * block_ptr = ( void * )current_node;
  current_node     = current_node->next;
  blocks_used++;

  return block_ptr;
};

void blockAllocator::deallocate( void *& block_ptr ) {
  std::lock_guard< std::mutex > alloc_guard( alloc_mtx );

  if ( block_ptr == nullptr ) {
    throw std::runtime_error( "Failed to deallocate memory: block pointer is nullptr" );
  }

  blockNode * node_ptr = ( blockNode * )block_ptr;
  node_ptr->next       = current_node;
  current_node         = node_ptr;
  block_ptr            = nullptr;
  blocks_used--;
};

void blockAllocator::reset() {
  current_node = ( blockNode * )root_mem;
  blocks_used  = 0;

  for ( size_t i = 0; i < block_count; i++ ) {
    size_t      offset = i * block_size;
    blockNode * block  = ( blockNode * )( ( char * )root_mem + offset );
    block->next        = current_node;
    current_node       = block;
  }
};
