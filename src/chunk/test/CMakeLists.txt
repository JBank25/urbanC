set(CMAKE_FIND_DEBUG_MODE ON)
find_package(unity)
set(CMAKE_FIND_DEBUG_MODE OFF)


add_executable(${MODULE_TEST_TARGET} chunk_tests.c)

target_link_libraries(${MODULE_TEST_TARGET} PRIVATE 
                        ${MODULE_TARGET}
                        unity::unity)

add_test(${MODULE_TEST_SUITE} ${MODULE_TEST_TARGET})