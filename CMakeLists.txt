cmake_minimum_required(VERSION 3.0.0...3.20.0)

project(gc)

set(DEBUG on)
set(C_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_C_STANDARD 11)
set(BUILD_SHARED_LIBS off)

if(DEBUG)
    set(C_FLAGS "${C_FLAGS} -ggbd")
endif()

set(
    INC_DIR
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(main ${CMAKE_SOURCE_DIR}/example/main.c)
target_include_directories(main PUBLIC ${INC_DIR})
