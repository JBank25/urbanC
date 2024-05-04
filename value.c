#include "memory.h"
#include "value.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * Initializes a ValueArray structure.
 *
 * This function initializes the given ValueArray structure by setting its count and capacity to 0,
 * and its values pointer to NULL.
 *
 * @param array A pointer to the ValueArray structure to be initialized.
 */
void initValueArray(ValueArray *array)
{
    // TODO: error check if ptr is NULL
    array->count = 0;
    array->capacity = 0;
    array->values = NULL;
}

/**
 * Writes a value to a ValueArray.
 *
 * @param array The ValueArray to write to.
 * @param value The value to write.
 */
void writeValueArray(ValueArray *array, Value value)
{
    if (array->capacity < array->count + 1)
    {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values,
                                   oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

/**
 * Frees the memory allocated for a ValueArray and initializes it to 0
 *
 * @param array The ValueArray to be freed.
 */
void freeValueArray(ValueArray *array)
{
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

void printValue(Value value)
{
    printf("%g", AS_NUMBER(value));
}