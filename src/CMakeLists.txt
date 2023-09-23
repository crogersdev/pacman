# Define the source files for the pacman target
set(SOURCES
    main.cpp
)

# Add the sources to the pacman target
add_executable(pacman ${SOURCES})

# Link against any necessary libraries (e.g., SFML)
target_link_libraries(pacman sfml-graphics sfml-window sfml-system)
