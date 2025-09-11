#include "b_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int init_test( bAllocator * alloc ) {
  int size  = 0;
  int count = 0;
  int res   = 0;

  res = bAllocatorInitialize( alloc, size, count );
  if ( res == 0 ) {
    return 1;
  }

  size = 1;
  res  = bAllocatorInitialize( alloc, size, count );
  if ( res == 0 ) {
    return 1;
  }

  count = 8;
  res   = bAllocatorInitialize( alloc, size, count );
  if ( res ) {
    return 1;
  }

  return 0;
};

int alloc_test( bAllocator * alloc ) {
  for ( int i = 0; i < alloc->block_count; i++ ) {
    void * p = bAllocatorAllocate( alloc );
    if ( p ) {
      printf( "allocate succes: %p\n", p );
    }
    else {
      return 1;
    }
    bAllocatorDeallocate( alloc, p );
  }

  return 0;
};

int fill_test( bAllocator * alloc ) {
  int    blocks    = alloc->block_count;
  int ** block_ptr = malloc( blocks * alloc->block_count );

  for ( int i = 0; i < alloc->block_count / 2; i++ ) {
    block_ptr[i] = ( int * )bAllocatorAllocate( alloc );
    usleep( 1000 );
  }

  for ( int i = 0; i < alloc->block_count / 2; i++ ) {
    bAllocatorDeallocate( alloc, ( void * )block_ptr[i] );
  }

  free( block_ptr );

  return 0;
};

int multithread_test( void * alloc_ptr ) {
  bAllocator * alloc = ( bAllocator * )alloc_ptr;
  pthread_t    t1;
  pthread_t    t2;
  pthread_create( &t1, NULL, ( void * )fill_test, ( void * )alloc );
  pthread_create( &t2, NULL, ( void * )fill_test, ( void * )alloc );

  pthread_join( t1, NULL );
  pthread_join( t2, NULL );

  return 0;
};

int main( int argc, char ** argv ) {
  int res = 0;

  bAllocator * alloc = malloc( sizeof( bAllocator ) );
  res += init_test( alloc );
  res += alloc_test( alloc );
  res += fill_test( alloc );
  res += multithread_test( alloc );
  bAllocatorDeinitialize( alloc );

  free( alloc );

  return res;
}