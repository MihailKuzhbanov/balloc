cmake_minimum_required(VERSION 3.14)

project(balloc VERSION 1.0.2 DESCRIPTION "block allocator")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bins)

add_library(ba_c SHARED
    ${CMAKE_SOURCE_DIR}/src/b_allocator.c
)
set_target_properties(ba_c PROPERTIES VERSION 1.0.1 SOVERSION 1)

add_library(ba_cpp SHARED
    ${CMAKE_SOURCE_DIR}/src/block_allocator.cpp
)
set_target_properties(ba_cpp PROPERTIES VERSION 1.0.2 SOVERSION 1)

add_executable(ba_testc
    ${CMAKE_SOURCE_DIR}/tests/test.c
)

add_executable(ba_testcpp
    ${CMAKE_SOURCE_DIR}/tests/test.cpp
)

include_directories(PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(ba_testc pthread ba_c)
target_link_libraries(ba_testcpp ba_cpp)

enable_testing()
add_test(
  NAME ba_testc
  COMMAND $<TARGET_FILE:ba_testc>
)

add_test(
  NAME ba_testcpp
  COMMAND $<TARGET_FILE:ba_testcpp>
)

option(DOCS_REQUIRED "Build documentation using Doxygen" ON)
if(DOCS_REQUIRED)
    find_package(Doxygen REQUIRED)
    
    if(DOXYGEN_FOUND)        
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile")            
            add_custom_target(docs
                COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
        else()
            message(WARNING "Doxyfile not found in ${CMAKE_CURRENT_SOURCE_DIR}")
        endif()
    else()
        message(WARNING "Doxygen not found but DOCS_REQUIRED is ON")
    endif()
else()
    message(STATUS "Documentation generation is disabled (DOCS_REQUIRED=OFF)")
endif()


