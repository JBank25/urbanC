# module1/CMakeLists.txt (repeat similar structure for module2, etc.)
add_library(table_lib
    src/table.c
    # Add more source files as needed
)

target_include_directories(table_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependency on header only subdir
target_link_libraries(table_lib
    PUBLIC debug_lib object_lib memory_lib# or PRIVATE depending on your needs
)