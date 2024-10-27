# module1/CMakeLists.txt (repeat similar structure for module2, etc.)
add_library(memory_lib
    src/memory.c
    # Add more source files as needed
)

target_include_directories(memory_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependency on header only subdir
target_link_libraries(memory_lib
    PUBLIC common_lib object_lib vm_lib# or PRIVATE depending on your needs
)