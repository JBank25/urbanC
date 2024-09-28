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
    int capacity;
    int count;
    Value *values;
} ValueArray;

bool valueEquals(Value a, Value b);
void initValueArray(ValueArray *array);
void writeValueArray(ValueArray *array, Value value);
void freeValueArray(ValueArray *array);
void printValue(Value value, uint16_t colorCode);