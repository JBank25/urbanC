#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "ERROR: module name required"
    exit 1
fi

MODULE_PATH="../src/$1"
TEST_DIR="$MODULE_PATH/test"

CMAKE_TEXT="find_package(unity)

add_executable(\${MODULE_TEST_TARGET} <TEST_SOURCE_FILE_HERE>.c)

target_link_libraries(\${MODULE_TEST_TARGET} PUBLIC
                        \${MODULE_TARGET}
                        # MODULE DEPENDENCIES HERE
                        unity::unity)

add_test(\${MODULE_TEST_SUITE} \${MODULE_TEST_TARGET})"

TEST_FILE_TEXT=""
echo "$CMAKE_TEXT"
# Create test directory if it doesn't exist
mkdir -p "$TEST_DIR"

touch "$TEST_DIR/$1_tests.c"

# Create CMakeLists.txt in the test directory
echo "$CMAKE_TEXT" > "$TEST_DIR/CMakeLists.txt"

echo "Created $TEST_DIR/CMakeLists.txt with the following content:"
cat "$TEST_DIR/CMakeLists.txt"

