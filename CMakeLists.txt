cmake_minimum_required(VERSION 3.21)
project(pacman LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SUBDIRECTORIES LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/src/*")

message(STATUS "Subdirectories found:")
foreach(SUBDIR ${SUBDIRECTORIES})
    if(IS_DIRECTORY ${SUBDIR})
        message(STATUS "  Adding: ${SUBDIR}")
        add_subdirectory(${SUBDIR})
    endif()
endforeach()

add_executable(pacman src/main.cpp ${SOURCE_FILES})

find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    set(CMAKE_CXX_CPPCHECK "${CPPCHECK}" "--enable=all" "--inconclusive")
endif()

add_custom_target(run-cppcheck
    COMMAND ${CMAKE_CXX_CPPCHECK} --project=compile_commands.json
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running cppcheck on the project"
)

add_dependencies(pacman run-cppcheck)

# having linker errors?  undefined references to things?  make sure
# the appropriate folder's named library from its CMakeLists.txt file
# is appended here
# TODO: can we auto-detect these...?
target_link_libraries(
    pacman
    sfml-graphics
    sfml-system
    sfml-window
    game-manager
    entities
    helpers
)

install(TARGETS pacman)
