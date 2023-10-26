cmake_minimum_required(VERSION 3.21)
project(pacman LANGUAGES CXX)

# file(GLOB_RECURSE SUBDIRECTORIES CMAKE_USE_RELATIVE_PATHS ${CMAKE_SOURCE_DIR} */)

# file(GLOB_RECURSE SUBDIRECTORIES RELATIVE ${CMAKE_SOURCE_DIR}/src "${CMAKE_SOURCE_DIR}/src/*/")
file(GLOB_RECURSE SUBDIRECTORIES LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/src/*")
set(LIBRARIES_TO_LINK)

# Print the list of subdirectories
message(STATUS "Subdirectories found:")
foreach(SUBDIR ${SUBDIRECTORIES})
    if(IS_DIRECTORY ${SUBDIR})
        message(STATUS "  Adding: ${SUBDIR}")
        add_subdirectory(${SUBDIR})
        list(APPEND LIBRARIES_TO_LINK ${SUBDIR})
    endif()
endforeach()

add_executable(pacman src/main.cpp ${SOURCE_FILES})

# having linker errors?  undefined references to things?  make sure
# the appropriate folder's named library from its CMakeLists.txt file
# is appended here
target_link_libraries(
    pacman
    sfml-graphics
    sfml-system
    sfml-window
    game-manager
    entities
)

install(TARGETS pacman)
