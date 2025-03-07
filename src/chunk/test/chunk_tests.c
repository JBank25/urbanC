#include "chunk.h"
#include "memory.h"

#include "unity.h"
#include "unity/unity.h"
#include "unity/unity_internals.h"
#include "unity_internals.h"

#include "fff.h"
DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(Value_initValueArray, ValueArray *);
#include "value.h"

void setUp(void) 
{
    RESET_FAKE(Value_initValueArray);

}

void tearDown(void) 
{
}

void Test_Chunk_InitChunk(void)
{
    Chunk testChunk = {};
    Chunk_InitChunk(&testChunk);
    TEST_ASSERT_EQUAL(testChunk.count, 0);
    TEST_ASSERT_EQUAL(Value_initValueArray_fake.call_count, 0);
}

void Test_Chunk_AddConstant(void)
{
    Chunk testChunk = {0};
    Value testVal = {.type = VAL_NUMBER, .as = {.number = 0xF0F0}};

    int constantIdx = Chunk_AddConstant(&testChunk, testVal);
    TEST_ASSERT_EQUAL(testChunk.constants.count , 1);
    TEST_ASSERT_EQUAL(testChunk.constants.capacity, MIN_ARR_THRESHOLD);
    TEST_ASSERT_EQUAL(AS_NUMBER(testChunk.constants.values[constantIdx]), AS_NUMBER(testVal));
    TEST_ASSERT_EQUAL(constantIdx, 0);
}
 
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(Test_Chunk_InitChunk);
    RUN_TEST(Test_Chunk_AddConstant);
 
  return UNITY_END();
}