#pragma once

#include "common.h"

typedef double Value;

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

void initValueArray(ValueArray *array);
void writeValueArray(ValueArray *array, Value value);
void freeValueArray(ValueArray *array);
void printValue(Value value);
