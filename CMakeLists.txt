cmake_minimum_required(VERSION 3.21)
project(flappy LANGUAGES CXX)

add_subdirectory(headers)
add_subdirectory(src)

install(TARGETS flappy)