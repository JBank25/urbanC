# module1/CMakeLists.txt (repeat similar structure for module2, etc.)
add_library(chunk_lib
    src/chunk.c
    # Add more source files as needed
)

target_include_directories(chunk_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependency on header only subdir
target_link_libraries(chunk_lib
    PUBLIC common_lib value_lib # or PRIVATE depending on your needs
)