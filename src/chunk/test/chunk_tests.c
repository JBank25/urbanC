#include "chunk.h"

#include "unity.h"
#include "unity/unity.h"
#include "unity/unity_internals.h"
#include "unity_internals.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void dummy(void)
{
    Chunk testChunk = {0};
    testChunk.count = 1;
    Chunk_InitChunk(&testChunk);
    TEST_ASSERT_EQUAL(testChunk.count, 0);
}
 
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(dummy);
 
  return UNITY_END();
}