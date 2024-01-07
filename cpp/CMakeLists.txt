cmake_minimum_required (VERSION 3.11)
project (primes)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_C_FLAGS "-std=c99")

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" )

if(UNIX)
    add_compile_options(-O3 -Wall -lm -pthread)
elseif(WIN32)
    add_compile_options(/Ot /W4 /MT)
endif()

add_executable(naive naive.c)
add_executable(sieve sieve.cpp Bitset.h Sieve.h little_endian.h)
add_executable(segmented segmented.cpp Bitset.h Sieve.h little_endian.h ArgParser.h Event.h)

if(UNIX)
    target_link_libraries(segmented pthread)
    target_link_libraries(naive m)
elseif(WIN32)
    target_link_libraries(sieve ws2_32)
    target_link_libraries(segmented ws2_32)
endif()
