cmake_minimum_required(VERSION 3.21)
project(pacman LANGUAGES CXX)

set(LIBRARIES_TO_LINK)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SUBDIRECTORIES LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/src/*")

message(STATUS "Subdirectories found:")
foreach(SUBDIR ${SUBDIRECTORIES})
    if(IS_DIRECTORY ${SUBDIR})
        message(STATUS "  Adding: ${SUBDIR}")
        add_subdirectory(${SUBDIR})
        list(APPEND LIBRARIES_TO_LINK ${SUBDIR})
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
target_link_libraries(
    pacman
    sfml-graphics
    sfml-system
    sfml-window
    game-manager
    entities
)

install(TARGETS pacman)
