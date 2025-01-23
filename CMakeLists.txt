cmake_minimum_required(VERSION 3.23)
project(urbanC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "Buidling with CMake version: ${CMAKE_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# TODO: Might need later
# find_package()

# TODO: Might need later
# enable_testing()

add_executable(${PROJECT_NAME} ./main.cpp)

add_subdirectory(chunk/)
add_subdirectory(common/)
add_subdirectory(compiler/)
add_subdirectory(debug/)
add_subdirectory(memory/)
add_subdirectory(object/)
add_subdirectory(scanner/)
add_subdirectory(table/)
add_subdirectory(value/)
add_subdirectory(vm/)

target_link_libraries(${PROJECT_NAME} PUBLIC 
                        # Put module names here
                        NetCommon
                        NetServer
                        NetConnection
                        )

target_include_directories(${PROJECT_NAME} PUBLIC
                    "${PROJECT_BINARY_DIR}"
                    # Put module paths here
                    "${PROJECT_SOURCE_DIR}/src/NetCommon"
                    "${PROJECT_SOURCE_DIR}/src/NetServer"
                    "${PROJECT_SOURCE_DIR}/src/NetConnection"
)