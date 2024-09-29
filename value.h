#pragma once

#include "common.h"

typedef struct Obj Obj;
typedef struct ObjString ObjString;

// Data types we'll support. Will serve as the tag
// in the "tagged" union
typedef enum
{
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_OBJ
} ValueType;

// TAGGED union
typedef struct
{
    ValueType type; // the type tag
    union           // union containing underlying value
    {
        bool boolean;
        double number;
        Obj *obj;
    } as; // reads nicely could be as number, as boolean, as string, etc
} Value;

// Macros to check an urbanC Values type
#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_OBJ(value) ((value).type == VAL_OBJ)

// Give Value tyoe produce a C type. Calls here MUST be guarded
// behing the IS_* macros
#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJ(value) ((value).as.obj)

// Macros to promote C val to urbanC val. Takes C type and
// produces the appropriate Value with tag and unerlying val
#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(object) ((Value){VAL_OBJ, {.obj = (Obj *)object}})

/**
 * Consider an snippet of code that looks like:
 * print 1;
 * print 2;
 *
 * Chunk would have two opcodes and 2 constants. The opcode must be
 * associated with the constant data. We will store operands alongside
 * out bytecode to ensure this functionality
 */

typedef struct
{
    int capacity;  // total capacity of the array
    int count;     // number of elements in the array currently in use
    Value *values; // dynamic array of values
} ValueArray;

bool valueEquals(Value a, Value b);

/**
 * Initializes a ValueArray structure.
 *
 * This function initializes the given ValueArray structure by setting its count and capacity to 0,
 * and its values pointer to NULL.
 *
 * @param array A pointer to the ValueArray structure to be initialized.
 */
void initValueArray(ValueArray *array);

/**
 * Writes a value to a ValueArray.
 *
 * @param array The ValueArray to write to.
 * @param value The value to write to the ValueArrays values array.
 */
void writeValueArray(ValueArray *array, Value value);

/**
 * Frees the memory allocated for a ValueArray and initializes it to 0
 *
 * @param array The ValueArray to be freed.
 */
void freeValueArray(ValueArray *array);

void printValue(Value value, uint16_t colorCode);