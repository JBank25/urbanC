# module1/CMakeLists.txt (repeat similar structure for module2, etc.)
add_library(debug_lib
    src/debug.c
    # Add more source files as needed
)

target_include_directories(debug_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Dependency on header only subdir
# it looks like this is only needed to resolve the headers included in the include/*.h
# here for example I do not need to link all the .h's in my src/*.c
target_link_libraries(debug_lib
    PUBLIC chunk_lib # or PRIVATE depending on your needs
)