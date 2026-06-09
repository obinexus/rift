# CMakeLists.txt for log_analyzer example

# Add the log_analyzer executable
add_executable(log_analyzer main.c)

# Find the rift library
find_library(RIFT_LIBRARY NAMES rift PATHS ${CMAKE_CURRENT_BINARY_DIR}/../../build)

# Link the log_analyzer executable with the rift library
target_link_libraries(log_analyzer PRIVATE ${RIFT_LIBRARY})

# Include directories for the log_analyzer example
target_include_directories(log_analyzer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include/librift
)