#include "b_allocator.h"

#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>

int bAllocatorInitialize( bAllocator * allocator, size_t req_block_size, size_t req_block_count ) {
  if ( req_block_size == 0 || req_block_count == 0 ) {
    printf( "error: invalid arguments, block_size and block count cannot be null\n" );
    return EINVAL;
  }

  allocator->block_size  = req_block_size;
  allocator->block_count = req_block_count;

  size_t pointer_size = sizeof( void * );
  if ( allocator->block_size < pointer_size ) {
    allocator->block_size = pointer_size;
    printf( "warning: block size changed to %d bytes\n", allocator->block_size );
  }

  size_t aligned_size = ( allocator->block_size + sizeof( void * ) - 1 ) & ~( sizeof( void * ) - 1 );
  if ( aligned_size != allocator->block_size ) {
    allocator->block_size = aligned_size;
    printf( "warning: block size changed to %d bytes\n", allocator->block_size );
  }

  allocator->root_mem = mmap( NULL, allocator->block_size * allocator->block_count, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0 );
  if ( allocator->root_mem == MAP_FAILED ) {
    printf( "error: memory pool allocated failed\n" );
    return ENOMEM;
  }

  allocator->current_node = ( bNode * )( allocator->root_mem );
  allocator->blocks_used  = 0;

  for ( size_t i = 0; i < allocator->block_count; i++ ) {
    size_t  offset          = i * ( allocator->block_size );
    bNode * block           = ( bNode * )( ( char * )( allocator->root_mem ) + offset );
    block->next             = allocator->current_node;
    allocator->current_node = block;
  }

  pthread_mutex_init( &allocator->alloc_mtx, NULL );

  return 0;
};

int bAllocatorDeinitialize( bAllocator * allocator ) {
  if ( allocator->root_mem != NULL ) {
    size_t memory_size = allocator->block_size * allocator->block_count;
    munmap( allocator->root_mem, memory_size );
    allocator->root_mem = NULL;
    pthread_mutex_destroy( &allocator->alloc_mtx );
  }
  else {
    printf( "warning: allocator already deinitialized\n" );
    return EINVAL;
  }

  return 0;
};

void * bAllocatorAllocate( bAllocator * allocator ) {
  pthread_mutex_lock( &allocator->alloc_mtx );

  if ( allocator->blocks_used >= allocator->block_count ) {
    printf( "error: allocate block failed\n" );
    pthread_mutex_unlock( &allocator->alloc_mtx );
    return NULL;
  }

  void * block_ptr        = ( void * )( allocator->current_node );
  allocator->current_node = allocator->current_node->next;
  allocator->blocks_used++;
  pthread_mutex_unlock( &allocator->alloc_mtx );

  return block_ptr;
};

void bAllocatorDeallocate( bAllocator * allocator, void * block_ptr ) {
  if ( block_ptr == NULL ) {
    printf( "error: deallocate block failed (nullptr)\n" );
    return;
  }

  pthread_mutex_lock( &allocator->alloc_mtx );
  bNode * node_ptr        = ( bNode * )block_ptr;
  node_ptr->next          = allocator->current_node;
  allocator->current_node = node_ptr;
  block_ptr               = NULL;
  allocator->blocks_used--;
  pthread_mutex_unlock( &allocator->alloc_mtx );
};