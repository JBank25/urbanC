find_package(unity)

add_executable(${MODULE_TEST_TARGET} chunk_tests.c)

target_link_libraries(${MODULE_TEST_TARGET} PUBLIC 
                        ${MODULE_TARGET}
                        Value
                        unity::unity)

add_test(${MODULE_TEST_SUITE} ${MODULE_TEST_TARGET})