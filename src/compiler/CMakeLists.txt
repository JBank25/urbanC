# module1/CMakeLists.txt (repeat similar structure for module2, etc.)
add_library(compiler_lib
    src/compiler.c
    # Add more source files as needed
)

target_include_directories(compiler_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependency on header only subdir
target_link_libraries(compiler_lib
    PUBLIC object_lib scanner_lib # or PRIVATE depending on your needs
)
