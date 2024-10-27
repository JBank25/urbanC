# Root CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(prog C)

# Set compiler flags to match your Makefile
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -pg -g -Wall")

# Specify the build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Create variables for source directories
set(CHUNK_DIR "${CMAKE_SOURCE_DIR}/chunk_lib")
set(COMMON_DIR "${CMAKE_SOURCE_DIR}/common_lib")
set(VALUE_DIR "${CMAKE_SOURCE_DIR}/value_lib")
# Add more module directories as needed

# Add subdirectories containing source files
add_subdirectory(chunk)
add_subdirectory(common)
add_subdirectory(value)
# Add more subdirectories as needed

# Create the main executable
add_executable(prog main.c)

# Link the libraries to the main executable
target_link_libraries(prog
    chunk_lib
    value_lib
)

# Include directories for header files
target_include_directories(prog PRIVATE
    ${CHUNK_DIR}/include
    ${COMMON_DIR}/include
    ${VALUE_DIR}/include
)