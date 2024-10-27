# module1/CMakeLists.txt (repeat similar structure for module2, etc.)
add_library(object_lib
    src/object.c
    # Add more source files as needed
)

target_include_directories(object_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependency on header only subdir
target_link_libraries(object_lib
    PUBLIC chunk_lib table_lib # or PRIVATE depending on your needs
)