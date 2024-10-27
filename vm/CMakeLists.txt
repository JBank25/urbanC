# module1/CMakeLists.txt (repeat similar structure for module2, etc.)
add_library(vm_lib
    src/vm.c
    # Add more source files as needed
)

target_include_directories(vm_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependency on header only subdir
target_link_libraries(vm_lib
    PUBLIC debug_lib object_lib memory_lib compiler_lib# or PRIVATE depending on your needs
)