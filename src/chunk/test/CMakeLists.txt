find_package(unity)
find_package(fff)

add_executable(${MODULE_TEST_TARGET} chunk_tests.c)

target_link_libraries(${MODULE_TEST_TARGET} PUBLIC 
                        ${MODULE_TARGET}
                        Memory
                        unity::unity
                        fff::fff)

add_test(${MODULE_TEST_SUITE} ${MODULE_TEST_TARGET})