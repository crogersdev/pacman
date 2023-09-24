# List all source files in the "src" directory
file(GLOB_RECURSE SOURCE_FILES *.cpp)

include_directories(${CMAKE_SOURCE_DIR}/state)

set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)