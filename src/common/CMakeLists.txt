
# module1/CMakeLists.txt (repeat similar structure for module2, etc.)
add_library(common_lib INTERFACE
    # Add more source files as needed
)

target_include_directories(common_lib
   INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)