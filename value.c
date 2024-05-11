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
    switch (value.type)
    {
    case VAL_BOOL:
        printf(AS_BOOL(value) ? "true" : "false");
        break;
    case VAL_NIL:
        printf("nil");
        break;
    case VAL_NUMBER:
        printf("%g", AS_NUMBER(value));
        break;
    case VAL_OBJ:
        printObject(value);
        break;
    }
}

// Some languages have “implicit conversions” where values of different types
// may be considered equal if one can be converted to the other’s type.
// For example, the number 0 is equivalent to the string “0” in JavaScript.
// This looseness was a large enough source of pain that JS added a separate
// “strict equality” operator, ===.
bool valueEquals(Value a, Value b)
{
    // if types not the same, def not equal
    if (a.type != b.type)
    {
        return false;
    }

    // else unwrap the vals and compare them directly
    switch (a.type)
    {
    case VAL_BOOL:
        return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NIL:
        return true;
    case VAL_NUMBER:
        return AS_NUMBER(a) == AS_NUMBER(b);
    default:
        return false;
    }
}