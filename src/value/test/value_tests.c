#include "unity.h"
#include "unity/unity.h"
#include "unity/unity_internals.h"
#include "unity_internals.h"

void setUp(void) 
{
}

void tearDown(void) 
{
}

void Test_Value_Example(void)
{
    TEST_ASSERT_EQUAL(1,1);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(Test_Value_Example);
 
    return UNITY_END();
}
