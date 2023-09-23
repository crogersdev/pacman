# List all source files in the "src" directory
file(GLOB SOURCE_FILES *.cpp)

include_directories(${CMAKE_SOURCE_DIR}/headers)

set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)
#src/main.cpp) target_sources(pacman PRIVATE ${SOURCE_FILES} ${HEADER_FILES})