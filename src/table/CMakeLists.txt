include(Module.cmake)

message("*****************BUILDING NEW MODULE*****************")
message("Building module:				 				${MODULE_TARGET}")
message("Building module test target:	 				${MODULE_TEST_TARGET}")
message("Test Suite for ${MODULE_TARGET}: 				${MODULE_TEST_SUITE}")
message("*****************************************************")

add_library(${MODULE_TARGET} src/table.c)

target_link_libraries(${MODULE_TARGET}
    PUBLIC
    Memory
    Object
    Value
    )

target_include_directories(${MODULE_TARGET}
        PUBLIC
		include/)
