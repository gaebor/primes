cmake_minimum_required (VERSION 3.1)
project (primes)

set (CMAKE_CXX_STANDARD 11)

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" )

add_executable(naive naive.c)
add_executable(sieve sieve.cpp Bitset.h Sieve.h little_endian.h)
add_executable(segmented segmented.cpp Bitset.h Sieve.h little_endian.h ArgParser.h Event.h)

if(UNIX)
    add_compile_options("-march=native" "-Wall" "-pthread")
    target_link_libraries(segmented pthread)
elseif(WIN32)
    add_compile_options("/Ot" "/W4")
    target_link_libraries(sieve ws2_32)
    target_link_libraries(segmented ws2_32)
endif()
