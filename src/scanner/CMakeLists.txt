include(Module.cmake)

message("*****************BUILDING NEW MODULE*****************")
message("Building module:				 				${MODULE_TARGET}")
message("Building module test target:	 				${MODULE_TEST_TARGET}")
message("Test Suite for ${MODULE_TARGET}: 				${MODULE_TEST_SUITE}")
message("*****************************************************")

add_library(${MODULE_TARGET} src/scanner.c)

target_link_libraries(${MODULE_TARGET}
    PRIVATE
    Common
    )

target_include_directories(${MODULE_TARGET}
        PUBLIC
		include/)
