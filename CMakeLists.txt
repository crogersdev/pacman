cmake_minimum_required(VERSION 3.21)
project(pacman LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SFML 3 COMPONENTS Graphics Audio Network)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified...")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Setting CXX Flags for Debug build...")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "Setting the CXX Flags for Release build...")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
endif()

file(GLOB_RECURSE SUBDIRECTORIES LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/src/*")
file(COPY res DESTINATION ${CMAKE_BINARY_DIR})

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
# TRICKY: order here matters
target_link_libraries(
    pacman
    SFML::Graphics
    SFML::System
    SFML::Window 
    game-manager
    entities
    helpers
    # state  # todo: add me in later
)

install(TARGETS pacman)
