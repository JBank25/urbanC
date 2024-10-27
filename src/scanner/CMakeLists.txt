# module1/CMakeLists.txt (repeat similar structure for module2, etc.)
add_library(scanner_lib
    src/scanner.c
    # Add more source files as needed
)

target_include_directories(scanner_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependency on header only subdir
target_link_libraries(scanner_lib
    PUBLIC common_lib # or PRIVATE depending on your needs
)
