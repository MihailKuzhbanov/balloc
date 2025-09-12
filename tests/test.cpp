#include "block_allocator.hpp"
#include <vector>
#include <thread>
#include <chrono>

class LargeObject {
public:
  char data[64]{'a'};
};

void run_tests( std::vector< int > sizes, std::vector< int > counts ) {
  for ( auto siz : sizes ) {
    for ( auto cnt : counts ) {
      try {
        blockAllocator alloc( siz, cnt );
      } catch ( std::exception & e ) {
        std::cerr << "Test case on wrong arguments: " << e.what() << std::endl;
      }
    }
  }

  int            size_def  = counts.at( 0 );
  int            count_def = counts.at( 0 );
  blockAllocator alloc( size_def, count_def );
  try {
    for ( int i = 0; i < count_def + 1; i++ ) {
      auto p = alloc.allocate();
    }
  } catch ( std::exception & e ) {
    std::cerr << "Test cast on out of range allocate: " << e.what() << std::endl;
  }

  alloc.reset();

  // get raw pointer for placement new and call class constructor
  LargeObject * obj = new ( alloc.allocate() ) LargeObject();
  obj->~LargeObject();
  alloc.dealloc( obj );
  obj = nullptr;

  // get class pointer without constructor calling
  obj = alloc.alloc<LargeObject>();
  char old_val = obj->data[1];
  obj->data[1] = 'a';
  char new_val = obj->data[1];

  std::cout << "LargeObject pointer, without calling class constructor" << std::endl;
  std::cout << "LargeObject::data[1]: " << old_val << std::endl;

  std::thread * t1 = new std::thread( [&] {
    for ( int i = 0; i < 10000; i++ ) {
      auto p = alloc.allocate();
      alloc.deallocate( p );
    }
  } );

  for ( int i = 0; i < 10000; i++ ) {
    auto p = alloc.allocate();
    alloc.deallocate( p );
  };
  t1->join();
};

int main( int argc, char ** argv ) {
  run_tests( std::vector< int >{ 64, 1, 0 }, std::vector< int >{ 8, 1, 0 } );

  return 0;
}