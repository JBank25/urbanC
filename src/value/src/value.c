#include "debug.h"
#include "memory.h"
#include "object.h"
#include "value.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void Value_initValueArray(ValueArray *array)
{
    // TODO: error check if ptr is NULL
    array->count = 0;
    array->capacity = 0;
    array->values = NULL;
}

void writeValueArray(ValueArray *array, Value value)
{
    // grow array and copy values if necessary
    if (array->capacity < array->count + 1)
    {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values,
                                   oldCapacity, array->capacity);
    }

    array->values[array->count] = value; // write the value to the array
    array->count++;                      // increment count of vars in array in use
}

void freeValueArray(ValueArray *array)
{
    FREE_ARRAY(Value, array->values, array->capacity);
    Value_initValueArray(array);
}

void Value_printValue(Value value, uint16_t colorCode)
{
    char buffer[100];
    switch (value.type)
    {
    case VAL_BOOL:
        snprintf(buffer, sizeof(buffer), AS_BOOL(value) ? "true" : "false");
        Print_Color(buffer, colorCode);
        break;
    case VAL_NIL:
        snprintf(buffer, sizeof(buffer), "nil");
        Print_Color(buffer, colorCode);
        break;
    case VAL_NUMBER:
        snprintf(buffer, sizeof(buffer), "%g", AS_NUMBER(value));
        Print_Color(buffer, colorCode);
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
bool Value_valueEquals(Value a, Value b)
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
    case VAL_OBJ:
        return AS_OBJ(a) == AS_OBJ(b);
    default:
        return false;
    }
}