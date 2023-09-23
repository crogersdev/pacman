# Define the source files for the flappy target
set(SOURCES
    main.cpp
)

# Add the sources to the flappy target
add_executable(flappy ${SOURCES})

# Link against any necessary libraries (e.g., SFML)
target_link_libraries(flappy sfml-graphics sfml-window sfml-system)
