cmake_minimum_required(VERSION 3.21)
project(pacman LANGUAGES CXX)

file(GLOB_RECURSE SUBDIRECTORIES RELATIVE ${CMAKE_SOURCE_DIR} */)

add_subdirectory(headers)
add_subdirectory(src)

message(STATUS "Source files being compiled:")
foreach(source_file ${SOURCE_FILES})
    message(STATUS "  ${source_file}")
endforeach()

add_executable(pacman ${SOURCE_FILES})

target_link_libraries(pacman sfml-graphics sfml-window sfml-system)

install(TARGETS pacman)