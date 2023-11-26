get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

file(GLOB FILES
    "*.cpp"
    "*.hpp"
)

# Add a library with the derived folder name
add_library(${FOLDER_NAME} ${FILES})

# Include the "code" directory for headers
target_include_directories(${FOLDER_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})