#pragma once

#include "common.h"
#include "value.h"

typedef struct
{
    ObjString *key; // key is always a string
    Value value;
} Entry;

typedef struct
{
    int count;    // Number of allocated spaces actually being used
    int capacity; // ALLOCATED size
    Entry *entries;
} Table;

void initTable(Table *table);
void freeTable(Table *table);
