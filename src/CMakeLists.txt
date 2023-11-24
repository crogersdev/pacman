# List all source files in the "src" directory
file(GLOB_RECURSE SOURCE_FILES *.cpp)

set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)