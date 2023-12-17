project (naive)

set(CMAKE_C_FLAGS "-std=c99")

if(UNIX)
    add_compile_options(-O3 -Wall -lm)
elseif(WIN32)
    add_compile_options(/Ot /W4 /MT)
endif()

add_executable(naive naive.c)

if(UNIX)
    target_link_libraries(naive m)
endif()
