find_package(unity)

add_executable(${MODULE_TEST_TARGET} value_tests.c)

target_link_libraries(${MODULE_TEST_TARGET} PUBLIC
                        ${MODULE_TARGET}
                        # MODULE DEPENDENCIES HERE
                        unity::unity)

add_test(${MODULE_TEST_SUITE} ${MODULE_TEST_TARGET})
