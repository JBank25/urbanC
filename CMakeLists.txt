cmake_minimum_required(VERSION 3.23)
project(urbanC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "Buidling with CMake version: ${CMAKE_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# TODO: Might need later
# find_package()

# TODO: Might need later
enable_testing()

add_executable(${PROJECT_NAME} ./src/main.c)

add_subdirectory(src/)

target_link_libraries(${PROJECT_NAME} PUBLIC 
                        # Put module names here
                        Chunk
                        Common
                        Compiler
                        Debug
                        Memory
                        Object
                        Scanner
                        Table
                        Value
                        Vm
                        )

target_include_directories(${PROJECT_NAME} PUBLIC
                    "${PROJECT_BINARY_DIR}"
                    # Put module paths here
                    "${PROJECT_SOURCE_DIR}/chunk/"
                    "${PROJECT_SOURCE_DIR}/common"
                    "${PROJECT_SOURCE_DIR}/compiler"
                    "${PROJECT_SOURCE_DIR}/debug"
                    "${PROJECT_SOURCE_DIR}/memory"
                    "${PROJECT_SOURCE_DIR}/object"
                    "${PROJECT_SOURCE_DIR}/scanner"
                    "${PROJECT_SOURCE_DIR}/table"
                    "${PROJECT_SOURCE_DIR}/value"
                    "${PROJECT_SOURCE_DIR}/vm"
)