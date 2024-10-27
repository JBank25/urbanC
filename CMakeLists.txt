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
set(DEBUG_DIR "${CMAKE_SOURCE_DIR}/debug_lib")
set(OBJECT_DIR "${CMAKE_SOURCE_DIR}/object_lib")
set(TABLE_DIR "${CMAKE_SOURCE_DIR}/table_lib")
set(MEMORY_DIR "${CMAKE_SOURCE_DIR}/memory_lib")
set(VM_DIR "${CMAKE_SOURCE_DIR}/vm_lib")
set(COMPILER_DIR "${CMAKE_SOURCE_DIR}/compiler_lib")
set(SCANNER_DIR "${CMAKE_SOURCE_DIR}/scanner_lib")

# Add more module directories as needed

# Add subdirectories containing source files
add_subdirectory(chunk)
add_subdirectory(common)
add_subdirectory(value)
add_subdirectory(debug)
add_subdirectory(object)
add_subdirectory(table)
add_subdirectory(memory)
add_subdirectory(vm)
add_subdirectory(compiler)
add_subdirectory(scanner)
# Add more subdirectories as needed

# Create the main executable
add_executable(prog main.c)

# Link the libraries to the main executable
target_link_libraries(prog
    chunk_lib
    value_lib
    debug_lib
    object_lib
    table_lib
    memory_lib
    vm_lib
    compiler_lib
    scanner_lib
)

# Include directories for header files
target_include_directories(prog PRIVATE
    ${CHUNK_DIR}/include
    ${COMMON_DIR}/include
    ${VALUE_DIR}/include
    ${DEBUG_DIR}/include
    ${OBJECT_DIR}/include
    ${TABLE_DIR}/include
    ${MEMORY_DIR}/include
    ${VM_DIR}/include
    ${COMPILER_DIR}/include
    ${SCANNER_DIR}/include
)